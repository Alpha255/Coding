#include "VulkanSwapChain.h"
#include "VulkanEngine.h"

void vkSwapchain::vkSurface::create(uint64_t appInstance, uint64_t windowHandle, const vkInstance &instance)
{
	assert(!isValid() && appInstance && windowHandle && instance.isValid());

#if defined(Platform_Win32)
	VkWin32SurfaceCreateInfoKHR createInfo
	{
		VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		nullptr, 
		0u,  /// reserved for future use
		(::HINSTANCE)appInstance,
		(::HWND)windowHandle
	};

	VkSurfaceKHR handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateWin32SurfaceKHR(*instance, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);
#else
	assert(0);
#endif
}

void vkSwapchain::vkSurface::destroy(const vkInstance &instance)
{
	assert(instance.isValid());

	if (isValid())
	{
		vkDestroySurfaceKHR(*instance, **this, vkMemoryAllocator);
		reset();
	}
}

void vkSwapchain::create(
	uint64_t appInstance, 
	uint64_t windowHandle, 
	uint32_t width, 
	uint32_t height, 
	bool8_t vSync, 
	bool8_t fullscreen, 
	const vkInstance &instance, 
	const vkPhysicalDevice &physicalDevice,
	vkDevice &device)
{
	assert(!isValid() && instance.isValid() && physicalDevice.isValid());

	m_Surface.create(appInstance, windowHandle, instance);

	uint32_t count = 0u;
	vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &count, nullptr);
	assert(count > 0u);
	
	std::vector<VkQueueFamilyProperties> queueFamilyProperties(count);
	vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &count, queueFamilyProperties.data());

	std::vector<VkBool32> surfaceSupportKHR(count);
	for (uint32_t i = 0u; i < count; ++i)
	{
		rVerifyVk(vkGetPhysicalDeviceSurfaceSupportKHR(*physicalDevice, i, *m_Surface, &surfaceSupportKHR[i]));
	}

	uint32_t graphicQueue = UINT32_MAX;
	uint32_t presentQueue = UINT32_MAX;
	for (uint32_t i = 0u; i < count; ++i)
	{
		if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT)
		{
			graphicQueue == UINT32_MAX ? graphicQueue = i : graphicQueue = graphicQueue;

			if (surfaceSupportKHR[i] == VK_TRUE)
			{
				graphicQueue = i;
				presentQueue = i;
				break;
			}
		}
	}
	if (presentQueue == UINT32_MAX)
	{
		for (uint32_t i = 0u; i < count; ++i)
		{
			if (surfaceSupportKHR[i] == VK_TRUE)
			{
				presentQueue = i;
				break;
			}
		}
	}

	assert(graphicQueue != UINT32_MAX && presentQueue != UINT32_MAX && graphicQueue == presentQueue);

	rVerifyVk(vkGetPhysicalDeviceSurfaceFormatsKHR(*physicalDevice, *m_Surface, &count, nullptr));
	assert(count > 0u);
	std::vector<VkSurfaceFormatKHR> surfaceFormats(count);
	rVerifyVk(vkGetPhysicalDeviceSurfaceFormatsKHR(*physicalDevice, *m_Surface, &count, surfaceFormats.data()));

	m_Surface.SurfaceFormat = surfaceFormats[0];
	for (auto const &surfaceFormat : surfaceFormats)
	{
		if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
		{
			m_Surface.SurfaceFormat = surfaceFormat;
			break;
		}
	}
	assert(m_Surface.SurfaceFormat.colorSpace != VK_COLOR_SPACE_MAX_ENUM_KHR);

	rVerifyVk(vkGetPhysicalDeviceSurfacePresentModesKHR(*physicalDevice, *m_Surface, &count, nullptr));
	assert(count > 0u);
	m_Surface.PresentModes.resize(count);
	rVerifyVk(vkGetPhysicalDeviceSurfacePresentModesKHR(*physicalDevice, *m_Surface, &count, m_Surface.PresentModes.data()));

	m_PresentCompleteSemaphore = new vkSemaphore(device);

	recreate(width, height, vSync, fullscreen, physicalDevice, device);
}

void vkSwapchain::recreate(
	uint32_t width, 
	uint32_t height, 
	bool8_t vSync, 
	bool8_t fullscreen, 
	const vkPhysicalDevice &physicalDevice,
	vkDevice &device)
{
	assert(device.isValid() && m_Surface.isValid());

	m_Width = width;
	m_Height = height;
	m_bVSync = vSync;
	m_bFullScreen = fullscreen;

	rVerifyVk(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*physicalDevice, *m_Surface, &m_Surface.SurfaceCapabilities));

	VkExtent2D sizeExtent{};
	sizeExtent.width = std::min<uint32_t>(width, m_Surface.SurfaceCapabilities.maxImageExtent.width);
	sizeExtent.height = std::min<uint32_t>(height, m_Surface.SurfaceCapabilities.maxImageExtent.height);
	sizeExtent.width = std::max<uint32_t>(sizeExtent.width, m_Surface.SurfaceCapabilities.minImageExtent.width);
	sizeExtent.height = std::max<uint32_t>(sizeExtent.height, m_Surface.SurfaceCapabilities.minImageExtent.height);

	/// VK_PRESENT_MODE_IMMEDIATE_KHR: Images submitted by your application are transferred to the screen right away, which may result in tearing

	/// VK_PRESENT_MODE_FIFO_KHR: The swap chain is a queue where the display takes an image from the front of the queue when the display is 
	/// refreshed and the program inserts rendered images at the back of the queue.If the queue is full then the program has to wait.
	/// This is most similar to vertical sync as found in modern games.The moment that the display is refreshed is known as ¡°vertical blank¡±
	
	/// VK_PRESENT_MODE_FIFO_RELAXED_KHR: This mode only differs from the previous one if the application is late and the queue was empty at the last
	/// vertical blank.Instead of waiting for the next vertical blank, the image is transferred right away when it finally arrives.This may result in visible tearing
	
	/// VK_PRESENT_MODE_MAILBOX_KHR: This is another variation of the second mode. Instead of blocking the application when the queue is full, the
	/// images that are already queued are simply replaced with the newer ones. This mode can be used to implement triple buffering, which allows you 
	/// to avoid tearing with significantly less latency issues than standard vertical sync that uses double buffering

	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
	if (!m_bVSync)
	{
		for (uint32_t i = 0u; i < m_Surface.PresentModes.size(); ++i)
		{
			if (m_Surface.PresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
		}
	}

	uint32_t usageFlagBits = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (m_Surface.SurfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
	{
		usageFlagBits |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}
	if (m_Surface.SurfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
	{
		/// use a value like VK_IMAGE_USAGE_TRANSFER_DST_BIT instead and use a memory operation to transfer the rendered image to a swap chain image
		usageFlagBits |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

	VkCompositeAlphaFlagBitsKHR compositeAlphaFlagBits = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
	if (m_Surface.SurfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
	{
		compositeAlphaFlagBits = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	}

	VkSurfaceTransformFlagBitsKHR surfaceTransformFlagBits = VK_SURFACE_TRANSFORM_FLAG_BITS_MAX_ENUM_KHR;
	if (m_Surface.SurfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		surfaceTransformFlagBits = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else
	{
		surfaceTransformFlagBits = m_Surface.SurfaceCapabilities.currentTransform;
	}

	/// Specify how to handle swap chain images that will be used across multiple queue families, 
	/// That will be the case in application if the graphics queue family is different from the presentation queue
	
	/// VK_SHARING_MODE_EXCLUSIVE: An image is owned by one queue family at a time and ownership must be explicitly transfered before using 
	/// it in another queue family.This option offers the best performance
	
	/// VK_SHARING_MODE_CONCURRENT: Images can be used across multiple queue families without explicit ownership transfers,
	/// concurrent mode requires you to specify at least two distinct queue families
	VkSwapchainKHR oldSwapchain = **this;
	VkSwapchainCreateInfoKHR createInfo
	{
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		nullptr,
		0u,
		*m_Surface,
		presentMode == VK_PRESENT_MODE_MAILBOX_KHR ? m_Surface.SurfaceCapabilities.minImageCount + 1u : m_Surface.SurfaceCapabilities.minImageCount,
		m_Surface.SurfaceFormat.format,
		m_Surface.SurfaceFormat.colorSpace,
		sizeExtent,
		1u, /// This is always 1 unless you are developing a stereoscopic 3D application
		usageFlagBits,
		VK_SHARING_MODE_EXCLUSIVE,
		0u,
		nullptr,
		surfaceTransformFlagBits,
		compositeAlphaFlagBits,  /// The compositeAlpha field specifies if the alpha channel should be used for blending with other windows in the window system
		presentMode,
		VK_TRUE,
		oldSwapchain
	};

	/// If the clipped member is set to VK_TRUE then that means that we don¡¯t care about the color of pixels that are
	/// obscured, for example because another window is in front of them.Unless you really need to be able to read these 
	/// pixels back and get predictable results, you will get the best performance by enabling clipping

	device.waitIdle();

	VkSwapchainKHR handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateSwapchainKHR(*device, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);

	if (oldSwapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(*device, oldSwapchain, vkMemoryAllocator);
		clearBackBuffers(device);
	}

	uint32_t imageCount = 0u;
	rVerifyVk(vkGetSwapchainImagesKHR(*device, **this, &imageCount, nullptr));
	std::vector<VkImage> images(imageCount);
	m_BackBuffers.resize(imageCount);
	rVerifyVk(vkGetSwapchainImagesKHR(*device, **this, &imageCount, images.data()));
	for (uint32_t i = 0u; i < images.size(); ++i)
	{
		vkImage image;
		image.reset(images[i]);

		m_BackBuffers[i].create(device, image, m_Surface.SurfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

void vkSwapchain::destroy(const vkInstance &instance, const vkDevice &device)
{
	assert(instance.isValid() && device.isValid());

	vkDestroySwapchainKHR(*device, **this, vkMemoryAllocator);
	m_Surface.destroy(instance);
	reset();
	m_PresentCompleteSemaphore->destroy(device);

	clearBackBuffers(device);
}

uint32_t vkSwapchain::acquireBackBuffer(const vkDevice &device)
{
	assert(isValid() && device.isValid());
	m_CurrentFrameIndex = UINT32_MAX;

	rVerifyVk(vkAcquireNextImageKHR(*device, **this, UINT64_MAX, **m_PresentCompleteSemaphore, VK_NULL_HANDLE, &m_CurrentFrameIndex));
	return m_CurrentFrameIndex;
}

void vkSwapchain::present(const vkDeviceQueue &queue, const vkSemaphore &renderCompleteSephore) const
{
	VkSwapchainKHR handle = **this;
	VkSemaphore semaphore = *renderCompleteSephore;
	VkPresentInfoKHR presentInfo
	{
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		nullptr,
		1u,
		&semaphore,
		1u,
		&handle,
		&m_CurrentFrameIndex,
		nullptr
	};
	rVerifyVk(vkQueuePresentKHR(*queue, &presentInfo));
}

void vkSwapchain::clearBackBuffers(const vkDevice &device)
{
	for (uint32_t i = 0u; i < m_BackBuffers.size(); ++i)
	{
		m_BackBuffers[i].destroy(device);
	}
	m_BackBuffers.clear();
}
