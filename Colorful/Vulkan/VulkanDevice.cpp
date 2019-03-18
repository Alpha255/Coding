#include "VulkanDevice.h"
#include "VulkanEngine.h"

static void VerifyLayerProperties(const std::vector<VkLayerProperties> supportedLayers, const std::vector<const char *> layers)
{
	for each (auto layer in layers)
	{
		auto it = supportedLayers.begin();
		for (; it != supportedLayers.end(); ++it)
		{
			if (_stricmp(it->layerName, layer) == 0)
			{
				break;
			}
		}

		if (it == supportedLayers.end())
		{
			assert(0);
		}
	}
}

static void VerifyExtensionProperties(const std::vector<VkExtensionProperties> &supportedExtensions, const std::vector<const char *> extensions)
{
	for each (auto extension in extensions)
	{
		auto it = supportedExtensions.begin();
		for (; it != supportedExtensions.end(); ++it)
		{
			if (_stricmp(it->extensionName, extension) == 0)
			{
				break;
			}
		}

		if (it == supportedExtensions.end())
		{
			assert(0);
		}
	}
}

#if 0
static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugReportFunc(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objectType,
	uint64_t object,
	size_t location,
	int32_t messageCode,
	const char* pLayerPrefix,
	const char* pMessage,
	void* pUserData)
{
	Base::Log("[Vulkan] ObjectType: %i\nMessage: %s", objectType, pMessage);

	return VK_FALSE;
}
#endif

void VulkanInstance::Create(const std::string &appName, const std::string &engineName)
{
	assert(!IsValid());

	std::vector<const char*> layers =
	{
		"VK_LAYER_LUNARG_standard_validation",
#if defined(_DEBUG)
		"VK_LAYER_LUNARG_core_validation",
		"VK_LAYER_LUNARG_object_tracker",
		"VK_LAYER_LUNARG_parameter_validation"
#endif
	};

	std::vector<const char*> extensions =
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME
	};

	uint32_t count = 0U;
	Check(vkEnumerateInstanceLayerProperties(&count, nullptr));
	std::vector<VkLayerProperties> supportedLayers(count);
	Check(vkEnumerateInstanceLayerProperties(&count, supportedLayers.data()));
	VerifyLayerProperties(supportedLayers, layers);

	Check(vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));
	std::vector<VkExtensionProperties> supportedExtensions(count);
	Check(vkEnumerateInstanceExtensionProperties(nullptr, &count, supportedExtensions.data()));
	VerifyExtensionProperties(supportedExtensions, extensions);

	VkApplicationInfo appInfo
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		appName.c_str(),
		0U,
		engineName.c_str(),
		0U,
		VK_API_VERSION_1_0
	};

	VkInstanceCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,
		0U,
		&appInfo,
		(uint32_t)layers.size(),
		layers.data(),
		(uint32_t)extensions.size(),
		extensions.data()
	};

	Check(vkCreateInstance(&createInfo, nullptr, &m_Handle));

#if 0
	auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Handle, "vkCreateDebugReportCallbackEXT");
	assert(vkCreateDebugReportCallbackEXT != NULL);

	VkDebugReportCallbackCreateInfoEXT debugReportInfo
	{
		VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
		nullptr,
		VK_DEBUG_REPORT_INFORMATION_BIT_EXT | 
		VK_DEBUG_REPORT_ERROR_BIT_EXT | 
		VK_DEBUG_REPORT_WARNING_BIT_EXT | 
		VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | 
		VK_DEBUG_REPORT_DEBUG_BIT_EXT,
		VulkanDebugReportFunc,
		nullptr
	};
	Check(vkCreateDebugReportCallbackEXT(m_Handle, &debugReportInfo, nullptr, &m_DebugReportCallback));
#endif
}

void VulkanPhysicalDevice::Create()
{
	uint32_t count = 0U;
	Check(vkEnumeratePhysicalDevices(VulkanEngine::Instance().GetInstance().Get(), &count, nullptr));
	assert(count > 0U);
	m_PhysicalDevices.resize(count);
	Check(vkEnumeratePhysicalDevices(VulkanEngine::Instance().GetInstance().Get(), &count, m_PhysicalDevices.data()));

	for each (auto physicalDevice in m_PhysicalDevices)
	{
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilyProperties(count);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queueFamilyProperties.data());

		for (uint32_t i = 0U; i < queueFamilyProperties.size(); ++i)
		{
			if (queueFamilyProperties[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT))
			{
				m_QueueFamilyIndex = i;
				break;
			}
		}
	}
	assert(m_QueueFamilyIndex != UINT32_MAX);
	Reset(m_PhysicalDevices[m_QueueFamilyIndex]);

	Check(vkEnumerateDeviceLayerProperties(m_Handle, &count, nullptr));

	vkGetPhysicalDeviceFeatures(m_Handle, &m_DeviceFeatures);

	vkGetPhysicalDeviceProperties(m_Handle, &m_DeviceProperties);

	vkGetPhysicalDeviceMemoryProperties(m_Handle, &m_DeviceMemoryProperties);
}

void VulkanDevice::Create()
{
	assert(!IsValid());

	m_PhysicalDevice.Create();

	std::vector<const char *> layers =
	{
#if defined(_DEBUG)
		"VK_LAYER_LUNARG_standard_validation",
		"VK_LAYER_LUNARG_parameter_validation",
		"VK_LAYER_LUNARG_object_tracker",
		"VK_LAYER_LUNARG_core_validation"
#endif
	};

	std::vector<const char *> extensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		///VK_EXT_DEBUG_MARKER_EXTENSION_NAME
	};

	uint32_t count = 0U;
	Check(vkEnumerateDeviceLayerProperties(m_PhysicalDevice.Get(), &count, nullptr));
	std::vector<VkLayerProperties> supportedLayers(count);
	Check(vkEnumerateDeviceLayerProperties(m_PhysicalDevice.Get(), &count, supportedLayers.data()));
	VerifyLayerProperties(supportedLayers, layers);

	Check(vkEnumerateDeviceExtensionProperties(m_PhysicalDevice.Get(), nullptr, &count, nullptr));
	std::vector<VkExtensionProperties> supportedExtensions(count);
	Check(vkEnumerateDeviceExtensionProperties(m_PhysicalDevice.Get(), nullptr, &count, supportedExtensions.data()));
	VerifyExtensionProperties(supportedExtensions, extensions);

	std::vector<float> queuePriorities = 
	{ 
		0.0f 
	};

	std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos = 
	{
		{
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			nullptr,
			0U,
			m_PhysicalDevice.GetQueueFamilyIndex(),
			1U,
			queuePriorities.data()
		}
	};
	/// pQueuePriorities is an array of queueCount normalized floating point values, 
	/// specifying priorities of work that will be submitted to each created queue

	VkDeviceCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		nullptr,
		0U,
		(uint32_t)deviceQueueCreateInfos.size(),
		deviceQueueCreateInfos.data(),
		(uint32_t)layers.size(),
		layers.data(),
		(uint32_t)extensions.size(),
		extensions.data(),
		&m_PhysicalDevice.GetDeviceFeatures()
	};

	Check(vkCreateDevice(m_PhysicalDevice.Get(), &createInfo, nullptr, &m_Handle));

	vkGetDeviceQueue(m_Handle, m_PhysicalDevice.GetQueueFamilyIndex(), 0U, &m_DeviceQueue);
}

//uint32_t VulkanPhysicalDevice::GetOptimalSurfaceFormat(uint32_t flags, bool bDepthSurface) const
//{
//	assert(IsValid());
//
//	VkFormat result = VK_FORMAT_UNDEFINED;
//
//	static const VkFormat colorFormats[] =
//	{
//		VK_FORMAT_B8G8R8A8_UNORM,
//	};
//
//	static const VkFormat depthFormats[] =
//	{
//		VK_FORMAT_D24_UNORM_S8_UINT,
//		VK_FORMAT_D32_SFLOAT_S8_UINT,
//		VK_FORMAT_D16_UNORM_S8_UINT,
//	};
//
//	const VkFormat *pTargetFormats = bDepthSurface ? depthFormats : colorFormats;
//	uint32_t count = bDepthSurface ? _countof(depthFormats) : _countof(colorFormats);
//	for (uint32_t i = 0; i < count; ++i)
//	{
//		VkFormatProperties formatProperties = {};
//		vkGetPhysicalDeviceFormatProperties(m_Handle, *pTargetFormats, &formatProperties);
//		if (formatProperties.optimalTilingFeatures & flags)
//		{
//			result = *pTargetFormats;
//			break;
//		}
//		++pTargetFormats;
//	}
//
//	return result;
//}