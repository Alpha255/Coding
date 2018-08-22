#include "VulkanEngine.h"

void VulkanEngine::InitLayerProperties()
{
	uint32_t layerCount = 0U;
	VkResult res = VK_NOT_READY;
	std::unique_ptr<VkLayerProperties> vkLayerProperty = nullptr;

	do 
	{
		res = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		if (res || 0U == layerCount)
		{
			return;
		} 

		vkLayerProperty.reset(new VkLayerProperties[layerCount]());
		res = vkEnumerateInstanceLayerProperties(&layerCount, vkLayerProperty.get());
	} while (VK_INCOMPLETE == res);

	for (uint32_t i = 0U; i < layerCount; ++i)
	{

	}
}

void VulkanEngine::Initialize(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed)
{
	///InitLayerProperties();
	VkApplicationInfo appInfo
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		"VulkanEngineApp",
		VK_VERSION_1_0,
		"VulkanEngineApp",
		VK_VERSION_1_0,
		VK_API_VERSION_1_0
	};

	std::array<const char *, 2> extensionNames = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
	VkInstanceCreateInfo instInfo
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,
		0U,
		&appInfo,
	};

	VkInstance vkInstance;
	VKCheck(vkCreateInstance(&instInfo, nullptr, &vkInstance));
	
	uint32_t deviceCount = 0U;
	VKCheck(vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr));

	assert(deviceCount >= 1U);
	std::vector<VkPhysicalDevice> vkDeviceList(deviceCount);
	VKCheck(vkEnumeratePhysicalDevices(vkInstance, &deviceCount, vkDeviceList.data()));

	uint32_t deviceFamilyCount = 0U;
	vkGetPhysicalDeviceQueueFamilyProperties(vkDeviceList[0], &deviceFamilyCount, nullptr);

	assert(deviceFamilyCount > 1U);
	std::vector<VkQueueFamilyProperties> vkQueueFamilyProperties(deviceFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(vkDeviceList[0], &deviceFamilyCount, vkQueueFamilyProperties.data());

	VkPhysicalDeviceMemoryProperties vkDeviceMemoryProperty;
	vkGetPhysicalDeviceMemoryProperties(vkDeviceList[0], &vkDeviceMemoryProperty);

	VkPhysicalDeviceProperties vkDeviceProperty;
	vkGetPhysicalDeviceProperties(vkDeviceList[0], &vkDeviceProperty);
}
