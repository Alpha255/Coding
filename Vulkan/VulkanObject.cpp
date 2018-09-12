#include "VulkanObject.h"
#include "VulkanEngine.h"

void VulkanSwapchain::VulkanSurface::Create(::HWND hWnd)
{
	VkWin32SurfaceCreateInfoKHR createInfo =
	{
		VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		nullptr,
		0U,
		::GetModuleHandle(nullptr),
		hWnd
	};

	VKCheck(vkCreateWin32SurfaceKHR(VulkanEngine::Instance().GetInstance().GetRef(), &createInfo, nullptr, &m_Object));
}

#if 0
std::array<uint32_t, 2> VulkanSwapchain::ValidPresentingSupport()
{
	assert(m_Surface.IsValid());

	uint32_t queueFamilyCount = 0U;
	vkGetPhysicalDeviceQueueFamilyProperties(vkpDevice.GetRef(), &queueFamilyCount, nullptr);
	assert(queueFamilyCount >= 1U);

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(vkpDevice.GetRef(), &queueFamilyCount, queueFamilyProperties.data());
	assert(queueFamilyCount >= 1U);

	std::vector<VkBool32> supportsPresent(queueFamilyCount);
	for (uint32_t i = 0U; i < queueFamilyCount; ++i)
	{
		vkGetPhysicalDeviceSurfaceSupportKHR(vkpDevice.GetRef(), i, m_Surface.GetRef(), &supportsPresent[i]);
	}

	uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
	uint32_t presentQueueFamilyIndex = UINT32_MAX;

	for (uint32_t i = 0U; i < queueFamilyCount; ++i)
	{
		if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
		{
			graphicsQueueFamilyIndex = graphicsQueueFamilyIndex == UINT32_MAX ? i : graphicsQueueFamilyIndex;

			if (VK_TRUE == supportsPresent[i])
			{
				graphicsQueueFamilyIndex = i;
				presentQueueFamilyIndex = i;
				break;
			}
		}
	}

	if (UINT32_MAX == presentQueueFamilyIndex)
	{
		for (uint32_t i = 0U; i < queueFamilyCount; ++i)
		{
			if (VK_TRUE == supportsPresent[i])
			{
				presentQueueFamilyIndex = i;
				break;
			}
		}
	}

	assert(graphicsQueueFamilyIndex != UINT32_MAX && presentQueueFamilyIndex != UINT32_MAX);

	return std::array<uint32_t, 2>{ graphicsQueueFamilyIndex, presentQueueFamilyIndex };
}
#endif

void VulkanSwapchain::Create(::HWND hWnd, uint32_t width, uint32_t height)
{
	m_Surface.Create(hWnd);

	///std::array<uint32_t, 2> queueFamilyIndices = ValidPresentingSupport(vkpDevice);

	///VkSurfaceCapabilitiesKHR surfCapabilities;
	///VKCheck(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkpDevice.GetRef(), m_Surface.GetRef(), &surfCapabilities));

	///uint32_t formatCount = 0U;
	///VKCheck(vkGetPhysicalDeviceSurfaceFormatsKHR(vkpDevice.GetRef(), m_Surface.GetRef(), &formatCount, nullptr));

	///std::vector<VkSurfaceFormatKHR> formats(formatCount);
	///VKCheck(vkGetPhysicalDeviceSurfaceFormatsKHR(vkpDevice.GetRef(), m_Surface.GetRef(), &formatCount, formats.data()));

	///VkFormat format = VK_FORMAT_UNDEFINED;
	///if (1U == formatCount && VK_FORMAT_UNDEFINED == formats[0].format)
	///{
	///	format = VK_FORMAT_B8G8R8A8_UNORM;
	///}
	///else
	///{
	///	assert(formatCount > 1U);
	///	format = formats[0].format;
	///}

	///VkExtent2D swapchainExtent;
	///if (0xFFFFFFFF == surfCapabilities.currentExtent.width)
	///{
	///	swapchainExtent.width = std::max<uint32_t>(width, surfCapabilities.minImageExtent.width);
	///	swapchainExtent.width = std::min<uint32_t>(swapchainExtent.width, surfCapabilities.maxImageExtent.width);
	
	///	swapchainExtent.height = std::max<uint32_t>(height, surfCapabilities.minImageExtent.height);
	///	swapchainExtent.height = std::min<uint32_t>(swapchainExtent.height, surfCapabilities.maxImageExtent.height);
	///}
	///else
	///{
	///	swapchainExtent = surfCapabilities.currentExtent;
	///}
	
	///VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	///VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[] = 
	///{
	///	VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
	///	VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
	///	VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
	///	VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
	///};
	///for (uint32_t i = 0; i < sizeof(compositeAlphaFlags); ++i) 
	///{
	///	if (surfCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) 
	///	{
	///		compositeAlpha = compositeAlphaFlags[i];
	///		break;
	///	}
	///}
	
	///bool sameIndex = queueFamilyIndices[0] == queueFamilyIndices[1];
	VkExtent2D extent = 
	{
		width,
		height
	};

	VkSwapchainCreateInfoKHR createInfo =
	{
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		nullptr,
		0U,
		m_Surface.GetRef(),
		0U,///surfCapabilities.minImageCount,
		VK_FORMAT_B8G8R8A8_UNORM,///format,
		VK_COLORSPACE_SRGB_NONLINEAR_KHR,
		extent, ///swapchainExtent,
		1U,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_SHARING_MODE_EXCLUSIVE, ///sameIndex ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
		0U, ///sameIndex ? 0U : 2U,
		nullptr, ///sameIndex ? nullptr : queueFamilyIndices.data(),
		VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR, ///surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : surfCapabilities.currentTransform,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, ///compositeAlpha,
		VK_PRESENT_MODE_FIFO_KHR,
		true,
		VK_NULL_HANDLE
	};

	VKCheck(vkCreateSwapchainKHR(VulkanEngine::Instance().GetDevice().GetRef(), &createInfo, nullptr, &m_Object));
}
