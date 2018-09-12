#include "VulkanEngine.h"

std::unique_ptr<VulkanEngine, std::function<void(VulkanEngine *)>> VulkanEngine::s_Instance;

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

void VulkanEngine::CreateDevice()
{
	uint32_t deviceCount = 0U;
	VKCheck(vkEnumeratePhysicalDevices(m_Instance.GetRef(), &deviceCount, nullptr));
	assert(deviceCount >= 1U);

	VKCheck(vkEnumeratePhysicalDevices(m_Instance.GetRef(), &deviceCount, m_PhysicalDevice.Get()));
	assert(deviceCount >= 1U);

	const float queuePriorities[] = { 0.0f };
	VkDeviceQueueCreateInfo queueInfo =
	{
		VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		nullptr,
		0U,
		0U,
		1U,
		queuePriorities
	};

	const char *pExtensionNames[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	VkDeviceCreateInfo deviceInfo =
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		nullptr,
		0U,
		1U,
		&queueInfo,
		0U,
		nullptr,
		1U,
		pExtensionNames,
		nullptr
	};

	VKCheck(vkCreateDevice(m_PhysicalDevice.GetRef(), &deviceInfo, nullptr, m_Device.Get()));
}

void VulkanEngine::Initialize(::HWND hWnd, uint32_t width, uint32_t height, bool bWindowed)
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

	const char *pExtensionNames[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
	VkInstanceCreateInfo instInfo
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,
		0U,
		&appInfo,
		0U,
		nullptr,
		2U,
		pExtensionNames
	};

	VKCheck(vkCreateInstance(&instInfo, nullptr, m_Instance.Get()));

	CreateDevice();

	m_Swapchain.Create(hWnd, width, height);
}
