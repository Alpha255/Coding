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

void VulkanSurface::Destory()
{
	assert(IsValid());
	vkDestroySurfaceKHR(VulkanEngine::Instance().GetInstance().Get(), m_Handle, nullptr);
	Reset();
}

void VulkanSwapchain::Initialize(::HWND hWnd, uint32_t width, uint32_t height, bool bFullScreen)
{
	m_bFullScreen = bFullScreen;
	m_hWnd = hWnd;

	m_Surface.Create(hWnd);

	auto &surfaceCapabilities = m_Surface.GetCapabilities();
	assert(surfaceCapabilities.maxImageExtent.width && surfaceCapabilities.maxImageExtent.height);
	m_Size.width = std::min<uint32_t>(width, surfaceCapabilities.maxImageExtent.width);
	m_Size.height = std::min<uint32_t>(height, surfaceCapabilities.maxImageExtent.height);

	m_ColorFormat = VK_FORMAT_B8G8R8A8_UNORM;

	std::vector<VkFormat> depthFormats
	{
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM
	};
	for each(auto format in depthFormats)
	{
		VkFormatProperties formatProperties = {};
		vkGetPhysicalDeviceFormatProperties(VulkanEngine::Instance().GetDevice().GetPhysicalDevice().Get(), format, &formatProperties);
		if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			m_DepthStencilFormat = format;
			break;
		}
	}
	assert(m_DepthStencilFormat != VK_FORMAT_UNDEFINED);
}

void VulkanSwapchain::Create()
{
	VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_MAX_ENUM_KHR;

	auto &surfaceFormats = m_Surface.GetFormats();
	for each (auto surfaceFormat in surfaceFormats)
	{
		if (surfaceFormat.format == m_ColorFormat)
		{
			colorSpace = surfaceFormat.colorSpace;
			break;
		}
	}
	assert(colorSpace != VK_COLOR_SPACE_MAX_ENUM_KHR);

	auto &surfaceCapabilities = m_Surface.GetCapabilities();
	uint32_t usageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
	{
		usageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}
	if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
	{
		usageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

	m_PresentMode = VK_PRESENT_MODE_FIFO_KHR;
	auto presentModes = m_Surface.GetPresentModes();
	for (uint32_t i = 0u; i < presentModes.size(); ++i)
	{
		if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
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
		m_ColorFormat,
		colorSpace,
		m_Size,
		1U,
		usageFlags,
		VK_SHARING_MODE_EXCLUSIVE,
		0U,
		nullptr,
		VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,  /// Alpha = 1.0f
		m_PresentMode,
		VK_TRUE, 
		VK_NULL_HANDLE
	};

	Check(vkCreateSwapchainKHR(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));

	m_PresentSemaphore.Create();
	m_WaitSemaphore.Create();

	VulkanImage depthStencilImage;
	depthStencilImage.Create(
		VulkanImage::eImage2D, 
		m_Size.width, 
		m_Size.height,
		1U, 
		m_DepthStencilFormat,
		1U, 
		1U, 
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, 
		VK_IMAGE_LAYOUT_UNDEFINED);
	m_DepthStencilView.Create(depthStencilImage);

	uint32_t count = 0U;
	Check(vkGetSwapchainImagesKHR(VulkanEngine::Instance().GetDevice().Get(), m_Handle, &count, nullptr));
	std::vector<VkImage> images(count);
	Check(vkGetSwapchainImagesKHR(VulkanEngine::Instance().GetDevice().Get(), m_Handle, &count, images.data()));

	m_BackBuffers.resize(count);
	for (uint32_t i = 0U; i < count; ++i)
	{
		VulkanImage image;
		image.Reset(images[i]);

		m_BackBuffers[i].RenderTargetView.CreateAsTexture(eTexture2D, image, m_ColorFormat, 1U, false);
		m_BackBuffers[i].PresentFence.Create(VulkanFence::eSignaled);
		m_BackBuffers[i].CommandBuffer = VulkanEngine::Instance().AllocCommandBuffer(VulkanCommandPool::eGeneral, VulkanCommandPool::ePrimary, 1U);

		std::vector<VkImageView> imageViews
		{
			m_BackBuffers[i].RenderTargetView.Get(),
			m_DepthStencilView.Get()
		};
		m_BackBuffers[i].FrameBuffer.Create(m_Size.width, m_Size.height, imageViews);
	}
}

void VulkanSwapchain::Resize(uint32_t width, uint32_t height)
{
	Check(vkDeviceWaitIdle(VulkanEngine::Instance().GetDevice().Get()));
	Destory();
	Initialize(m_hWnd, width, height, m_bFullScreen);
	Create();
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
#endif

void VulkanSwapchain::Flush()
{
	Check(vkAcquireNextImageKHR(VulkanEngine::Instance().GetDevice().Get(), m_Handle, UINT64_MAX, m_PresentSemaphore.Get(), VK_NULL_HANDLE, &m_CurrentBackBufferIndex));
	Check(vkWaitForFences(VulkanEngine::Instance().GetDevice().Get(), 1u, &m_BackBuffers[m_CurrentBackBufferIndex].PresentFence, true, UINT64_MAX));
	Check(vkResetFences(VulkanEngine::Instance().GetDevice().Get(), 1u, &m_BackBuffers[m_CurrentBackBufferIndex].PresentFence));

	VkQueue deviceQueue = VulkanEngine::Instance().GetDevice().GetQueue().Get();

	VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submitInfo
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		1U,
		&m_PresentSemaphore,
		&pipelineStageFlags,
		1U,
		&m_BackBuffers[m_CurrentBackBufferIndex].CommandBuffer,
		1U,
		&m_WaitSemaphore
	};
	Check(vkQueueSubmit(deviceQueue, 1U, &submitInfo, m_BackBuffers[m_CurrentBackBufferIndex].PresentFence.Get()));

	VkPresentInfoKHR presentInfo
	{
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		nullptr,
		1U,
		&m_WaitSemaphore,
		1U,
		&m_Handle,
		&m_CurrentBackBufferIndex,
		nullptr
	};

	Check(vkQueuePresentKHR(deviceQueue, &presentInfo));

	VulkanEngine::Instance().GetDevice().Wait();
}

void VulkanSwapchain::Destory()
{
	assert(IsValid());

	m_DepthStencilView.Destory();

	for (uint32_t i = 0U; i < m_BackBuffers.size(); ++i)
	{
		m_BackBuffers[i].FrameBuffer.Destory();
		m_BackBuffers[i].RenderTargetView.Destory();
		m_BackBuffers[i].PresentFence.Destory();
	}
	m_BackBuffers.clear();

	vkDestroySwapchainKHR(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	m_PresentSemaphore.Destory();
	m_WaitSemaphore.Destory();

	Reset();

	m_Surface.Destory();
}