#include <Windows.h>
#include <stdint.h>
#include <vector>
#include <assert.h>
#include <Vulkan/vulkan.h>
#include <Vulkan/vulkan_win32.h>

#define check(func) if (VK_SUCCESS != func) { assert(0); }

struct VkQueueFamilyIndex
{
	uint32_t Graphics;
	uint32_t Compute;
	uint32_t Transfer;
};

VkInstance vkInstance;
std::vector<VkPhysicalDevice> vkPhysicalDevices;
VkDevice vkDevice;
VkPhysicalDeviceProperties vkDeviceProperties;
VkPhysicalDeviceFeatures vkDeviceFeatures;
VkPhysicalDeviceMemoryProperties vkDeviceMemoryProperties;
VkPhysicalDeviceFeatures vkEnabledFeatures;
std::vector<VkQueueFamilyProperties> vkQueueFamilyProperties;
VkQueueFamilyIndex vkQueueFamilyIndex;
std::vector<const char*> vkEnabledDeviceExtensions;
std::vector<const char*> vkEnabledInstanceExtensions;
std::vector<std::string> vkSupportedExtensions;

bool ExtensionSupported(std::string extension)
{
	return (std::find(vkSupportedExtensions.begin(), vkSupportedExtensions.end(), extension) != vkSupportedExtensions.end());
}

uint32_t GetQueueFamilyIndex(VkQueueFlagBits queueFlags)
{
	if (queueFlags & VK_QUEUE_COMPUTE_BIT)
	{
		for (uint32_t i = 0; i < static_cast<uint32_t>(vkQueueFamilyProperties.size()); i++)
		{
			if ((vkQueueFamilyProperties[i].queueFlags & queueFlags) && 
				((vkQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
			{
				return i;
				break;
			}
		}
	}

	if (queueFlags & VK_QUEUE_TRANSFER_BIT)
	{
		for (uint32_t i = 0; i < static_cast<uint32_t>(vkQueueFamilyProperties.size()); i++)
		{
			if ((vkQueueFamilyProperties[i].queueFlags & queueFlags) && 
				((vkQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && 
				((vkQueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
			{
				return i;
				break;
			}
		}
	}

	for (uint32_t i = 0; i < vkQueueFamilyProperties.size(); i++)
	{
		if (vkQueueFamilyProperties[i].queueFlags & queueFlags)
		{
			return i;
			break;
		}
	}

	return UINT32_MAX;
}

void CreateInstance()
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "VulkanTest";
	appInfo.pEngineName = "VulkanTest";
	appInfo.apiVersion = VK_API_VERSION_1_0;

	std::vector<const char*> instanceExtensions = 
	{ 
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME
	};

	const char *validationLayerNames[] = 
	{
		"VK_LAYER_LUNARG_standard_validation"
	};

	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = nullptr;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
	instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
	instanceCreateInfo.enabledLayerCount = 1U;
	instanceCreateInfo.ppEnabledLayerNames = validationLayerNames;

	check(vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance));
}

void CreateDevice()
{
	uint32_t gpuCount = 0;
	check(vkEnumeratePhysicalDevices(vkInstance, &gpuCount, nullptr));
	assert(gpuCount > 0);

	vkPhysicalDevices.resize(gpuCount);
	check(vkEnumeratePhysicalDevices(vkInstance, &gpuCount, vkPhysicalDevices.data()));

	vkGetPhysicalDeviceProperties(vkPhysicalDevices[0], &vkDeviceProperties);
	vkGetPhysicalDeviceFeatures(vkPhysicalDevices[0], &vkDeviceFeatures);
	vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevices[0], &vkDeviceMemoryProperties);

	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevices[0], &queueFamilyCount, nullptr);
	assert(queueFamilyCount > 0);
	vkQueueFamilyProperties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevices[0], &queueFamilyCount, vkQueueFamilyProperties.data());

	uint32_t extCount = 0;
	vkEnumerateDeviceExtensionProperties(vkPhysicalDevices[0], nullptr, &extCount, nullptr);
	if (extCount > 0)
	{
		std::vector<VkExtensionProperties> extensions(extCount);
		if (vkEnumerateDeviceExtensionProperties(vkPhysicalDevices[0], nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
		{
			for (auto ext : extensions)
			{
				vkSupportedExtensions.emplace_back(ext.extensionName);
			}
		}
	}

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	const float defaultQueuePriority(0.0f);
	VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
	if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT)
	{
		vkQueueFamilyIndex.Graphics = GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
		VkDeviceQueueCreateInfo queueInfo{};
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.queueFamilyIndex = vkQueueFamilyIndex.Graphics;
		queueInfo.queueCount = 1;
		queueInfo.pQueuePriorities = &defaultQueuePriority;
		queueCreateInfos.emplace_back(queueInfo);
	}
	else
	{
		vkQueueFamilyIndex.Graphics = 0U;
	}

	if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
	{
		vkQueueFamilyIndex.Compute = GetQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);
		if (vkQueueFamilyIndex.Compute != vkQueueFamilyIndex.Graphics)
		{
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = vkQueueFamilyIndex.Compute;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &defaultQueuePriority;
			queueCreateInfos.emplace_back(queueInfo);
		}
	}
	else
	{
		vkQueueFamilyIndex.Compute = vkQueueFamilyIndex.Graphics;
	}

	if (requestedQueueTypes & VK_QUEUE_TRANSFER_BIT)
	{
		vkQueueFamilyIndex.Transfer = GetQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT);
		if ((vkQueueFamilyIndex.Transfer != vkQueueFamilyIndex.Graphics) &&
			(vkQueueFamilyIndex.Transfer != vkQueueFamilyIndex.Compute))
		{
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = vkQueueFamilyIndex.Transfer;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &defaultQueuePriority;
			queueCreateInfos.push_back(queueInfo);
		}
	}
	else
	{
		vkQueueFamilyIndex.Transfer = vkQueueFamilyIndex.Graphics;
	}

	std::vector<const char*> deviceExtensions(vkEnabledDeviceExtensions);
	deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.pEnabledFeatures = &vkEnabledFeatures;

	if (ExtensionSupported(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
	{
		deviceExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
	}

	if (deviceExtensions.size() > 0)
	{
		deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	}

	check(vkCreateDevice(vkPhysicalDevices[0], &deviceCreateInfo, nullptr, &vkDevice));
}

int32_t WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int32_t /*nShow*/)
{
	CreateInstance();

	CreateDevice();

	return 0;
}