#pragma once

#include "swapchain_core.h"

namespace vk
{
#if defined(ANDROID)
	using swapchain_ANDROID = native_swapchain_base;
	using swapchain_NATIVE = swapchain_ANDROID;

	[[maybe_unused]] static
	VkSurfaceKHR make_WSI_surface(VkInstance vk_instance, display_handle_t window_handle, WSI_config* /*config*/)
	{
		VkSurfaceKHR result = VK_NULL_HANDLE;

		// Was a copy of the Win32 version: the Win32 create-info struct with an
		// Android sType, and this->m_instance inside a free function. None of it
		// had ever been compiled - nothing builds this header - so it only
		// surfaced once an Android build got this far.
		VkAndroidSurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
		createInfo.window = std::get<ANativeWindow *>(window_handle);

		CHECK_RESULT(vkCreateAndroidSurfaceKHR(vk_instance, &createInfo, nullptr, &result));
		return result;
	}
#endif
}
