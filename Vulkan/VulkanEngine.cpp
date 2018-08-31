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

void VkEngine::CreateDevice(const VulkanPhysicalDevice &vkpDevice)
{
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

	VKCheck(vkCreateDevice(vkpDevice.GetRef(), &deviceInfo, nullptr, m_Device.MakeObject()));
}

void VkEngine::Initialize(::HWND hWnd, uint32_t width, uint32_t height, bool bWindowed)
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

	VKCheck(vkCreateInstance(&instInfo, nullptr, m_Instance.MakeObject()));
	
	uint32_t deviceCount = 0U;
	VKCheck(vkEnumeratePhysicalDevices(m_Instance.GetRef(), &deviceCount, nullptr));
	assert(deviceCount >= 1U);

	VulkanPhysicalDevice vkpDevice;
	VKCheck(vkEnumeratePhysicalDevices(m_Instance.GetRef(), &deviceCount, vkpDevice.MakeObject()));

	CreateDevice(vkpDevice);

	m_Swapchain.Create(vkpDevice, m_Device, m_Instance, width, height, hWnd);
}
