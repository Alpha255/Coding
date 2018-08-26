#include "VulkanEngine.h"

std::unique_ptr<VkEngine, std::function<void(VkEngine *)>> VkEngine::s_Instance;

void VkEngine::InitLayerProperties()
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

void VkEngine::Initialize(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed)
{
	///InitLayerProperties();
	VkApplicationInfo appInfo
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		"VulkanApp",
		0U,
		nullptr,
		0U,
		VK_API_VERSION_1_0
	};

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
