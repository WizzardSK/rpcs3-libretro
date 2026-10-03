#include "stdafx.h"
#include "VKLibretro.h"
#include "vkutils/instance.h"
#include "vkutils/swapchain.h"

#include <algorithm>
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
		std::vector<u32> s_ready_frames;
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

	void frame_ready(u32 image_index)
	{
		std::vector<u32> overtaken;
		{
			std::lock_guard lock(s_frames_mutex);
			s_ready_frames.push_back(image_index);
			while (s_ready_frames.size() > kMaxQueued)
			{
				overtaken.push_back(s_ready_frames.front());
				s_ready_frames.erase(s_ready_frames.begin());
			}
		}

		if (auto* swapchain = dynamic_cast<vk::swapchain_LIBRETRO*>(s_swapchain))
			for (u32 index : overtaken)
				swapchain->release_image(index);
	}

	bool take_frame(u32& image_index)
	{
		std::lock_guard lock(s_frames_mutex);
		if (s_ready_frames.empty())
			return false;
		image_index = s_ready_frames.front();
		s_ready_frames.erase(s_ready_frames.begin());
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
