#include "VulkanObject.h"

void VulkanSurface::Create(const VulkanInstance & inst, const ::HWND window)
{
	VkWin32SurfaceCreateInfoKHR createInfo =
	{
		VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		nullptr,
		0U,
		nullptr,
		window
	};

	VKCheck(vkCreateWin32SurfaceKHR(inst.GetRef(), &createInfo, nullptr, MakeObject()));
}

void VulkanSwapchain::ValidPresentingSupport(const VulkanPhysicalDevice &vkpDevice)
{
	uint32_t queueFamilyCount = 0U;
	vkGetPhysicalDeviceQueueFamilyProperties(vkpDevice.GetRef(), &queueFamilyCount, nullptr);
	assert(queueFamilyCount >= 1U);

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
}

void VulkanSwapchain::Create(const VulkanPhysicalDevice &vkpDevice, const VulkanDevice &device, uint32_t width, uint32_t height)
{
	assert(device.IsValid());

	m_Device = device;

	ValidPresentingSupport(vkpDevice);

	VkSurfaceCapabilitiesKHR surfCapabilities;
	///VKCheck(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkpDevice, ));
}
