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
}

void VulkanSwapchain::Init(::HWND hWnd)
{
	//m_Surface.Create(hWnd);

	//auto queueFamilyProterties = VulkanEngine::Instance().GetPhysicalDevice().GetQueueFamilyProperties();

	//std::vector<VkBool32> supportsPresent(queueFamilyProterties.size());
	//for (uint32_t i = 0U; i < supportsPresent.size(); ++i)
	//{
	//	VKCheck(vkGetPhysicalDeviceSurfaceSupportKHR(VulkanEngine::Instance().GetPhysicalDevice().Get(), i, m_Surface.Get(), &supportsPresent[i]));
	//}

	//for (uint32_t i = 0U; i < supportsPresent.size(); ++i)
	//{
	//	if ((queueFamilyProterties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
	//	{
	//		m_GraphicsQueueFamilyIndex = m_GraphicsQueueFamilyIndex == UINT32_MAX ? i : m_GraphicsQueueFamilyIndex;

	//		if (VK_TRUE == supportsPresent[i])
	//		{
	//			m_GraphicsQueueFamilyIndex = i;
	//			m_PresentQueueFamilyIndex = i;
	//			break;
	//		}
	//	}
	//}

	//if (UINT32_MAX == m_PresentQueueFamilyIndex)
	//{
	//	for (uint32_t i = 0U; i < supportsPresent.size(); ++i)
	//	{
	//		if (VK_TRUE == supportsPresent[i])
	//		{
	//			m_PresentQueueFamilyIndex = i;
	//			break;
	//		}
	//	}
	//}

	//assert(m_GraphicsQueueFamilyIndex != UINT32_MAX && m_PresentQueueFamilyIndex != UINT32_MAX);
}

void VulkanSwapchain::Create(uint32_t width, uint32_t height, bool bWindowed)
{
	//VkSurfaceCapabilitiesKHR surfCapabilities{};
	//VKCheck(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VulkanEngine::Instance().GetPhysicalDevice().Get(), m_Surface.Get(), &surfCapabilities));

	//uint32_t formatCount = 0U;
	//VKCheck(vkGetPhysicalDeviceSurfaceFormatsKHR(VulkanEngine::Instance().GetPhysicalDevice().Get(), m_Surface.Get(), &formatCount, nullptr));

	//std::vector<VkSurfaceFormatKHR> formats(formatCount);
	//VKCheck(vkGetPhysicalDeviceSurfaceFormatsKHR(VulkanEngine::Instance().GetPhysicalDevice().Get(), m_Surface.Get(), &formatCount, formats.data()));

	//if (1U == formatCount && VK_FORMAT_UNDEFINED == formats[0].format)
	//{
	//	m_Format = VK_FORMAT_B8G8R8A8_UNORM;
	//}
	//else
	//{
	//	assert(formatCount > 1U);
	//	m_Format = formats[0].format;
	//}

	//VkExtent2D swapchainExtent;
	//if (0xFFFFFFFF == surfCapabilities.currentExtent.width)
	//{
	//	swapchainExtent.width = std::max<uint32_t>(width, surfCapabilities.minImageExtent.width);
	//	swapchainExtent.width = std::min<uint32_t>(swapchainExtent.width, surfCapabilities.maxImageExtent.width);

	//	swapchainExtent.height = std::max<uint32_t>(height, surfCapabilities.minImageExtent.height);
	//	swapchainExtent.height = std::min<uint32_t>(swapchainExtent.height, surfCapabilities.maxImageExtent.height);
	//}
	//else
	//{
	//	swapchainExtent = surfCapabilities.currentExtent;
	//}

	//VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	//VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[] = 
	//{
	//	VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
	//	VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
	//	VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
	//	VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
	//};
	//for (uint32_t i = 0; i < sizeof(compositeAlphaFlags); ++i) 
	//{
	//	if (surfCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) 
	//	{
	//		compositeAlpha = compositeAlphaFlags[i];
	//		break;
	//	}
	//}

	//bool sameIndex = m_GraphicsQueueFamilyIndex == m_PresentQueueFamilyIndex;
	//uint32_t queueFamilyIndices[] = { m_GraphicsQueueFamilyIndex, m_PresentQueueFamilyIndex };
	//VkSwapchainCreateInfoKHR createInfo =
	//{
	//	VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
	//	nullptr,
	//	0U, /// flags
	//	m_Surface.Get(),
	//	surfCapabilities.minImageCount,
	//	(VkFormat)m_Format,
	//	VK_COLORSPACE_SRGB_NONLINEAR_KHR,
	//	swapchainExtent,
	//	1U,
	//	VK_IMAGE_USAGE_TRANSFER_SRC_BIT & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
	//	sameIndex ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
	//	sameIndex ? 0U : 2U,
	//	sameIndex ? nullptr : queueFamilyIndices,
	//	surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : surfCapabilities.currentTransform,
	//	compositeAlpha,
	//	VK_PRESENT_MODE_IMMEDIATE_KHR,
	//	true,
	//	VK_NULL_HANDLE
	//};

	//VKCheck(vkCreateSwapchainKHR(VulkanEngine::Instance().GetDevice(), &createInfo, nullptr, &m_Handle));
}

void VulkanSwapchain::CreateFrameBuffer(uint32_t width, uint32_t height, VkRenderPass renderPass)
{
	uint32_t imageCount = 0U;
	VkImage *pImage = new VkImage[imageCount]();
	VKCheck(vkGetSwapchainImagesKHR(VulkanEngine::Instance().GetDevice(), m_Handle, &imageCount, nullptr));
	VKCheck(vkGetSwapchainImagesKHR(VulkanEngine::Instance().GetDevice(), m_Handle, &imageCount, pImage));

	for (uint32_t i = 0U; i < imageCount; ++i)
	{
		FrameImage &image = m_Images[i];

		VkImageViewCreateInfo viewInfo;
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.pNext = NULL;
		viewInfo.format = (VkFormat)m_Format;
		viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
		viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
		viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
		viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.flags = 0;
		viewInfo.image = pImage[i];

		image.Image = pImage[i];
		VKCheck(vkCreateImageView(VulkanEngine::Instance().GetDevice(), &viewInfo, nullptr, &image.View));

		VkFramebufferCreateInfo fbInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		fbInfo.attachmentCount = 2;
		fbInfo.pAttachments = &image.View;
		fbInfo.width = width;
		fbInfo.height = height;
		fbInfo.layers = 1;
		fbInfo.renderPass = renderPass;
		VKCheck(vkCreateFramebuffer(VulkanEngine::Instance().GetDevice(), &fbInfo, nullptr, &image.FrameBuffer));
	}
}