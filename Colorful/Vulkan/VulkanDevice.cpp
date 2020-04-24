#include "VulkanDevice.h"
#include "VulkanEngine.h"

template <typename T> std::vector<const char8_t *> getSupportedProperties(
	const std::vector<T> &supportedProperties, 
	const std::vector<const char8_t *> &targetProperties)
{
	std::vector<const char8_t *> result;

	for each (auto propertyName in targetProperties)
	{
		for (auto it = supportedProperties.begin(); it != supportedProperties.end(); ++it)
		{
			if (_stricmp((const char8_t *)(&(*it)), propertyName) == 0)
			{
				result.emplace_back(propertyName);
				break;
			}
		}
	}

	return result;
}

VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugUtilsMessengerFunc(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageServerityFlagBits,
	VkDebugUtilsMessageTypeFlagsEXT messageTypeFlags,
	const VkDebugUtilsMessengerCallbackDataEXT *pMessengerCallbackData,
	void* pUserData)
{
	(void)(pUserData);
	(void)(messageTypeFlags);

	logger::eLogLevel logLevel = logger::eError;
	if (messageServerityFlagBits & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
	{
		logLevel = logger::eInfo;
	}
	else if (messageServerityFlagBits & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
	{
		logLevel = logger::eInfo;
	}
	else if (messageServerityFlagBits & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		logLevel = logger::eWarning;
	}
	else if (messageServerityFlagBits & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		logLevel = logger::eError;
	}

	std::string message = format("[%3d][%10s]: %s", 
		pMessengerCallbackData->messageIdNumber, 
		pMessengerCallbackData->pMessageIdName,
		pMessengerCallbackData->pMessage);
	
	logger::instance().log(logLevel, "Vulkan Validation: %s.", message.c_str());
	return VK_FALSE;
}

void vkInstance::create()
{
	assert(!isValid());

	std::vector<const char8_t*> layers =
	{
		"VK_LAYER_LUNARG_standard_validation",
#if defined(_DEBUG)
		"VK_LAYER_LUNARG_core_validation",
		"VK_LAYER_LUNARG_object_tracker",
		"VK_LAYER_LUNARG_parameter_validation",
		"VK_LAYER_KHRONOS_validation"
#endif
	};

	std::vector<const char8_t*> extensions =
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(Platform_Win32)
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined(Platform_Linux)
		VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME
	};

	uint32_t count = 0U;
	rVerifyVk(vkEnumerateInstanceLayerProperties(&count, nullptr));
	std::vector<VkLayerProperties> supportedLayers(count);
	rVerifyVk(vkEnumerateInstanceLayerProperties(&count, supportedLayers.data()));
	layers = getSupportedProperties<VkLayerProperties>(supportedLayers, layers);

	rVerifyVk(vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));
	std::vector<VkExtensionProperties> supportedExtensions(count);
	rVerifyVk(vkEnumerateInstanceExtensionProperties(nullptr, &count, supportedExtensions.data()));
	extensions = getSupportedProperties<VkExtensionProperties>(supportedExtensions, extensions);

	VkApplicationInfo appInfo
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		"vkApplication",
		0U,
		"vkEngine",
		0U,
		VK_API_VERSION_1_0
	};

	VkInstanceCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,
		0u,
		&appInfo,
		(uint32_t)layers.size(),
		layers.data(),
		(uint32_t)extensions.size(),
		extensions.data()
	};

	VkInstance handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateInstance(&createInfo, vkMemoryAllocator, &handle));
	reset(handle);
}

void vkInstance::destroy()
{
	if (isValid())
	{
		vkDestroyInstance(**this, vkMemoryAllocator);
		reset();
	}
}

void vkDebugUtilsMessenger::create(const vkInstance &instance, bool8_t verbose)
{
	assert(!isValid() && instance.isValid());

	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMesserger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
		vkGetInstanceProcAddr(*instance, "vkCreateDebugUtilsMessengerEXT"));
	assert(vkCreateDebugUtilsMesserger);

	VkDebugUtilsMessageSeverityFlagsEXT messageServityFlags = verbose ? (VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT 
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT 
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT 
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) : 
		(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT);

	VkDebugUtilsMessengerCreateInfoEXT createInfo
	{
		VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		nullptr, 
		0u,
		messageServityFlags,
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT 
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		vkDebugUtilsMessengerFunc,
		nullptr
	};

	VkDebugUtilsMessengerEXT handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateDebugUtilsMesserger(*instance, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);
}

void vkDebugUtilsMessenger::destroy(const vkInstance &instance)
{
	assert(instance.isValid());

	if (isValid())
	{
		PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
			vkGetInstanceProcAddr(*instance, "vkDestroyDebugUtilsMessengerEXT")
			);
		assert(vkDestroyDebugUtilsMessenger);
		vkDestroyDebugUtilsMessenger(*instance, **this, vkMemoryAllocator);
		reset();
	}
}

std::vector<vkPhysicalDevice> vkPhysicalDevice::enumeratePhysicalDevices(const vkInstance &instance)
{
	assert(instance.isValid());

	std::vector<vkPhysicalDevice> physicalDevices;
	std::vector<VkPhysicalDevice> physicalDeviceHandles;

	uint32_t count = 0U;
	rVerifyVk(vkEnumeratePhysicalDevices(*instance, &count, nullptr));
	assert(count > 0U);
	physicalDevices.resize(count);
	physicalDeviceHandles.resize(count);
	rVerifyVk(vkEnumeratePhysicalDevices(*instance, &count, physicalDeviceHandles.data()));
	for (uint32_t i = 0u; i < count; ++i)
	{
		physicalDevices[i].reset(physicalDeviceHandles[i]);
	}

	return physicalDevices;
}

uint32_t vkDevice::create(
	const std::vector<vkPhysicalDevice> &physicalDevices,
	uint32_t &graphicsQueueIndex,
	uint32_t &computeQueueIndex,
	uint32_t &transferQueueIndex)
{
	assert(!isValid() && physicalDevices.size() > 0u);

	uint32_t gpuIndex = UINT32_MAX;
	uint32_t discreteGpuIndex = UINT32_MAX;
	std::string gpuTypeName;

	uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
	uint32_t computeQueueFamilyIndex = UINT32_MAX;
	uint32_t transferQueueFamilyIndex = UINT32_MAX;
	uint32_t numQueuePriority = 0u;

	std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
	for (uint32_t i = 0u; i < physicalDevices.size(); ++i)
	{
		VkPhysicalDeviceProperties properties = {};
		vkGetPhysicalDeviceProperties(*physicalDevices[i], &properties);

		switch (properties.deviceType)
		{
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			gpuTypeName = enumToString(VK_PHYSICAL_DEVICE_TYPE_OTHER);
			break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			gpuTypeName = enumToString(VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
			break;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			gpuTypeName = enumToString(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
			discreteGpuIndex = i;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			gpuTypeName = enumToString(VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU);
			break;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			gpuTypeName = enumToString(VK_PHYSICAL_DEVICE_TYPE_CPU);
			break;
		default:
			gpuTypeName = "Unknown";
			break;
		}

		gpuIndex = i;

		uint32_t count = 0u;
		vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevices[i], &count, nullptr);
		assert(count > 0u);

		std::vector<VkQueueFamilyProperties> queueFamilyProperties(count);
		vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevices[i], &count, queueFamilyProperties.data());

		for (uint32_t j = 0U; j < queueFamilyProperties.size(); ++j)
		{
			if (((queueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT) &&
				graphicsQueueFamilyIndex == UINT32_MAX)
			{
				graphicsQueueFamilyIndex = j;
			}

			if (((queueFamilyProperties[j].queueFlags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT) &&
				computeQueueFamilyIndex == UINT32_MAX &&
				computeQueueFamilyIndex != j &&
				graphicsQueueFamilyIndex != j)
			{
				computeQueueFamilyIndex = j;
			}

			if (((queueFamilyProperties[j].queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT) &&
				transferQueueFamilyIndex == UINT32_MAX && 
				transferQueueFamilyIndex != j && 
				((queueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) &&
				((queueFamilyProperties[j].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
			{
				transferQueueFamilyIndex = j;
			}

			if (graphicsQueueFamilyIndex == UINT32_MAX &&
				computeQueueFamilyIndex == UINT32_MAX &&
				transferQueueFamilyIndex == UINT32_MAX)
			{
				continue;
			}

			numQueuePriority += queueFamilyProperties[j].queueCount;
			VkDeviceQueueCreateInfo deviceQueueCreateInfo
			{
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				nullptr, /// pNext must be NULL or a pointer to a valid instance of VkDeviceQueueGlobalPriorityCreateInfoEXT
				0u,      /// flags must be a valid combination of VkDeviceQueueCreateFlagBits values
				j,
				queueFamilyProperties[j].queueCount,
				nullptr
			};
			deviceQueueCreateInfos.emplace_back(deviceQueueCreateInfo);
			/// VkDeviceQueueCreateFlagBits::VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT specifies that the device queue is a protected-capable queue. 
			/// If the protected memory feature is not enabled, the VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT bit of flags must not be set.
		}
	}

	assert(gpuIndex == discreteGpuIndex);
	if (gpuIndex != discreteGpuIndex)
	{
		logger::instance().log(logger::eInfo, "Can't find discrete gpu, the valid gpu type is \"%s\"", gpuTypeName.c_str());
	}

	std::vector<float32_t> queuePriorities(numQueuePriority, 1.0f);

	for (auto &deviceQueueCreateInfo : deviceQueueCreateInfos)
	{
		deviceQueueCreateInfo.pQueuePriorities = queuePriorities.data();  /// Each element of pQueuePriorities must be between 0.0 and 1.0 inclusive
	}

	std::vector<const char8_t *> layers =
	{
		"VK_LAYER_LUNARG_standard_validation",
		"VK_LAYER_LUNARG_parameter_validation",
		"VK_LAYER_LUNARG_object_tracker",
		"VK_LAYER_LUNARG_core_validation"
	};

	std::vector<const char8_t *> extensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_EXT_DEBUG_MARKER_EXTENSION_NAME
	};

	uint32_t count = 0U;
	rVerifyVk(vkEnumerateDeviceLayerProperties(*physicalDevices[gpuIndex], &count, nullptr));
	std::vector<VkLayerProperties> supportedLayers(count);
	rVerifyVk(vkEnumerateDeviceLayerProperties(*physicalDevices[gpuIndex], &count, supportedLayers.data()));
	layers = getSupportedProperties<VkLayerProperties>(supportedLayers, layers);

	rVerifyVk(vkEnumerateDeviceExtensionProperties(*physicalDevices[gpuIndex], nullptr, &count, nullptr));
	std::vector<VkExtensionProperties> supportedExtensions(count);
	rVerifyVk(vkEnumerateDeviceExtensionProperties(*physicalDevices[gpuIndex], nullptr, &count, supportedExtensions.data()));
	extensions = getSupportedProperties<VkExtensionProperties>(supportedExtensions, extensions);

	vkGetPhysicalDeviceFeatures(*physicalDevices[gpuIndex], &m_DeviceFeatures);

	VkDeviceCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		nullptr,
		0u,
		(uint32_t)deviceQueueCreateInfos.size(),
		deviceQueueCreateInfos.data(),
		(uint32_t)layers.size(),
		layers.data(),
		(uint32_t)extensions.size(),
		extensions.data(),
		&m_DeviceFeatures
	};

	VkDevice handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateDevice(*physicalDevices[gpuIndex], &createInfo, vkMemoryAllocator, &handle));
	reset(handle);

#if defined(UsingVkLoader)
	vkLoader::initializeDeviceFunctionTable(*this);
#endif

	VkPhysicalDeviceProperties properties = {};
	vkGetPhysicalDeviceProperties(*physicalDevices[gpuIndex], &properties);
	m_Adapter.DeviceName = properties.deviceName;
	m_Adapter.DeviceID = properties.deviceID;
	m_Adapter.VendorID = properties.vendorID;
	logger::instance().log(logger::eInfo, "Created vulkan device on adapter: \"%s %s\", DeviceID = %d. VulkanAPI Version: %d.%d.%d",
		rAdapter::getVendorName(m_Adapter.VendorID).c_str(),
		m_Adapter.DeviceName.c_str(),
		m_Adapter.DeviceID,
		VK_VERSION_MAJOR(properties.apiVersion),
		VK_VERSION_MINOR(properties.apiVersion),
		VK_VERSION_PATCH(properties.apiVersion));

	m_DeviceLimits = properties.limits;
	assert(m_DeviceLimits.maxColorAttachments >= eRMax_RenderTargets);

	vkGetPhysicalDeviceMemoryProperties(*physicalDevices[gpuIndex], &m_DeviceMemoryProperties);

	graphicsQueueIndex = graphicsQueueFamilyIndex;
	computeQueueIndex = computeQueueFamilyIndex;
	transferQueueIndex = transferQueueFamilyIndex;

	m_CommandPool.create(*this, graphicsQueueFamilyIndex);

	m_PipelineCache.create(*this);

	m_GpuResourcePool = std::make_unique<vkGpuResourcePool>(*this);
	m_PipelinePool = std::make_unique<vkPipelinePool>(*this);

	return gpuIndex;
}

void vkDevice::waitIdle()
{
	/// To wait on the host for the completion of outstanding queue operations for all queues on a given logical device
	/// equivalent to calling vkQueueWaitIdle for all queues owned by device
	if (isValid())
	{
		rVerifyVk(vkDeviceWaitIdle(**this));
	}
}

uint32_t vkDevice::getMemoryTypeIndex(eRBufferUsage usage, uint32_t memoryTypeBits) const
{
	assert(isValid() && usage < eRBufferUsage_MaxEnum);

	VkMemoryPropertyFlags memoryPropertyFlags = 0u;
	switch (usage)
	{
	case eGpuReadWrite:
	case eGpuReadOnly:
		memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case eGpuReadCpuWrite:
		memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		break;
	case eGpuCopyToCpu:
		memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		break;
	default:
		assert(0);
		break;
	}

	for (uint32_t i = 0u; i < m_DeviceMemoryProperties.memoryTypeCount; ++i)
	{
		if (((memoryTypeBits >> i) & 1u) == 1u)
		{
			if ((m_DeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags)
			{
				return i;
			}
		}
	}

	assert(0);
	return UINT32_MAX;
}

void vkDevice::destroy()
{
	if (isValid())
	{
		rVerifyVk(vkDeviceWaitIdle(**this));

		m_GpuResourcePool->destoryAll();
		m_PipelinePool->destroyAll();

		m_CommandPool.destroy(*this);

		m_PipelineCache.destroy(*this);

		vkDestroyDevice(**this, vkMemoryAllocator);
		reset();
	}
}
