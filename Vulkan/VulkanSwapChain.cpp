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
	m_PresentMode.resize(count);
	VKCheck(vkGetPhysicalDeviceSurfacePresentModesKHR(VulkanEngine::Instance().GetVulkanDevice().GetPhysicalDevice(), m_Handle, &count, m_PresentMode.data()));

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

void VulkanSwapchain::Create(::HWND hWnd, uint32_t uWidth, uint32_t uHeight, bool bWindowed)
{
	if (!m_Surface.IsValid())
	{
		m_Surface.Create(hWnd);
	}

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
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_SHARING_MODE_EXCLUSIVE,
		VulkanEngine::Instance().GetVulkanDevice().GetQueueFamilyIndex(),
		nullptr,
		VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		m_PresentMode,
		true,
		m_Handle
	};

	VKCheck(vkCreateSwapchainKHR(VulkanEngine::Instance().GetDevice(), &createInfo, nullptr, &m_Handle));
}

void VulkanSwapchain::Resize(uint32_t uWidth, uint32_t uHeight)
{

}

void VulkanSwapchain::CreateFrameBuffer(uint32_t width, uint32_t height, VkRenderPass renderPass)
{
	//uint32_t imageCount = 0U;
	//VkImage *pImage = new VkImage[imageCount]();
	//VKCheck(vkGetSwapchainImagesKHR(VulkanEngine::Instance().GetDevice(), m_Handle, &imageCount, nullptr));
	//VKCheck(vkGetSwapchainImagesKHR(VulkanEngine::Instance().GetDevice(), m_Handle, &imageCount, pImage));

	//for (uint32_t i = 0U; i < imageCount; ++i)
	//{
	//	FrameImage &image = m_Images[i];

	//	VkImageViewCreateInfo viewInfo;
	//	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	//	viewInfo.pNext = NULL;
	//	viewInfo.format = (VkFormat)m_Format;
	//	viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
	//	viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	//	viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	//	viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	//	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	//	viewInfo.subresourceRange.baseMipLevel = 0;
	//	viewInfo.subresourceRange.levelCount = 1;
	//	viewInfo.subresourceRange.baseArrayLayer = 0;
	//	viewInfo.subresourceRange.layerCount = 1;
	//	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	//	viewInfo.flags = 0;
	//	viewInfo.image = pImage[i];

	//	image.Image = pImage[i];
	//	VKCheck(vkCreateImageView(VulkanEngine::Instance().GetDevice(), &viewInfo, nullptr, &image.View));

	//	VkFramebufferCreateInfo fbInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
	//	fbInfo.attachmentCount = 2;
	//	fbInfo.pAttachments = &image.View;
	//	fbInfo.width = width;
	//	fbInfo.height = height;
	//	fbInfo.layers = 1;
	//	fbInfo.renderPass = renderPass;
	//	VKCheck(vkCreateFramebuffer(VulkanEngine::Instance().GetDevice(), &fbInfo, nullptr, &image.FrameBuffer));
	//}
}