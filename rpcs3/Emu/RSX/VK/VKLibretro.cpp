#include "stdafx.h"
#include "VKLibretro.h"
#include "vkutils/instance.h"
#include "vkutils/swapchain.h"

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <mutex>
#include <string>

namespace vk::libretro
{
	namespace
	{
		bool s_hw_present = false;

		std::vector<std::string> s_extensions;
		VkPhysicalDeviceFeatures s_features{};

		VkDevice s_frontend_device = VK_NULL_HANDLE;

		void (*s_lock)(void*) = nullptr;
		void (*s_unlock)(void*) = nullptr;
		void* s_lock_handle = nullptr;

		vk::swapchain_base* s_swapchain = nullptr;

		std::mutex s_frames_mutex;
		std::condition_variable s_frames_cv;
		std::vector<u32> s_ready_frames;

		// When retro_run last began or returned, whether one is running now,
		// and whether it is expected to come again
		bool s_frontend_attached = false;
		bool s_frontend_in_run = false;
		std::chrono::steady_clock::time_point s_frontend_ran{};

		// Stepping: the last retro_run came after the frontend had stopped.
		// Each such run lets the game draw one frame (the credit) and shows
		// the frame pushed since (s_pushes past s_step_base).
		bool s_stepping = false;
		u32 s_step_credits = 0;
		u64 s_pushes = 0;
		u64 s_step_base = 0;
		atomic_t<u32> s_generation{0};
	}

	bool hw_present() { return s_hw_present; }
	void set_hw_present(bool on) { s_hw_present = on; }

	void set_frontend_requirements(const char* const* extensions, u32 count, const VkPhysicalDeviceFeatures* features)
	{
		s_extensions.clear();
		for (u32 i = 0; i < count; ++i)
			if (extensions[i])
				s_extensions.emplace_back(extensions[i]);
		s_features = features ? *features : VkPhysicalDeviceFeatures{};
	}

	void apply_frontend_requirements(std::vector<const char*>& extensions, VkPhysicalDeviceFeatures& features)
	{
		if (!s_hw_present)
			return;

		for (const auto& ext : s_extensions)
		{
			const bool have = std::any_of(extensions.cbegin(), extensions.cend(), [&](const char* e) { return ext == e; });
			if (!have)
				extensions.push_back(ext.c_str());
		}

		// VkPhysicalDeviceFeatures is nothing but VkBool32s, so OR them.
		static_assert(sizeof(VkPhysicalDeviceFeatures) % sizeof(VkBool32) == 0);
		auto* dst = reinterpret_cast<VkBool32*>(&features);
		const auto* src = reinterpret_cast<const VkBool32*>(&s_features);
		for (usz i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); ++i)
			dst[i] = dst[i] || src[i];
	}

	void set_frontend_device(VkDevice dev) { s_frontend_device = dev; }
	bool is_frontend_device(VkDevice dev) { return dev && dev == s_frontend_device; }

	void set_queue_lock(void (*lock)(void*), void (*unlock)(void*), void* handle)
	{
		// Both or neither: an older frontend leaves them null.
		const bool usable = lock && unlock;
		s_lock = usable ? lock : nullptr;
		s_unlock = usable ? unlock : nullptr;
		s_lock_handle = handle;
	}

	void lock_queue()
	{
		if (s_lock)
			s_lock(s_lock_handle);
	}

	void unlock_queue()
	{
		if (s_unlock)
			s_unlock(s_lock_handle);
	}

	vk::instance& shared_instance()
	{
		static vk::instance s_instance;
		return s_instance;
	}

	void set_shared_swapchain(vk::swapchain_base* swapchain) { s_swapchain = swapchain; }
	vk::swapchain_base* shared_swapchain() { return s_swapchain; }

	// Finished frames wait in order and each is handed over once (take_frame
	// takes the oldest). Keeping only the newest, as before, threw a frame away
	// whenever two finished between frontend frames and repeated one whenever
	// none did - the emulator's frame times vary by a few percent, the
	// frontend's do not, and that difference showed as judder (NNshi: 117-123
	// fps through a 120 Hz frontend). A short queue absorbs it. Beyond
	// kMaxQueued the oldest go back unseen, which bounds the latency it adds -
	// and keeps images free while retro_run is not running at all (boot,
	// shutdown), when nothing takes frames: left waiting there, they held every
	// image and each flip waited five seconds for one (Wipeout HD).
	constexpr usz kMaxQueued = 2;

	// How long after a retro_run returned the frontend still counts as
	// running: a frame and a half at 60 a second. Past it, it has stopped
	// taking frames (paused, in its menu), and a retro_run that comes after
	// such a gap is a step: Frame Advance, or the first run after a pause.
	// Counted from the return, not from the start: a step waits in retro_run
	// for its frame, and counted from the start that wait alone made the next
	// run a step too - after a pause the game stayed at a frame per step, 5-10
	// fps, for seconds (NNshi).
	constexpr auto kFrontendRunning = std::chrono::milliseconds(25);

	// How long a step waits for the frame the game draws in it. Below the
	// 100 ms after which the core's watchdog pauses the emulator again.
	constexpr auto kStepWait = std::chrono::milliseconds(90);

	// Under s_frames_mutex. Whether a finished flip has to wait before the
	// game goes on to its next frame.
	static bool frontend_stopped()
	{
		if (!s_frontend_attached)
			return false;
		if (s_stepping)
			return s_step_credits == 0;
		if (s_frontend_in_run)
			return false;
		return std::chrono::steady_clock::now() - s_frontend_ran > kFrontendRunning;
	}

	void frontend_ran()
	{
		{
			std::lock_guard lock(s_frames_mutex);
			const auto now = std::chrono::steady_clock::now();
			s_stepping = s_frontend_attached && now - s_frontend_ran > kFrontendRunning;
			// one frame of the game for this retro_run, and the frame it
			// draws is the one to show (take_frame)
			s_step_credits = s_stepping ? 1 : 0;
			s_step_base = s_pushes;
			s_frontend_attached = true;
			s_frontend_in_run = true;
			s_frontend_ran = now;
		}
		s_frames_cv.notify_all();
	}

	void frontend_returned()
	{
		std::lock_guard lock(s_frames_mutex);
		s_frontend_in_run = false;
		s_frontend_ran = std::chrono::steady_clock::now();
	}

	void frontend_detached()
	{
		{
			std::lock_guard lock(s_frames_mutex);
			s_frontend_attached = false;
			s_frontend_in_run = false;
			s_stepping = false;
		}
		s_frames_cv.notify_all();
	}

	void frame_ready(u32 image_index)
	{
		std::vector<u32> overtaken;
		{
			std::lock_guard lock(s_frames_mutex);
			s_ready_frames.push_back(image_index);
			s_pushes++;
			// the frame a step let the game draw
			if (s_stepping && s_step_credits > 0)
				s_step_credits--;
			while (s_ready_frames.size() > kMaxQueued)
			{
				overtaken.push_back(s_ready_frames.front());
				s_ready_frames.erase(s_ready_frames.begin());
			}
		}
		s_frames_cv.notify_all();

		if (auto* swapchain = dynamic_cast<vk::swapchain_LIBRETRO*>(s_swapchain))
			for (u32 index : overtaken)
				swapchain->release_image(index);

		// The frontend has stopped calling retro_run (paused, in its menu,
		// stepping frames): the game waits here, its frame handed over, for
		// the next one. It used to run on until a watchdog paused it 100 ms
		// later; the frames in between were dropped and Frame Advance let
		// that many through a step (NNshi: 4-6 frames a step, a skip after
		// every pause). A step lets exactly one frame through. In slices, so
		// a stop of the content is noticed.
		std::unique_lock lock(s_frames_mutex);
		while (frontend_stopped())
			s_frames_cv.wait_for(lock, std::chrono::milliseconds(5));
	}

	bool take_frame(u32& image_index)
	{
		std::vector<u32> older;
		{
			std::unique_lock lock(s_frames_mutex);

			// A step shows the frame the game draws in it - with the input
			// retro_run has just polled - not one finished before it: wait
			// for that frame, and take the newest
			if (s_stepping)
			{
				s_frames_cv.wait_for(lock, kStepWait, [] { return s_pushes != s_step_base; });
				while (s_ready_frames.size() > 1)
				{
					older.push_back(s_ready_frames.front());
					s_ready_frames.erase(s_ready_frames.begin());
				}
			}

			if (s_ready_frames.empty())
				return false;
			image_index = s_ready_frames.front();
			s_ready_frames.erase(s_ready_frames.begin());
		}

		if (auto* swapchain = dynamic_cast<vk::swapchain_LIBRETRO*>(s_swapchain))
			for (u32 index : older)
				swapchain->release_image(index);
		return true;
	}

	void drop_pending_frames()
	{
		std::lock_guard lock(s_frames_mutex);
		s_ready_frames.clear();
	}

	u32 image_generation() { return s_generation; }

	void new_image_generation()
	{
		drop_pending_frames();
		s_generation++;
	}
}
