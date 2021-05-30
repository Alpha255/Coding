#include "Colorful/Vulkan/VulkanSwapChain.h"
#include "Colorful/Vulkan/VulkanDevice.h"
#include "Colorful/Vulkan/VulkanQueue.h"

NAMESPACE_START(Gfx)

VulkanSurface::VulkanSurface(uint64_t windowHandle)
{
	assert(windowHandle);

#if defined(PLATFORM_WIN32)
	VkWin32SurfaceCreateInfoKHR createInfo
	{
		VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		nullptr,
		0u,  /// reserved for future use
		reinterpret_cast<::HINSTANCE>(System::getModuleHandle()),
		reinterpret_cast<::HWND>(windowHandle)
	};

	VERIFY_VK(vkCreateWin32SurfaceKHR(s_Device->instance(), &createInfo, VK_MEMORY_ALLOCATOR, reference()));
#else
	assert(0);
#endif
}

VulkanSurface::~VulkanSurface()
{
	vkDestroySurfaceKHR(s_Device->instance(), get(), VK_MEMORY_ALLOCATOR);
}

VulkanSwapchain::VulkanSwapchain(
	uint64_t windowHandle,
	uint32_t width,
	uint32_t height,
	bool8_t fullscreen,
	bool8_t VSync,
	bool8_t sRGB)
	: m_WindowHandle(windowHandle)
	, m_FullScreen(fullscreen)
	, m_VSync(VSync)
	, m_Width(width)
	, m_Height(height)
	, m_ColorFormat(sRGB ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_B8G8R8A8_UNORM)
{
	m_Surface = std::make_unique<VulkanSurface>(windowHandle);

	create();
}

VkColorSpaceKHR VulkanSwapchain::getProperFormats()
{
	uint32_t count = 0u;
	VERIFY_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(s_Device->physicalDevice(), m_Surface->get(), &count, nullptr));
	assert(count > 0u);
	std::vector<VkSurfaceFormatKHR> surfaceFormats(count);
	VERIFY_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(s_Device->physicalDevice(), m_Surface->get(), &count, surfaceFormats.data()));

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

	std::vector<VkFormat> depthStencilFormats
	{
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM
	};
	for (auto& format : depthStencilFormats)
	{
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(s_Device->physicalDevice(), format, &formatProperties);
		if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			m_DepthStencilFormat = format;
			break;
		}
	}
	assert(m_DepthStencilFormat != VkFormat::VK_FORMAT_UNDEFINED);

	return ColorSpace;
}

void VulkanSwapchain::create()
{
	/// VK_PRESENT_MODE_IMMEDIATE_KHR: Images submitted by your application are transferred to the screen right away, which may result in tearing

	/// VK_PRESENT_MODE_FIFO_KHR: The swap chain is a queue where the display takes an image from the front of the queue when the display is 
	/// refreshed and the program inserts rendered images at the back of the queue.If the queue is full then the program has to wait.
	/// This is most similar to vertical sync as found in modern games.The moment that the display is refreshed is known as ¡°vertical blank¡±

	/// VK_PRESENT_MODE_FIFO_RELAXED_KHR: This mode only differs from the previous one if the application is late and the queue was empty at the last
	/// vertical blank.Instead of waiting for the next vertical blank, the image is transferred right away when it finally arrives.This may result in visible tearing

	/// VK_PRESENT_MODE_MAILBOX_KHR: This is another variation of the second mode. Instead of blocking the application when the queue is full, the
	/// images that are already queued are simply replaced with the newer ones. This mode can be used to implement triple buffering, which allows you 
	/// to avoid tearing with significantly less latency issues than standard vertical sync that uses double buffering
	auto ColorSpace = getProperFormats();

	uint32_t count = 0u;
	VERIFY_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(s_Device->physicalDevice(), m_Surface->get(), &count, nullptr));
	assert(count > 0u);
	std::vector<VkPresentModeKHR> presentModes(count);
	VERIFY_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(s_Device->physicalDevice(), m_Surface->get(), &count, presentModes.data()));

	VkPresentModeKHR presentMode = m_VSync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR;
	VERIFY(std::find(presentModes.cbegin(), presentModes.cend(), presentMode) != presentModes.end());

	vkGetPhysicalDeviceQueueFamilyProperties(s_Device->physicalDevice(), &count, nullptr);
	assert(count > 0u);
	std::vector<VkQueueFamilyProperties> queueFamilyProperties(count);
	vkGetPhysicalDeviceQueueFamilyProperties(s_Device->physicalDevice(), &count, queueFamilyProperties.data());
	std::vector<VkBool32> surfaceSupportKHRs(count);
	for (uint32_t i = 0u; i < count; ++i)
	{
		VERIFY_VK(vkGetPhysicalDeviceSurfaceSupportKHR(s_Device->physicalDevice(), i, m_Surface->get(), &surfaceSupportKHRs[i]));
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

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	VERIFY_VK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(s_Device->physicalDevice(), m_Surface->get(), &surfaceCapabilities));

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
	for (auto& compositeAlphaFlagBit : compositeAlphaFlagBitsArray)
	{
		if (surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlagBit)
		{
			compositeAlphaFlagBits = compositeAlphaFlagBit;
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
		VK_TRUE, /// clipped specifies whether the Vulkan implementation is allowed to discard rendering operations that affect regions of the surface that are not visible.
		oldSwapchain
	};

	/// If the clipped member is set to VK_TRUE then that means that we don¡¯t care about the color of pixels that are
	/// obscured, for example because another window is in front of them.Unless you really need to be able to read these 
	/// pixels back and get predictable results, you will get the best performance by enabling clipping

	VERIFY_VK(vkCreateSwapchainKHR(s_Device->get(), &createInfo, VK_MEMORY_ALLOCATOR, reference()));

	if (oldSwapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(s_Device->get(), oldSwapchain, VK_MEMORY_ALLOCATOR);
		oldSwapchain = VK_NULL_HANDLE;
	}

	uint32_t imageCount = 0u;
	VERIFY_VK(vkGetSwapchainImagesKHR(s_Device->get(), get(), &imageCount, nullptr));

	m_BackBuffer.clear();
	m_BackBuffer.ColorTargets.resize(imageCount);
	std::vector<VkImage> images(imageCount);
	VERIFY_VK(vkGetSwapchainImagesKHR(s_Device->get(), get(), &imageCount, images.data()));
	for (uint32_t i = 0u; i < imageCount; ++i)
	{
		m_BackBuffer.ColorTargets[i] = create_shared<VulkanTexture>(images[i], m_ColorFormat);
	}
}

VkFramebuffer VulkanSwapchain::currentFramebuffer(VkRenderPass renderPass)
{
	assert(renderPass); /// Double check description ????

	if (m_BackBuffer.FrameBuffers.empty())
	{
		TextureDesc depthStencilDesc
		{
			ETextureType::T_2D,
			FormatAttribute::attribute_Vk(m_DepthStencilFormat).Format,
			EBufferUsage::Default,
			EBindFlags::DepthStencil,
			m_Width,
			m_Height
		};
		m_BackBuffer.DepthStencil = create_shared<VulkanTexture>(depthStencilDesc, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);

		m_BackBuffer.FrameBuffers.resize(m_BackBuffer.ColorTargets.size());
		for (uint32_t i = 0u; i < m_BackBuffer.ColorTargets.size(); ++i)
		{
			FrameBufferDesc frameBufferDesc
			{
				{ m_BackBuffer.ColorTargets[i] },
				m_BackBuffer.DepthStencil,
				m_Width,
				m_Height,
			};
			m_BackBuffer.FrameBuffers[i] = create_shared<VulkanFramebuffer>(renderPass, frameBufferDesc);
		}
	}

	assert(m_CurImageIndex < m_BackBuffer.FrameBuffers.size());
	return m_BackBuffer.FrameBuffers[m_CurImageIndex]->get();
}

void VulkanSwapchain::recreate()
{
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	if (VK_ERROR_SURFACE_LOST_KHR == vkGetPhysicalDeviceSurfaceCapabilitiesKHR(s_Device->physicalDevice(), m_Surface->get(), &surfaceCapabilities))
	{
		if (isValid())
		{
			vkDestroySwapchainKHR(s_Device->get(), get(), VK_MEMORY_ALLOCATOR);
			m_Handle = VK_NULL_HANDLE;
		}

		m_Surface.reset();
		m_Surface = create_unique<VulkanSurface>(m_WindowHandle);
	}

	create();
}

void VulkanSwapchain::present()
{
	/// swapchain must not be in the retired state
	/// If semaphore is not VK_NULL_HANDLE it must be unsignaled
	/// If semaphore is not VK_NULL_HANDLE it must not have any uncompleted signal or wait operations pending
	/// If fence is not VK_NULL_HANDLE it must be unsignaled and must not be associated with any other queue command that has not yet completed execution on that queue
	/// semaphore and fence must not both be equal to VK_NULL_HANDLE

	m_CurImageIndex = ~0u;
	VERIFY_VK(vkAcquireNextImageKHR(s_Device->get(), get(), UINT64_MAX, m_PresentComplete->get(), VK_NULL_HANDLE, &m_CurImageIndex));

	VkQueues::instance().queuePresent(get(), m_PresentComplete->get(), &m_CurImageIndex);
}

VulkanSwapchain::~VulkanSwapchain()
{
	m_BackBuffer.clear();
	vkDestroySwapchainKHR(s_Device->get(), get(), VK_MEMORY_ALLOCATOR);
}

NAMESPACE_END(Gfx)
