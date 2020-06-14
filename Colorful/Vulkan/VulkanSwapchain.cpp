#include "Colorful/Vulkan/VulkanEngine.h"

void VulkanSwapchain::VulkanSurface::create(uint64_t windowHandle, VkInstance instance)
{
	assert(!isValid() && windowHandle);

#if defined(Platform_Win32)
	::HMODULE hInstance = ::GetModuleHandleA(nullptr);
	verify_Log(hInstance);

	VkWin32SurfaceCreateInfoKHR createInfo
	{
		VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		nullptr, 
		0u,  /// reserved for future use
		(::HINSTANCE)hInstance,
		(::HWND)windowHandle
	};

	GfxVerifyVk(vkCreateWin32SurfaceKHR(instance, &createInfo, vkMemoryAllocator, &Handle));
#else
	assert(0);
#endif
}

void VulkanSwapchain::VulkanSurface::destroy(VkInstance instance)
{
	if (isValid())
	{
		vkDestroySurfaceKHR(instance, Handle, vkMemoryAllocator);
		Handle = VK_NULL_HANDLE;
	}
}

VulkanSwapchain::VulkanSwapchain(
	uint64_t windowHandle,
	uint32_t width,
	uint32_t height,
	bool8_t VSync,
	bool8_t fullscreen,
	VkInstance instance,
	VkPhysicalDevice physicalDevice,
	VkDevice device)
	: m_LogicDevice(device)
	, m_PhysicalDevice(physicalDevice)
	, m_FullScreen(fullscreen)
	, m_VSync(VSync)
	, m_Width(width)
	, m_Height(height)
{
	assert(!isValid());

	m_Surface.create(windowHandle, instance);

	uint32_t count = 0u;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
	assert(count > 0u);
	
	std::vector<VkQueueFamilyProperties> queueFamilyProperties(count);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queueFamilyProperties.data());

	std::vector<VkBool32> surfaceSupportKHR(count);
	for (uint32_t i = 0u; i < count; ++i)
	{
		GfxVerifyVk(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_Surface.Handle, &surfaceSupportKHR[i]));
	}

	uint32_t graphicQueue = std::numeric_limits<uint32_t>().max();
	uint32_t presentQueue = std::numeric_limits<uint32_t>().max();
	for (uint32_t i = 0u; i < count; ++i)
	{
		if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT)
		{
			graphicQueue == std::numeric_limits<uint32_t>().max() ? graphicQueue = i : graphicQueue = graphicQueue;

			if (surfaceSupportKHR[i] == VK_TRUE)
			{
				graphicQueue = i;
				presentQueue = i;
				break;
			}
		}
	}
	if (presentQueue == std::numeric_limits<uint32_t>().max())
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

	assert(graphicQueue != std::numeric_limits<uint32_t>().max() && presentQueue != std::numeric_limits<uint32_t>().max() && graphicQueue == presentQueue);

	GfxVerifyVk(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface.Handle, &count, nullptr));
	assert(count > 0u);
	std::vector<VkSurfaceFormatKHR> surfaceFormats(count);
	GfxVerifyVk(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface.Handle, &count, surfaceFormats.data()));

	m_Surface.SurfaceFormat = surfaceFormats[0];
	for (auto const& surfaceFormat : surfaceFormats)
	{
		if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
		{
			m_Surface.SurfaceFormat = surfaceFormat;
			break;
		}
	}
	assert(m_Surface.SurfaceFormat.colorSpace != VK_COLOR_SPACE_MAX_ENUM_KHR);

	GfxVerifyVk(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface.Handle, &count, nullptr));
	assert(count > 0u);
	m_Surface.PresentModes.resize(count);
	GfxVerifyVk(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface.Handle, &count, m_Surface.PresentModes.data()));

	m_PresentCompleteSemaphore = VulkanAsyncPool::instance()->allocSemaphore();

	recreate();
}

void VulkanSwapchain::recreate()
{
	assert(m_Surface.isValid());

	GfxVerifyVk(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface.Handle, &m_Surface.SurfaceCapabilities));

	VkExtent2D sizeExtent{};
	sizeExtent.width = std::min<uint32_t>(m_Width, m_Surface.SurfaceCapabilities.maxImageExtent.width);
	sizeExtent.height = std::min<uint32_t>(m_Height, m_Surface.SurfaceCapabilities.maxImageExtent.height);
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
	if (!m_VSync)
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
	VkSwapchainKHR oldSwapchain = Handle;
	VkSwapchainCreateInfoKHR createInfo
	{
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		nullptr,
		0u,
		m_Surface.Handle,
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

	GfxVerifyVk(vkCreateSwapchainKHR(m_LogicDevice, &createInfo, vkMemoryAllocator, &Handle));

	if (oldSwapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(m_LogicDevice, oldSwapchain, vkMemoryAllocator);
		destroyBackBuffers();
	}

	uint32_t imageCount = 0u;
	GfxVerifyVk(vkGetSwapchainImagesKHR(m_LogicDevice, Handle, &imageCount, nullptr));
	std::vector<VkImage> images(imageCount);
	m_BackBufferImages.resize(imageCount);
	GfxVerifyVk(vkGetSwapchainImagesKHR(m_LogicDevice, Handle, &imageCount, images.data()));
	for (uint32_t i = 0u; i < images.size(); ++i)
	{
		auto image = std::make_shared<VulkanImage>(images[i], m_Surface.SurfaceFormat.format);
		m_BackBufferImages[i] = std::make_shared<VulkanImageView>(m_LogicDevice, image, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT);
	}
	m_DepthStencil = std::make_shared<VulkanImageView>(
		m_LogicDevice, 
		sizeExtent.width, 
		sizeExtent.height, 
		1u, 1u, 1u, 
		VK_IMAGE_VIEW_TYPE_2D, 
		VK_FORMAT_D24_UNORM_S8_UINT, 
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
		VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
}

void VulkanSwapchain::destroy(VkInstance instance)
{
	vkDestroySwapchainKHR(m_LogicDevice, Handle, vkMemoryAllocator);
	m_Surface.destroy(instance);
	Handle = VK_NULL_HANDLE;

	destroyBackBuffers();
}

uint32_t VulkanSwapchain::acquireNextFrame()
{
	assert(isValid());
	m_CurrentFrameIndex = std::numeric_limits<uint32_t>().max();

	GfxVerifyVk(vkAcquireNextImageKHR(m_LogicDevice, Handle, UINT64_MAX, m_PresentCompleteSemaphore->Handle, VK_NULL_HANDLE, &m_CurrentFrameIndex));
	return m_CurrentFrameIndex;
}

void VulkanSwapchain::present(VkSemaphore renderCompleteSephore) const
{
	VkPresentInfoKHR presentInfo
	{
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		nullptr,
		1u,
		&renderCompleteSephore,
		1u,
		&Handle,
		&m_CurrentFrameIndex,
		nullptr
	};
	GfxVerifyVk(vkQueuePresentKHR(VulkanQueueManager::instance()->gfxQueue()->Handle, &presentInfo));
}

void VulkanSwapchain::destroyBackBuffers()
{
	for (uint32_t i = 0u; i < m_BackBufferImages.size(); ++i)
	{
		m_BackBufferImages[i]->destroy(m_LogicDevice);
	}
	m_BackBufferImages.clear();

	m_DepthStencil->destroy(m_LogicDevice);
}
