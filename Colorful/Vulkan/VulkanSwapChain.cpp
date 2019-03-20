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

	Check(vkCreateWin32SurfaceKHR(VulkanEngine::Instance().GetInstance().Get(), &createInfo, nullptr, &m_Handle));

	VkPhysicalDevice physicalDevice = VulkanEngine::Instance().GetDevice().GetPhysicalDevice().Get();

	Check(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Handle, &m_SurfaceCapabilities));

	uint32_t count = 0U;
	Check(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Handle, &count, nullptr));
	m_PresentModes.resize(count);
	Check(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Handle, &count, m_PresentModes.data()));

	VkBool32 checkSupport = 0U;
	Check(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, VulkanEngine::Instance().GetDevice().GetQueueFamilyIndex(), m_Handle, &checkSupport));
	assert(checkSupport);

	Check(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Handle, &count, nullptr));
	m_SurfaceFormats.resize(count);
	Check(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Handle, &count, m_SurfaceFormats.data()));
}

void VulkanSwapchain::Create(::HWND hWnd, uint32_t width, uint32_t height, bool bFullScreen)
{
	VulkanEngine::Instance().GetDevice().Wait();

	m_bFullScreen = bFullScreen;

	m_Surface.Create(hWnd);

#if 0
	auto &surfaceCapabilities = m_Surface.GetCapabilities();
	assert(surfaceCapabilities.maxImageExtent.width && surfaceCapabilities.maxImageExtent.height);
	m_Size.width = std::min<uint32_t>(width, surfaceCapabilities.maxImageExtent.width);
	m_Size.height = std::min<uint32_t>(height, surfaceCapabilities.maxImageExtent.height);

	m_ColorSurfaceFormat = (VkFormat)VulkanEngine::Instance().GetPhysicalDevice().GetOptimalSurfaceFormat(VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, false);
	m_DepthSurfaceFormat = (VkFormat)VulkanEngine::Instance().GetPhysicalDevice().GetOptimalSurfaceFormat(VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, true);
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
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VK_SHARING_MODE_EXCLUSIVE, /// exclusive or concurrent access resource
		VulkanEngine::Instance().GetDevice().GetQueueFamilyIndex(),
		nullptr,
		surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : surfaceCapabilities.currentTransform,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,  /// Alpha = 1.0f
		m_PresentMode,
		true,  /// indicates whether the Vulkan implementation is allowed to discard rendering operations that affect regions of the surface which aren’t visible
		m_Handle
	};

	Check(vkCreateSwapchainKHR(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));

	uint32_t count = 0U;
	Check(vkGetSwapchainImagesKHR(VulkanEngine::Instance().GetDevice().Get(), m_Handle, &count, nullptr));
	std::vector<VkImage> images(count);
	Check(vkGetSwapchainImagesKHR(VulkanEngine::Instance().GetDevice().Get(), m_Handle, &count, images.data()));

	m_DepthStencilImage.Create(VulkanImage::eImage2D, width, height, 1U, m_DepthSurfaceFormat, 1U, 1U, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_IMAGE_LAYOUT_UNDEFINED);
	m_DepthStencilView.CreateAsTexture(eTexture2D, m_DepthStencilImage, m_DepthSurfaceFormat, 0U, 1U);

	m_BackBuffers.resize(count);
	for (uint32_t i = 0U; i < count; ++i)
	{
		VulkanImage image(images[i]);
		m_BackBuffers[i].RenderTargetView.CreateAsTexture(eTexture2D, image, m_ColorSurfaceFormat, 1U);
		m_BackBuffers[i].PresentFence.Create(VulkanFence::eSignaled);
		m_BackBuffers[i].ImageAcquireSemaphore.Create();
		m_BackBuffers[i].RenderCompleteSemaphore.Create();
		m_BackBuffers[i].CommandBuffer = VulkanEngine::Instance().AllocCommandBuffer(VulkanCommandPool::eGeneral, VulkanCommandPool::ePrimary);
		
		std::vector<VulkanImageView> imageViews;
		imageViews.emplace_back(m_BackBuffers[i].RenderTargetView);
		imageViews.emplace_back(m_DepthStencilView);
		m_BackBuffers[i].FrameBuffer.Create(imageViews, VulkanEngine::Instance().GetDefaultRenderPass(), uWidth, uHeight, 1U);
	}
#endif
}

void VulkanSwapchain::Resize(uint32_t uWidth, uint32_t uHeight)
{
	///Create(nullptr, uWidth, uHeight, m_bFullScreen);
}

#if 0
void VulkanSwapchain::Begin()
{
	VKCheck(vkAcquireNextImageKHR(VulkanEngine::Instance().GetDevice(), m_Handle, UINT64_MAX, m_BackBuffers[m_CurBackBufferIndex].ImageAcquireSemaphore.Get(), VK_NULL_HANDLE, &m_CurBackBufferIndex));

	VKCheck(vkWaitForFences(VulkanEngine::Instance().GetDevice(), 1U, &m_BackBuffers[m_CurBackBufferIndex].PresentFence, VK_TRUE, UINT64_MAX));

	VKCheck(vkResetFences(VulkanEngine::Instance().GetDevice(), 1U, &m_BackBuffers[m_CurBackBufferIndex].PresentFence));

	m_BackBuffers[m_CurBackBufferIndex].CommandBuffer.ResetCommand(0U);

	m_BackBuffers[m_CurBackBufferIndex].CommandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkClearValue clearValue = {};
	memcpy(clearValue.color.float32, &Color::DarkBlue, sizeof(Vec4));
	VkRenderPassBeginInfo info
	{
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		nullptr,
		VulkanEngine::Instance().GetDefaultRenderPass().Get(),
		m_BackBuffers[m_CurBackBufferIndex].FrameBuffer.Get(),
		{ 0U, 0U, 800U, 600U },
		1U,
		&clearValue
	};
	vkCmdBeginRenderPass(m_BackBuffers[m_CurBackBufferIndex].CommandBuffer.Get(), &info, VK_SUBPASS_CONTENTS_INLINE);

	m_CurBackBufferIndex = (m_CurBackBufferIndex + 1) % m_BackBuffers.size();
}

void VulkanSwapchain::End()
{
	vkCmdEndRenderPass(m_BackBuffers[m_CurBackBufferIndex].CommandBuffer.Get());

	VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo info
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		1U,
		&m_BackBuffers[m_CurBackBufferIndex].ImageAcquireSemaphore,
		&wait_stage,
		1U,
		&m_BackBuffers[m_CurBackBufferIndex].CommandBuffer,
		1U,
		&m_BackBuffers[m_CurBackBufferIndex].RenderCompleteSemaphore
	};
	m_BackBuffers[m_CurBackBufferIndex].CommandBuffer.End();

	VKCheck(vkQueueSubmit(VulkanEngine::Instance().GetQueue(), 1U, &info, m_BackBuffers[m_CurBackBufferIndex].PresentFence.Get()));
}

void VulkanSwapchain::Flush()
{
#if 0
	static uint32_t s_FrameIndex = 0U;
	if (s_FrameIndex == UINT32_MAX)
	{
		s_FrameIndex = 0U;
	}
	m_CurBackBufferIndex = s_FrameIndex % m_BackBuffers.size();

	VKCheck(vkDeviceWaitIdle(VulkanEngine::Instance().GetDevice()));

	VkResult result = vkGetFenceStatus(VulkanEngine::Instance().GetDevice(), m_BackBuffers[m_CurBackBufferIndex].PresentFence.Get());
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

	VKCheck(vkQueuePresentKHR(VulkanEngine::Instance().GetQueue(), &presentInfo));

	VKCheck(vkQueueSubmit(VulkanEngine::Instance().GetQueue(), 0U, nullptr, m_BackBuffers[m_CurBackBufferIndex].PresentFence.Get()));

	++s_FrameIndex;
#endif

	//VkPresentInfoKHR presentInfo
	//{
	//	VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
	//	nullptr,
	//	1U,
	//	&m_BackBuffers[m_CurBackBufferIndex].RenderCompleteSemaphore,
	//	1U,
	//	&m_Handle,
	//	&m_CurBackBufferIndex,
	//	nullptr
	//};

	//VKCheck(vkQueuePresentKHR(VulkanEngine::Instance().GetQueue(), &presentInfo));
}
#endif

void VulkanSwapchain::Destory()
{
	assert(IsValid());

	for (uint32_t i = 0U; i < m_BackBuffers.size(); ++i)
	{
		m_BackBuffers[i].ImageAcquireSemaphore.Destory();
		///m_BackBuffers[i].FrameBuffer.Destory();
		m_BackBuffers[i].PresentFence.Destory();
		m_BackBuffers[i].RenderCompleteSemaphore.Destory();
		///m_BackBuffers[i].RenderTargetView.Destory();
	}
	m_BackBuffers.clear();

	vkDestroySwapchainKHR(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	Reset();
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