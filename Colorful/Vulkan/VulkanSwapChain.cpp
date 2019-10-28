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
		/// use a value like VK_IMAGE_USAGE_TRANSFER_DST_BIT instead and use a memory operation to transfer the rendered image to a swap chain image
		usageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;  
	}

	/// VK_PRESENT_MODE_IMMEDIATE_KHR: Images submitted by your application are transferred to the screen right away, which may result in tearing

	/// VK_PRESENT_MODE_FIFO_KHR: The swap chain is a queue where the display takes an image from the front of the queue when the display is 
	/// refreshed and the program inserts rendered images at the back of the queue.If the queue is full then the program has to wait.
	/// This is most similar to vertical sync as found in modern games.The moment that the display is refreshed is known as ¡°vertical blank¡±
	
	/// VK_PRESENT_MODE_FIFO_RELAXED_KHR: This mode only differs from the previous one if the application is late and the queue was empty at the last
	/// vertical blank.Instead of waiting for the next vertical blank, the image is transferred right away when it finally arrives.This may result in visible tearing
	
	/// VK_PRESENT_MODE_MAILBOX_KHR: This is another variation of the second mode. Instead of blocking the application when the queue is full, the
	/// images that are already queued are simply replaced with the newer ones. This mode can be used to implement triple buffering, which allows you 
	/// to avoid tearing with significantly less latency issues than standard vertical sync that uses double buffering

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

	/// Specify how to handle swap chain images that will be used across multiple queue families, 
	/// That will be the case in application if the graphics queue family is different from the presentation queue

	/// VK_SHARING_MODE_EXCLUSIVE: An image is owned by one queue family at a time and ownership must be explicitly transfered before using 
	/// it in another queue family.This option offers the best performance

	/// VK_SHARING_MODE_CONCURRENT: Images can be used across multiple queue families without explicit ownership transfers,
	/// concurrent mode requires you to specify at least two distinct queue families

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
		1U, /// This is always 1 unless you are developing a stereoscopic 3D application
		usageFlags,
		VK_SHARING_MODE_EXCLUSIVE,
		0U,
		nullptr,
		VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,  /// The compositeAlpha field specifies if the alpha channel should be used for blending with other windows in the window system
		m_PresentMode,
		VK_TRUE, 
		VK_NULL_HANDLE
	};

	/// If the clipped member is set to VK_TRUE then that means that we don¡¯t care about the color of pixels that are
	/// obscured, for example because another window is in front of them.Unless you really need to be able to read these 
	/// pixels back and get predictable results, you¡¯ll get the best performance by enabling clipping

	Check(vkCreateSwapchainKHR(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));

	m_PresentSemaphore.Create();
	m_WaitSemaphore.Create();

	/// ImageView describes how to access the image and which part of the image to access

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

void VulkanSwapchain::Flush()
{
	/// The drawFrame function will perform the following operations
	/// - Acquire an image from the swap chain
	/// - Execute the command buffer with that image as attachment in the framebuffer
	/// - Return the image to the swap chain for presentation
	/// Each of these events is set in motion using a single function call, but they are executed asynchronously.
	/// The function calls will return before the operations are actually finished and the order of execution is also undefined
	Check(vkAcquireNextImageKHR(VulkanEngine::Instance().GetDevice().Get(), m_Handle, UINT64_MAX, m_PresentSemaphore.Get(), VK_NULL_HANDLE, &m_CurrentBackBufferIndex));
	Check(vkWaitForFences(VulkanEngine::Instance().GetDevice().Get(), 1u, &m_BackBuffers[m_CurrentBackBufferIndex].PresentFence, true, UINT64_MAX));
	Check(vkResetFences(VulkanEngine::Instance().GetDevice().Get(), 1u, &m_BackBuffers[m_CurrentBackBufferIndex].PresentFence));

	/// Fences are mainly designed to synchronize your application itself with rendering operation,
	/// whereas semaphores are used to synchronize operations within or across command queues

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