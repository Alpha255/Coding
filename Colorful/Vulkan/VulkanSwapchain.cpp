#include "Colorful/Vulkan/VulkanSwapChain.h"

NAMESPACE_START(Gfx)

VulkanSurface::VulkanSurface(uint64_t appInstance, uint64_t windowHandle, VkInstance instance)
	: m_Instance(instance)
{
	assert(appInstance && windowHandle && instance);

#if defined(PLATFORM_WIN32)
	VkWin32SurfaceCreateInfoKHR createInfo
	{
		VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		nullptr,
		0u,  /// reserved for future use
		reinterpret_cast<::HINSTANCE>(appInstance),
		reinterpret_cast<::HWND>(windowHandle)
	};

	VERIFY_VK(vkCreateWin32SurfaceKHR(instance, &createInfo, VK_MEMORY_ALLOCATOR, reference()));
#else
	assert(0);
#endif
}

VulkanSwapchain::VulkanSwapchain(
	uint64_t appInstance,
	uint64_t windowHandle,
	uint32_t width,
	uint32_t height,
	bool8_t fullscreen,
	bool8_t VSync,
	bool8_t sRGB,
	const VkInstance instance,
	const VkPhysicalDevice physicalDevice,
	const VkDevice device)
	: m_AppInstance(appInstance)
	, m_WindowHandle(windowHandle)
	, m_FullScreen(fullscreen)
	, m_VSync(VSync)
	, m_Width(width)
	, m_Height(height)
	, m_ColorFormat(sRGB ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_B8G8R8A8_UNORM)
	, m_Instance(instance)
	, m_PhysicalDevice(physicalDevice)
	, m_Device(device)
{
	assert(windowHandle && physicalDevice && device);

	m_Surface = std::make_unique<VulkanSurface>(appInstance, windowHandle, instance);

	create();
}

void VulkanSwapchain::create()
{
	uint32_t count = 0u;
	VERIFY_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface->get(), &count, nullptr));
	assert(count > 0u);
	std::vector<VkSurfaceFormatKHR> surfaceFormats(count);
	VERIFY_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface->get(), &count, surfaceFormats.data()));

	VkColorSpaceKHR ColorSpace = VK_COLOR_SPACE_MAX_ENUM_KHR;
	for (auto const& surfaceFormat : surfaceFormats)
	{
		if (surfaceFormat.format == m_ColorFormat)
		{
			ColorSpace = surfaceFormat.colorSpace;
			break;
		}
	}
	if (ColorSpace == VK_COLOR_SPACE_MAX_ENUM_KHR)
	{
		VkFormat ColorFormat = VK_FORMAT_UNDEFINED;
		switch (m_ColorFormat)
		{
		case VK_FORMAT_R8G8B8A8_UNORM: ColorFormat = VK_FORMAT_B8G8R8A8_UNORM; break;
		case VK_FORMAT_B8G8R8A8_UNORM: ColorFormat = VK_FORMAT_R8G8B8A8_UNORM; break;
		case VK_FORMAT_B8G8R8A8_SRGB:  ColorFormat = VK_FORMAT_R8G8B8A8_SRGB;  break;
		case VK_FORMAT_R8G8B8A8_SRGB:  ColorFormat = VK_FORMAT_B8G8R8A8_SRGB;  break;
		}

		if (ColorFormat != VK_FORMAT_UNDEFINED)
		{
			for (auto const& surfaceFormat : surfaceFormats)
			{
				if (surfaceFormat.format == ColorFormat)
				{
					ColorSpace = surfaceFormat.colorSpace;
					m_ColorFormat = ColorFormat;
					break;
				}
			}
		}

		assert(ColorSpace != VK_COLOR_SPACE_MAX_ENUM_KHR);
	}

	/// VK_PRESENT_MODE_IMMEDIATE_KHR: Images submitted by your application are transferred to the screen right away, which may result in tearing

	/// VK_PRESENT_MODE_FIFO_KHR: The swap chain is a queue where the display takes an image from the front of the queue when the display is 
	/// refreshed and the program inserts rendered images at the back of the queue.If the queue is full then the program has to wait.
	/// This is most similar to vertical sync as found in modern games.The moment that the display is refreshed is known as ��vertical blank��

	/// VK_PRESENT_MODE_FIFO_RELAXED_KHR: This mode only differs from the previous one if the application is late and the queue was empty at the last
	/// vertical blank.Instead of waiting for the next vertical blank, the image is transferred right away when it finally arrives.This may result in visible tearing

	/// VK_PRESENT_MODE_MAILBOX_KHR: This is another variation of the second mode. Instead of blocking the application when the queue is full, the
	/// images that are already queued are simply replaced with the newer ones. This mode can be used to implement triple buffering, which allows you 
	/// to avoid tearing with significantly less latency issues than standard vertical sync that uses double buffering
	VERIFY_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface->get(), &count, nullptr));
	assert(count > 0u);
	std::vector<VkPresentModeKHR> presentModes(count);
	VERIFY_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface->get(), &count, presentModes.data()));

	VkPresentModeKHR presentMode = m_VSync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR;
	VERIFY(std::find(presentModes.cbegin(), presentModes.cend(), presentMode) != presentModes.end());

	vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, nullptr);
	assert(count > 0u);
	std::vector<VkQueueFamilyProperties> queueFamilyProperties(count);
	vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, queueFamilyProperties.data());
	std::vector<VkBool32> surfaceSupportKHRs(count);
	for (uint32_t i = 0u; i < count; ++i)
	{
		VERIFY_VK(vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, m_Surface->get(), &surfaceSupportKHRs[i]));
	}

	uint32_t graphicQueue = std::numeric_limits<uint32_t>().max();
	uint32_t presentQueue = std::numeric_limits<uint32_t>().max();
	for (uint32_t i = 0u; i < count; ++i)
	{
		if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
		{
			graphicQueue = graphicQueue == std::numeric_limits<uint32_t>().max() ? i : graphicQueue;

			if (surfaceSupportKHRs[i] == VK_TRUE)
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
			if (surfaceSupportKHRs[i] == VK_TRUE)
			{
				presentQueue = i;
				break;
			}
		}
	}

	assert(graphicQueue != std::numeric_limits<uint32_t>().max() && presentQueue != std::numeric_limits<uint32_t>().max() && graphicQueue == presentQueue);

	VkSurfaceCapabilitiesKHR surfaceCapabilities{};
	VERIFY_VK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface->get(), &surfaceCapabilities));

	VkExtent2D sizeExtent
	{
		std::max<uint32_t>(std::min<uint32_t>(m_Width, surfaceCapabilities.maxImageExtent.width), surfaceCapabilities.minImageExtent.width),
		std::max<uint32_t>(std::min<uint32_t>(m_Height, surfaceCapabilities.maxImageExtent.height), surfaceCapabilities.minImageExtent.height),
	};

	m_Width = sizeExtent.width;
	m_Height = sizeExtent.height;

	uint32_t usageFlagBits = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
	{
		usageFlagBits |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}
	if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
	{
		/// use a value like VK_IMAGE_USAGE_TRANSFER_DST_BIT instead and use a memory operation to transfer the rendered image to a swap chain image
		usageFlagBits |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

	VkCompositeAlphaFlagBitsKHR compositeAlphaFlagBits = VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR;
	VkCompositeAlphaFlagBitsKHR compositeAlphaFlagBitsArray[] 
	{
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR
	};
	for (uint32_t i = 0u; i < _countof(compositeAlphaFlagBitsArray); ++i)
	{
		if (surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlagBitsArray[i])
		{
			compositeAlphaFlagBits = compositeAlphaFlagBitsArray[i];
			break;
		}
	}
	assert(compositeAlphaFlagBits != VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR);

	VkSurfaceTransformFlagBitsKHR surfaceTransformFlagBits = VK_SURFACE_TRANSFORM_FLAG_BITS_MAX_ENUM_KHR;
	if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		surfaceTransformFlagBits = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else
	{
		surfaceTransformFlagBits = surfaceCapabilities.currentTransform;
	}

	/// Specify how to handle swap chain images that will be used across multiple queue families, 
	/// That will be the case in application if the graphics queue family is different from the presentation queue

	/// VK_SHARING_MODE_EXCLUSIVE: An image is owned by one queue family at a time and ownership must be explicitly transfered before using 
	/// it in another queue family.This option offers the best performance

	/// VK_SHARING_MODE_CONCURRENT: Images can be used across multiple queue families without explicit ownership transfers,
	/// concurrent mode requires you to specify at least two distinct queue families
	uint32_t minImageCount = std::min<uint32_t>(surfaceCapabilities.minImageCount + 1u, surfaceCapabilities.maxImageCount);
	VkSwapchainKHR oldSwapchain = m_Handle;
	VkSwapchainCreateInfoKHR createInfo
	{
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		nullptr,
		0u,
		m_Surface->get(),
		presentMode == VK_PRESENT_MODE_MAILBOX_KHR ? minImageCount : surfaceCapabilities.minImageCount,
		m_ColorFormat,
		ColorSpace,
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

	/// If the clipped member is set to VK_TRUE then that means that we don��t care about the color of pixels that are
	/// obscured, for example because another window is in front of them.Unless you really need to be able to read these 
	/// pixels back and get predictable results, you will get the best performance by enabling clipping

	VERIFY_VK(vkCreateSwapchainKHR(m_Device, &createInfo, VK_MEMORY_ALLOCATOR, reference()));

	if (oldSwapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(m_Device, oldSwapchain, VK_MEMORY_ALLOCATOR);
		oldSwapchain = VK_NULL_HANDLE;
	}

	createResources();
}

void VulkanSwapchain::recreate()
{
	destroyResources();

	VkSurfaceCapabilitiesKHR surfaceCapabilities{};
	if (VK_ERROR_SURFACE_LOST_KHR == vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface->get(), &surfaceCapabilities))
	{
		if (isValid())
		{
			vkDestroySwapchainKHR(m_Device, get(), VK_MEMORY_ALLOCATOR);
			m_Handle = VK_NULL_HANDLE;
		}

		m_Surface.reset();
		m_Surface = std::make_unique<VulkanSurface>(m_AppInstance, m_WindowHandle, m_Instance);
	}

	create();
}

void VulkanSwapchain::createResources()
{
#if 0
	m_PresentCompleteSemaphore = VulkanAsyncPool::instance()->allocSemaphore();
	uint32_t imageCount = 0u;
	GfxVerifyVk(vkGetSwapchainImagesKHR(m_LogicDevice, Handle, &imageCount, nullptr));
	std::vector<VkImage> images(imageCount);
	m_BackBufferImages.resize(imageCount);
	m_BackBuffers.resize(imageCount);
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

	for (uint32_t i = 0u; i < images.size(); ++i)
	{
		GfxFrameBufferDesc desc;
		desc.ColorSurface[0] = std::static_pointer_cast<GfxRenderSurface>(m_BackBufferImages[i]);
		desc.DepthSurface = std::static_pointer_cast<GfxRenderSurface>(m_DepthStencil);
		desc.Width = sizeExtent.width;
		desc.Height = sizeExtent.height;

		auto renderPass = VulkanRenderPassManager::instance()->getOrCreateRenderPass(desc);
		m_BackBuffers[i] = std::make_shared<VulkanFrameBuffer>(m_LogicDevice, renderPass->Handle, desc);
	}
#endif
}

void VulkanSwapchain::destroyResources()
{
}

NAMESPACE_END(Gfx)

#if 0

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
		m_BackBuffers[i]->destroy(m_LogicDevice);
	}
	m_BackBufferImages.clear();
	m_BackBuffers.clear();

	m_DepthStencil->destroy(m_LogicDevice);
}
#endif
