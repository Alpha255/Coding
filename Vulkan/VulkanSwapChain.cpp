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

	VKCheck(vkCreateWin32SurfaceKHR(VulkanEngine::Instance().GetInstance().Get(), &createInfo, nullptr, &m_Handle));
}

void VulkanSwapchain::Init(::HWND hWnd)
{
	m_Surface.Create(hWnd);

	auto queueFamilyProterties = VulkanEngine::Instance().GetPhysicalDevice().GetQueueFamilyProperties();

	std::vector<VkBool32> supportsPresent(queueFamilyProterties.size());
	for (uint32_t i = 0U; i < supportsPresent.size(); ++i)
	{
		VKCheck(vkGetPhysicalDeviceSurfaceSupportKHR(VulkanEngine::Instance().GetPhysicalDevice().Get(), i, m_Surface.Get(), &supportsPresent[i]));
	}

	for (uint32_t i = 0U; i < supportsPresent.size(); ++i)
	{
		if ((queueFamilyProterties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
		{
			m_GraphicsQueueFamilyIndex = m_GraphicsQueueFamilyIndex == UINT32_MAX ? i : m_GraphicsQueueFamilyIndex;

			if (VK_TRUE == supportsPresent[i])
			{
				m_GraphicsQueueFamilyIndex = i;
				m_PresentQueueFamilyIndex = i;
				break;
			}
		}
	}

	if (UINT32_MAX == m_PresentQueueFamilyIndex)
	{
		for (uint32_t i = 0U; i < supportsPresent.size(); ++i)
		{
			if (VK_TRUE == supportsPresent[i])
			{
				m_PresentQueueFamilyIndex = i;
				break;
			}
		}
	}

	assert(m_GraphicsQueueFamilyIndex != UINT32_MAX && m_PresentQueueFamilyIndex != UINT32_MAX);
}

void VulkanSwapchain::Create(uint32_t width, uint32_t height, bool bWindowed)
{
	VkSurfaceCapabilitiesKHR surfCapabilities{};
	VKCheck(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VulkanEngine::Instance().GetPhysicalDevice().Get(), m_Surface.Get(), &surfCapabilities));

	uint32_t formatCount = 0U;
	VKCheck(vkGetPhysicalDeviceSurfaceFormatsKHR(VulkanEngine::Instance().GetPhysicalDevice().Get(), m_Surface.Get(), &formatCount, nullptr));

	std::vector<VkSurfaceFormatKHR> formats(formatCount);
	VKCheck(vkGetPhysicalDeviceSurfaceFormatsKHR(VulkanEngine::Instance().GetPhysicalDevice().Get(), m_Surface.Get(), &formatCount, formats.data()));

	VkFormat format = VK_FORMAT_UNDEFINED;
	if (1U == formatCount && VK_FORMAT_UNDEFINED == formats[0].format)
	{
		format = VK_FORMAT_B8G8R8A8_UNORM;
	}
	else
	{
		assert(formatCount > 1U);
		format = formats[0].format;
	}

	VkExtent2D swapchainExtent;
	if (0xFFFFFFFF == surfCapabilities.currentExtent.width)
	{
		swapchainExtent.width = std::max<uint32_t>(width, surfCapabilities.minImageExtent.width);
		swapchainExtent.width = std::min<uint32_t>(swapchainExtent.width, surfCapabilities.maxImageExtent.width);

		swapchainExtent.height = std::max<uint32_t>(height, surfCapabilities.minImageExtent.height);
		swapchainExtent.height = std::min<uint32_t>(swapchainExtent.height, surfCapabilities.maxImageExtent.height);
	}
	else
	{
		swapchainExtent = surfCapabilities.currentExtent;
	}

	VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[] = 
	{
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
	};
	for (uint32_t i = 0; i < sizeof(compositeAlphaFlags); ++i) 
	{
		if (surfCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) 
		{
			compositeAlpha = compositeAlphaFlags[i];
			break;
		}
	}

	bool sameIndex = m_GraphicsQueueFamilyIndex == m_PresentQueueFamilyIndex;
	uint32_t queueFamilyIndices[] = { m_GraphicsQueueFamilyIndex, m_PresentQueueFamilyIndex };
	VkSwapchainCreateInfoKHR createInfo =
	{
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		nullptr,
		0U, /// flags
		m_Surface.Get(),
		surfCapabilities.minImageCount,
		format,
		VK_COLORSPACE_SRGB_NONLINEAR_KHR,
		swapchainExtent,
		1U,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		sameIndex ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
		sameIndex ? 0U : 2U,
		sameIndex ? nullptr : queueFamilyIndices,
		surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : surfCapabilities.currentTransform,
		compositeAlpha,
		VK_PRESENT_MODE_FIFO_KHR,
		true,
		VK_NULL_HANDLE
	};

	VKCheck(vkCreateSwapchainKHR(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));
}