#include "VulkanObject.h"

void VulkanSurface::Create(const VulkanInstance &inst, ::HWND window)
{
	m_Inst = inst;

	VkWin32SurfaceCreateInfoKHR createInfo =
	{
		VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		nullptr,
		0U,
		::GetModuleHandle(nullptr),
		window
	};

	VKCheck(vkCreateWin32SurfaceKHR(inst.GetRef(), &createInfo, nullptr, MakeObject()));
}

std::array<uint32_t, 2> VulkanSwapchain::ValidPresentingSupport(const VulkanPhysicalDevice &vkpDevice)
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

void VulkanSwapchain::Create(const VulkanPhysicalDevice &vkpDevice, const VulkanDevice &device, const VulkanInstance &inst, 
	uint32_t width, uint32_t height, ::HWND window)
{
	assert(device.IsValid());

	m_Device = device;

	m_Surface.Create(inst, window);

	std::array<uint32_t, 2> queueFamilyIndices = ValidPresentingSupport(vkpDevice);

	VkSurfaceCapabilitiesKHR surfCapabilities;
	VKCheck(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkpDevice.GetRef(), m_Surface.GetRef(), &surfCapabilities));

	uint32_t formatCount = 0U;
	VKCheck(vkGetPhysicalDeviceSurfaceFormatsKHR(vkpDevice.GetRef(), m_Surface.GetRef(), &formatCount, nullptr));

	std::vector<VkSurfaceFormatKHR> formats(formatCount);
	VKCheck(vkGetPhysicalDeviceSurfaceFormatsKHR(vkpDevice.GetRef(), m_Surface.GetRef(), &formatCount, formats.data()));

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

	bool sameIndex = queueFamilyIndices[0] == queueFamilyIndices[1];
	VkSwapchainCreateInfoKHR createInfo =
	{
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		nullptr,
		0U,
		m_Surface.GetRef(),
		surfCapabilities.minImageCount,
		format,
		VK_COLORSPACE_SRGB_NONLINEAR_KHR,
		swapchainExtent,
		1U,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		sameIndex ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
		sameIndex ? 0U : 2U,
		sameIndex ? nullptr : queueFamilyIndices.data(),
		surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : surfCapabilities.currentTransform,
		compositeAlpha,
		VK_PRESENT_MODE_FIFO_KHR,
		true,
		VK_NULL_HANDLE
	};

	VKCheck(vkCreateSwapchainKHR(m_Device.GetRef(), &createInfo, nullptr, MakeObject()));
}
