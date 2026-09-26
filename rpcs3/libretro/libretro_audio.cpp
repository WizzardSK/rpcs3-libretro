#include "stdafx.h"

#include "libretro_audio.h"
#include "libretro_core.h"

#include <algorithm>
#include <cstring>
#include <chrono>

static LibretroAudioBackend* s_audio_backend = nullptr;

LibretroAudioBackend* get_libretro_audio_backend()
{
    return s_audio_backend;
}

void libretro_audio_process(retro_audio_sample_batch_t audio_batch_cb)
{
    if (!audio_batch_cb || !s_audio_backend)
        return;

    // As much audio as has played in real time since the last retro_run.
    //
    // Handing over 2048 frames every run made audio sync hold RetroArch to
    // 48000 / 2048 = 23.4 runs a second - the 24 fps NNshi saw in every game.
    // A fixed 800 frames (48000 / 60) fixed that but is still wrong: RPCS3 is
    // clocked by real time, not by retro_run. Whenever the frontend ran more
    // than 60 times a second it drained cellAudio's ring faster than real
    // time, and with buffering on cellAudio answers a draining ring by
    // shortening its period - down to half - so the game's music ran ahead of
    // its pictures (Project Diva, 5-10 % fast). A sound card drains the ring
    // at a steady 48 kHz whatever the frame rate; this does the same.
    static constexpr double SAMPLE_RATE = 48000.0;
    // A run after a pause or a long stall would otherwise ask for everything
    // it missed. The emulator was paused too, so there is nothing to catch up.
    static constexpr double MAX_FRAMES_PER_RUN = SAMPLE_RATE / 10;
    static constexpr size_t FRAMES_PER_BATCH = 512;
    alignas(16) int16_t buffer[FRAMES_PER_BATCH * 2]; // 16-byte aligned for SIMD

    static std::chrono::steady_clock::time_point s_last_run;
    static double s_owed = 0.0; // fractional frames carried to the next run

    const auto now = std::chrono::steady_clock::now();
    if (s_last_run == std::chrono::steady_clock::time_point{})
        s_last_run = now - std::chrono::microseconds(1'000'000 / 60);
    s_owed += std::chrono::duration<double>(now - s_last_run).count() * SAMPLE_RATE;
    s_last_run = now;
    s_owed = std::min(s_owed, MAX_FRAMES_PER_RUN);

    const size_t due = static_cast<size_t>(s_owed);
    s_owed -= static_cast<double>(due);

    size_t remaining = due;
    while (remaining > 0)
    {
        const size_t want = std::min(remaining, FRAMES_PER_BATCH);
        const size_t frames = s_audio_backend->GetSamples(buffer, want);
        if (frames > 0)
            audio_batch_cb(buffer, frames);
        remaining -= frames;
        if (frames < want)
            break; // nothing more to give this run
    }

    // Without buffering cellAudio produces in 256-sample blocks on its own
    // clock, so a run often lands just before the next block is out. What was
    // due but not there yet is owed to the next run - dropping it made every
    // short run permanent, and the audio that came late piled up unplayed.
    // Bounded, so a pause does not come back as a burst.
    s_owed = std::min(s_owed + static_cast<double>(remaining), SAMPLE_RATE / 20);
}

LibretroAudioBackend::LibretroAudioBackend()
{
    s_audio_backend = this;
}

LibretroAudioBackend::~LibretroAudioBackend()
{
    Close();
    if (s_audio_backend == this)
        s_audio_backend = nullptr;
}

bool LibretroAudioBackend::Open(std::string_view dev_id, AudioFreq freq, AudioSampleSize sample_size, AudioChannelCnt ch_cnt, audio_channel_layout layout)
{
    (void)dev_id;

    std::lock_guard lock(m_mutex);

    // Set base class members - use what the config passes us
    // DO NOT force S16 here - let the config control this via raw.convert_to_s16
    // We handle float->s16 conversion ourselves in GetSamples() if needed
    m_sampling_rate = freq;
    m_sample_size = sample_size;  // Accept what config gives us (usually FLOAT)
    m_channels = static_cast<u32>(ch_cnt);
    m_layout = layout;

    // Ring buffer for ~500ms of audio - larger buffer reduces stutter
    // At 48kHz stereo float = 48000 * 2 * 4 * 0.5 = 192KB
    const size_t bytes_per_second = static_cast<size_t>(freq) * static_cast<u32>(ch_cnt) * get_sample_size();
    m_ring_buffer_bytes.resize(bytes_per_second / 2, 0);  // 500ms buffer
    m_ring_read_pos = 0;
    m_ring_write_pos = 0;
    m_ring_size = 0;

    m_initialized = true;
    return true;
}

void LibretroAudioBackend::Close()
{
    std::lock_guard lock(m_mutex);

    m_playing = false;
    m_initialized = false;
    m_ring_buffer_bytes.clear();
    m_ring_read_pos = 0;
    m_ring_write_pos = 0;
    m_ring_size = 0;
}

void LibretroAudioBackend::SetWriteCallback(std::function<u32(u32, void*)> cb)
{
    std::lock_guard lock(m_mutex);
    m_write_callback = std::move(cb);
}

void LibretroAudioBackend::SetStateCallback(std::function<void(AudioStateEvent)> cb)
{
    std::lock_guard lock(m_mutex);
    m_state_callback = std::move(cb);
}

f64 LibretroAudioBackend::GetCallbackFrameLen()
{
    // How long cellAudio has to hold on to audio before it is collected - it
    // sizes its ring from this. Collection happens once per retro_run, not
    // every 5.3 ms like a sound card callback, and with buffering off the ring
    // was 5.3 ms + two 256-sample blocks = 16 ms: less than one 60 Hz frame.
    // Blocks that did not fit were dropped, RetroArch starved (NNshi: 98 %
    // underrun at 60 fps, fine from 66 fps up). Three frames leave room for a
    // late run; the ring is emptied every run, so this adds no latency.
    return 3.0 / 60.0;
}

void LibretroAudioBackend::Play()
{
    m_playing = true;
}

void LibretroAudioBackend::Pause()
{
    m_playing = false;
}

size_t LibretroAudioBackend::GetSamples(int16_t* buffer, size_t max_frames)
{
    if (!m_playing || !m_initialized)
        return 0;

    // Use timed lock to avoid skipping frames but also avoid deadlock
    // 2ms timeout is long enough to avoid most contention but short enough to not block retro_run
    std::unique_lock lock(m_mutex, std::defer_lock);
    if (!lock.try_lock_for(std::chrono::milliseconds(2)))
        return 0;

    const u32 sample_size = get_sample_size();  // 4 for float, 2 for s16
    const u32 channels = get_channels();
    const bool is_float = (m_sample_size == AudioSampleSize::FLOAT);
    const size_t bytes_per_frame = channels * sample_size;

    // Aggressively fill our ring buffer from the emulator's audio callback
    // Pull multiple times to ensure buffer stays full
    if (m_write_callback)
    {
        static constexpr size_t PULL_FRAMES = 2048;
        const size_t pull_bytes = PULL_FRAMES * bytes_per_frame;
        alignas(16) u8 temp_buffer[PULL_FRAMES * 2 * sizeof(float)];

        // Pull only what this call is about to hand out. The callback always
        // delivers - it pads with silence - so filling the whole ring put up to
        // half a second of audio between the game and the speakers.
        const size_t wanted_bytes = max_frames * bytes_per_frame;
        for (int pulls = 0; pulls < 4 && m_ring_size < wanted_bytes; pulls++)
        {
            const size_t free_space = m_ring_buffer_bytes.size() - m_ring_size;
            if (free_space < pull_bytes)
                break;

            const u32 bytes_written = m_write_callback(static_cast<u32>(pull_bytes), temp_buffer);
            if (bytes_written == 0)
                break;

            // Push to ring buffer - use memcpy for efficiency when possible
            if (m_ring_write_pos + bytes_written <= m_ring_buffer_bytes.size())
            {
                // Contiguous write
                std::memcpy(&m_ring_buffer_bytes[m_ring_write_pos], temp_buffer, bytes_written);
                m_ring_write_pos += bytes_written;
                if (m_ring_write_pos >= m_ring_buffer_bytes.size())
                    m_ring_write_pos = 0;
            }
            else
            {
                // Wrap-around write
                const size_t first_part = m_ring_buffer_bytes.size() - m_ring_write_pos;
                std::memcpy(&m_ring_buffer_bytes[m_ring_write_pos], temp_buffer, first_part);
                std::memcpy(&m_ring_buffer_bytes[0], temp_buffer + first_part, bytes_written - first_part);
                m_ring_write_pos = bytes_written - first_part;
            }
            m_ring_size += bytes_written;
        }
    }

    // Calculate how many frames we can provide
    const size_t frames_available = std::min(max_frames, m_ring_size / bytes_per_frame);

    if (frames_available == 0)
        return 0;

    const size_t bytes_to_read = frames_available * bytes_per_frame;

    // Read from ring buffer into a contiguous temp buffer for easier processing
    alignas(16) u8 read_buffer[2048 * 2 * sizeof(float)];
    if (m_ring_read_pos + bytes_to_read <= m_ring_buffer_bytes.size())
    {
        // Contiguous read
        std::memcpy(read_buffer, &m_ring_buffer_bytes[m_ring_read_pos], bytes_to_read);
        m_ring_read_pos += bytes_to_read;
        if (m_ring_read_pos >= m_ring_buffer_bytes.size())
            m_ring_read_pos = 0;
    }
    else
    {
        // Wrap-around read
        const size_t first_part = m_ring_buffer_bytes.size() - m_ring_read_pos;
        std::memcpy(read_buffer, &m_ring_buffer_bytes[m_ring_read_pos], first_part);
        std::memcpy(read_buffer + first_part, &m_ring_buffer_bytes[0], bytes_to_read - first_part);
        m_ring_read_pos = bytes_to_read - first_part;
    }
    m_ring_size -= bytes_to_read;

    // Convert to s16 for libretro
    if (is_float)
    {
        const float* src = reinterpret_cast<const float*>(read_buffer);
        const size_t total_samples = frames_available * channels;
        for (size_t i = 0; i < total_samples; i++)
        {
            float sample = std::clamp(src[i], -1.0f, 1.0f);
            buffer[i] = static_cast<int16_t>(sample * 32767.0f);
        }
    }
    else
    {
        // Already s16, just copy
        std::memcpy(buffer, read_buffer, bytes_to_read);
    }

    return frames_available;
}
