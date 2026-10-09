#pragma once

#include "Emu/Audio/AudioBackend.h"
#include "libretro.h"

#include <vector>
#include <mutex>
#include <atomic>

// Multi-channel audio (RetroArch since 10 September 2026), from the current
// libretro.h, which the core's copy predates
#ifndef RETRO_ENVIRONMENT_GET_AUDIO_SAMPLE_BATCH_MULTI
#define RETRO_ENVIRONMENT_GET_AUDIO_SAMPLE_BATCH_MULTI (94 | RETRO_ENVIRONMENT_EXPERIMENTAL)
#define RETRO_AUDIO_SPEAKER_FRONT_LEFT    0x001
#define RETRO_AUDIO_SPEAKER_FRONT_RIGHT   0x002
#define RETRO_AUDIO_SPEAKER_FRONT_CENTER  0x004
#define RETRO_AUDIO_SPEAKER_LOW_FREQUENCY 0x008
#define RETRO_AUDIO_SPEAKER_BACK_LEFT     0x010
#define RETRO_AUDIO_SPEAKER_BACK_RIGHT    0x020
#define RETRO_AUDIO_SPEAKER_SIDE_LEFT     0x200
#define RETRO_AUDIO_SPEAKER_SIDE_RIGHT    0x400
typedef size_t (RETRO_CALLCONV *retro_audio_sample_batch_multi_int16_t)(
      const int16_t *data, size_t frames, unsigned channels, unsigned layout);
typedef size_t (RETRO_CALLCONV *retro_audio_sample_batch_multi_float_t)(
      const float *data, size_t frames, unsigned channels, unsigned layout);
struct retro_audio_sample_multi_callback
{
   retro_audio_sample_batch_multi_int16_t batch_int16;
   retro_audio_sample_batch_multi_float_t batch_float;
};
#endif

// Asks the frontend for multi-channel output; true when it takes it, and
// from then on all audio goes through it (the frontend's rule: one or the
// other for the loaded game). Call from retro_load_game.
bool libretro_audio_negotiate_multi(retro_environment_t environ_cb);
bool libretro_audio_multi_available();

// Process and send audio to libretro frontend
void libretro_audio_process(retro_audio_sample_batch_t audio_batch_cb);

// LibretroAudioBackend - AudioBackend implementation for libretro
// Uses a ring buffer to decouple emulator audio production from libretro consumption
class LibretroAudioBackend : public AudioBackend
{
private:
    std::timed_mutex m_mutex;  // timed_mutex for try_lock_for support
    std::atomic<bool> m_playing{false};
    std::atomic<bool> m_initialized{false};

    // Ring buffer for audio data (stored as bytes to handle float or s16)
    std::vector<u8> m_ring_buffer_bytes;
    size_t m_ring_read_pos = 0;
    size_t m_ring_write_pos = 0;
    size_t m_ring_size = 0;  // Current number of bytes in buffer

    // Note: We use the base class m_sampling_rate, m_sample_size, m_channels, m_layout
    // DO NOT shadow them here or audio format detection will break!

public:
    LibretroAudioBackend();
    ~LibretroAudioBackend() override;

    std::string_view GetName() const override { return "Libretro"; }

    bool Open(std::string_view dev_id, AudioFreq freq, AudioSampleSize sample_size, AudioChannelCnt ch_cnt, audio_channel_layout layout) override;
    void Close() override;

    void SetWriteCallback(std::function<u32(u32, void*)> cb) override;
    void SetStateCallback(std::function<void(AudioStateEvent)> cb) override;

    f64 GetCallbackFrameLen() override;

    bool IsPlaying() override { return m_playing; }

    void Play() override;
    void Pause() override;

    bool Initialized() override { return m_initialized; }

    // Called by libretro_audio_process to get audio data (non-blocking)
    // Handles float->s16 conversion internally if needed
    size_t GetSamples(int16_t* buffer, size_t max_frames);
};

// Global audio backend instance access
LibretroAudioBackend* get_libretro_audio_backend();
