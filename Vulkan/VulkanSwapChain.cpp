#include "VulkanSwapChain.h"
#include "VulkanEngine.h"

void VulkanSurface::Create(::HWND hWnd)
{
	VkWin32SurfaceCreateInfoKHR createInfo =
	{
		VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		nullptr,
		0U,
		::GetModuleHandle(nullptr),
		hWnd
	};

	VKCheck(vkCreateWin32SurfaceKHR(VulkanEngine::Instance().GetInstance(), &createInfo, nullptr, &m_Handle));

	VKCheck(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VulkanEngine::Instance().GetVulkanDevice().GetPhysicalDevice(), m_Handle, &m_Capabilities));

	uint32_t count = 0U;
	VKCheck(vkGetPhysicalDeviceSurfacePresentModesKHR(VulkanEngine::Instance().GetVulkanDevice().GetPhysicalDevice(), m_Handle, &count, nullptr));
	m_PresentModes.resize(count);
	VKCheck(vkGetPhysicalDeviceSurfacePresentModesKHR(VulkanEngine::Instance().GetVulkanDevice().GetPhysicalDevice(), m_Handle, &count, m_PresentModes.data()));

	VkBool32 checkSupport = 0U;
	VKCheck(vkGetPhysicalDeviceSurfaceSupportKHR(VulkanEngine::Instance().GetVulkanDevice().GetPhysicalDevice(), VulkanEngine::Instance().GetVulkanDevice().GetQueueFamilyIndex(), m_Handle, &checkSupport));

	if (!checkSupport)
	{
		System::Log("Physical device dose not support WSI!!!");
		assert(checkSupport);
	}

	VKCheck(vkGetPhysicalDeviceSurfaceFormatsKHR(VulkanEngine::Instance().GetVulkanDevice().GetPhysicalDevice(), m_Handle, &count, nullptr));
	m_Formats.resize(count);
	VKCheck(vkGetPhysicalDeviceSurfaceFormatsKHR(VulkanEngine::Instance().GetVulkanDevice().GetPhysicalDevice(), m_Handle, &count, m_Formats.data()));
}

void VulkanSwapchain::Create(::HWND hWnd, uint32_t uWidth, uint32_t uHeight, bool bFullScreen)
{
	///VKCheck(vkDeviceWaitIdle(VulkanEngine::Instance().GetDevice()));

	m_bFullScreen = bFullScreen;

	m_Surface.Create(hWnd);

	auto &surfaceCapabilities = m_Surface.GetCapabilities();
	assert(surfaceCapabilities.maxImageExtent.width && surfaceCapabilities.maxImageExtent.height);
	m_Size.width = std::min<uint32_t>(uWidth, surfaceCapabilities.maxImageExtent.width);
	m_Size.height = std::min<uint32_t>(uHeight, surfaceCapabilities.maxImageExtent.height);

	m_ColorSurfaceFormat = (VkFormat)VulkanEngine::Instance().GetVulkanDevice().GetOptimalSurfaceFormat(VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, false);
	m_DepthSurfaceFormat = (VkFormat)VulkanEngine::Instance().GetVulkanDevice().GetOptimalSurfaceFormat(VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, true);
	assert(m_ColorSurfaceFormat != VK_FORMAT_UNDEFINED && m_DepthSurfaceFormat != VK_FORMAT_UNDEFINED);

	auto &surfaceFormats = m_Surface.GetFormats();
	uint32_t formatIndex = 0U;
	for each (auto &format in surfaceFormats)
	{
		if (m_ColorSurfaceFormat == format.format)
		{
			break;
		}
		++formatIndex;
	}

	auto &presentModes = m_Surface.GetPresentModes();
	m_PresentMode = VK_PRESENT_MODE_FIFO_KHR;
	for each (auto presentMode in presentModes)
	{
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			m_PresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}
	}

	VkSwapchainCreateInfoKHR createInfo
	{
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		nullptr,
		0U, 
		m_Surface.Get(),
		surfaceCapabilities.minImageCount,
		surfaceFormats[formatIndex].format,
		surfaceFormats[formatIndex].colorSpace,
		m_Size,
		1U,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_SHARING_MODE_EXCLUSIVE, /// exclusive or concurrent access resource
		VulkanEngine::Instance().GetVulkanDevice().GetQueueFamilyIndex(),
		nullptr,
		surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : surfaceCapabilities.currentTransform,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,  /// Alpha = 1.0f
		m_PresentMode,
		true,  /// indicates whether the Vulkan implementation is allowed to discard rendering operations that affect regions of the surface which aren’t visible
		m_Handle
	};

	VKCheck(vkCreateSwapchainKHR(VulkanEngine::Instance().GetDevice(), &createInfo, nullptr, &m_Handle));

	uint32_t count = 0U;
	VKCheck(vkGetSwapchainImagesKHR(VulkanEngine::Instance().GetDevice(), m_Handle, &count, nullptr));

	std::vector<VkImage> images(count);
	VKCheck(vkGetSwapchainImagesKHR(VulkanEngine::Instance().GetDevice(), m_Handle, &count, images.data()));

	VulkanImage depthStencilImage;
	depthStencilImage.Create(VulkanImage::eImage2D, uWidth, uHeight, 1U, m_DepthSurfaceFormat, 1U, 1U, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_LAYOUT_UNDEFINED);
	m_DepthStencilView.CreateAsTexture(eTexture2D, depthStencilImage, m_DepthSurfaceFormat, 0U, 1U);

	m_BackBuffers.resize(count);
	for (uint32_t i = 0U; i < count; ++i)
	{
		VulkanImage image(images[i]);
		m_BackBuffers[i].RenderTargetView.CreateAsTexture(eTexture2D, image, m_ColorSurfaceFormat, 1U);
		m_BackBuffers[i].PresentFence.Create(VulkanFence::eUnSignaled);
		m_BackBuffers[i].AcquireSemaphore.Create();
		m_BackBuffers[i].RenderSemaphore.Create();
		
		std::vector<VulkanImageView> imageViews;
		imageViews.emplace_back(m_BackBuffers[i].RenderTargetView);
		imageViews.emplace_back(m_DepthStencilView);
		m_BackBuffers[i].FrameBuffer.Create(imageViews, VulkanEngine::Instance().GetDefaultRenderPass(), uWidth, uHeight, 1U);
	}

	///VKCheck(vkDeviceWaitIdle(VulkanEngine::Instance().GetDevice()));
}

void VulkanSwapchain::Resize(uint32_t uWidth, uint32_t uHeight)
{
	///Create(nullptr, uWidth, uHeight, m_bFullScreen);
}

void VulkanSwapchain::Flush()
{
	static uint32_t s_FrameIndex = 0U;
	if (s_FrameIndex == UINT32_MAX)
	{
		s_FrameIndex = 0U;
	}
	m_CurBackBufferIndex = s_FrameIndex % m_BackBuffers.size();

	VkResult result = vkDeviceWaitIdle(VulkanEngine::Instance().GetDevice());

	result = vkGetFenceStatus(VulkanEngine::Instance().GetDevice(), m_BackBuffers[m_CurBackBufferIndex].PresentFence.Get());
	if (VK_NOT_READY == result)
	{
		VKCheck(vkWaitForFences(VulkanEngine::Instance().GetDevice(), 1U, &m_BackBuffers[m_CurBackBufferIndex].PresentFence, true, UINT64_MAX));
	}
	else if (VK_SUCCESS == result)
	{
		VKCheck(vkResetFences(VulkanEngine::Instance().GetDevice(), 1U, &m_BackBuffers[m_CurBackBufferIndex].PresentFence));
	}
	else
	{
		System::Log("Vulkan Error: %d", result);
		assert(0);
	}

	VKCheck(vkAcquireNextImageKHR(VulkanEngine::Instance().GetDevice(), m_Handle, UINT64_MAX, m_BackBuffers[m_CurBackBufferIndex].AcquireSemaphore.Get(), VK_NULL_HANDLE, &m_CurBackBufferIndex));

	VkPresentInfoKHR presentInfo
	{
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		nullptr,
		1U,
		&m_BackBuffers[m_CurBackBufferIndex].RenderSemaphore,
		1U,
		&m_Handle,
		&m_CurBackBufferIndex,
		nullptr
	};

	VKCheck(vkQueuePresentKHR(VulkanEngine::Instance().GetVulkanDevice().GetQueue(), &presentInfo));

	VKCheck(vkQueueSubmit(VulkanEngine::Instance().GetVulkanDevice().GetQueue(), 0U, nullptr, m_BackBuffers[m_CurBackBufferIndex].PresentFence.Get()));

	++s_FrameIndex;
}

//void VulkanSwapchain::CreateFrameBuffer(uint32_t width, uint32_t height, VkRenderPass renderPass)
//{
//	//uint32_t imageCount = 0U;
//	//VkImage *pImage = new VkImage[imageCount]();
//	//VKCheck(vkGetSwapchainImagesKHR(VulkanEngine::Instance().GetDevice(), m_Handle, &imageCount, nullptr));
//	//VKCheck(vkGetSwapchainImagesKHR(VulkanEngine::Instance().GetDevice(), m_Handle, &imageCount, pImage));
//
//	//for (uint32_t i = 0U; i < imageCount; ++i)
//	//{
//	//	FrameImage &image = m_Images[i];
//
//	//	VkImageViewCreateInfo viewInfo;
//	//	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//	//	viewInfo.pNext = NULL;
//	//	viewInfo.format = (VkFormat)m_Format;
//	//	viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
//	//	viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
//	//	viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
//	//	viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
//	//	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//	//	viewInfo.subresourceRange.baseMipLevel = 0;
//	//	viewInfo.subresourceRange.levelCount = 1;
//	//	viewInfo.subresourceRange.baseArrayLayer = 0;
//	//	viewInfo.subresourceRange.layerCount = 1;
//	//	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
//	//	viewInfo.flags = 0;
//	//	viewInfo.image = pImage[i];
//
//	//	image.Image = pImage[i];
//	//	VKCheck(vkCreateImageView(VulkanEngine::Instance().GetDevice(), &viewInfo, nullptr, &image.View));
//
//	//	VkFramebufferCreateInfo fbInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
//	//	fbInfo.attachmentCount = 2;
//	//	fbInfo.pAttachments = &image.View;
//	//	fbInfo.width = width;
//	//	fbInfo.height = height;
//	//	fbInfo.layers = 1;
//	//	fbInfo.renderPass = renderPass;
//	//	VKCheck(vkCreateFramebuffer(VulkanEngine::Instance().GetDevice(), &fbInfo, nullptr, &image.FrameBuffer));
//	//}
//}