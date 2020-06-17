#include "Colorful/Vulkan/VulkanEngine.h"

void VulkanDevice::create(VkInstance instance)
{
	uint32_t count = 0U;
	GfxVerifyVk(vkEnumeratePhysicalDevices(instance, &count, nullptr));
	assert(count > 0U);

	std::vector<VkPhysicalDevice> physicalDevices(count);
	GfxVerifyVk(vkEnumeratePhysicalDevices(instance, &count, physicalDevices.data()));

	uint32_t gpuIndex = std::numeric_limits<uint32_t>().max();
	uint32_t discreteGpuIndex = std::numeric_limits<uint32_t>().max();
	std::string gpuTypeName;

	uint32_t graphicsQueueFamilyIndex = std::numeric_limits<uint32_t>().max();
	uint32_t computeQueueFamilyIndex = std::numeric_limits<uint32_t>().max();
	uint32_t transferQueueFamilyIndex = std::numeric_limits<uint32_t>().max();
	uint32_t numQueuePriority = 0u;

	std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
	for (uint32_t i = 0u; i < physicalDevices.size(); ++i)
	{
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);

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

		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &count, nullptr);
		assert(count > 0u);

		std::vector<VkQueueFamilyProperties> queueFamilyProperties(count);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &count, queueFamilyProperties.data());

		for (uint32_t j = 0U; j < queueFamilyProperties.size(); ++j)
		{
			if (((queueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT) &&
				graphicsQueueFamilyIndex == std::numeric_limits<uint32_t>().max())
			{
				graphicsQueueFamilyIndex = j;
			}

			if (((queueFamilyProperties[j].queueFlags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT) &&
				computeQueueFamilyIndex == std::numeric_limits<uint32_t>().max() &&
				computeQueueFamilyIndex != j &&
				graphicsQueueFamilyIndex != j)
			{
				computeQueueFamilyIndex = j;
			}

			if (((queueFamilyProperties[j].queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT) &&
				transferQueueFamilyIndex == std::numeric_limits<uint32_t>().max() &&
				transferQueueFamilyIndex != j &&
				((queueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) &&
				((queueFamilyProperties[j].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
			{
				transferQueueFamilyIndex = j;
			}

			if (graphicsQueueFamilyIndex == std::numeric_limits<uint32_t>().max() &&
				computeQueueFamilyIndex == std::numeric_limits<uint32_t>().max() &&
				transferQueueFamilyIndex == std::numeric_limits<uint32_t>().max())
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
		Logger::instance().log(Logger::eInfo, "Can't find discrete gpu, the valid gpu type is \"%s\"", gpuTypeName.c_str());
	}
	m_PhysicalDevice.Handle = physicalDevices[gpuIndex];

	std::vector<float32_t> queuePriorities(numQueuePriority, 1.0f);

	for (auto& deviceQueueCreateInfo : deviceQueueCreateInfos)
	{
		deviceQueueCreateInfo.pQueuePriorities = queuePriorities.data();  /// Each element of pQueuePriorities must be between 0.0 and 1.0 inclusive
	}

	std::vector<const char8_t*> layers =
	{
#if defined(_DEBUG)
		"VK_LAYER_LUNARG_standard_validation",
		"VK_LAYER_LUNARG_parameter_validation",
		"VK_LAYER_LUNARG_object_tracker",
		"VK_LAYER_LUNARG_core_validation"
#endif
	};

	std::vector<const char8_t*> extensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_EXT_DEBUG_MARKER_EXTENSION_NAME
	};

	GfxVerifyVk(vkEnumerateDeviceLayerProperties(m_PhysicalDevice.Handle, &count, nullptr));
	std::vector<VkLayerProperties> supportedLayers(count);
	GfxVerifyVk(vkEnumerateDeviceLayerProperties(m_PhysicalDevice.Handle, &count, supportedLayers.data()));
	layers = VulkanEngine::getSupportedProperties<VkLayerProperties>(supportedLayers, layers);

	GfxVerifyVk(vkEnumerateDeviceExtensionProperties(m_PhysicalDevice.Handle, nullptr, &count, nullptr));
	std::vector<VkExtensionProperties> supportedExtensions(count);
	GfxVerifyVk(vkEnumerateDeviceExtensionProperties(m_PhysicalDevice.Handle, nullptr, &count, supportedExtensions.data()));
	extensions = VulkanEngine::getSupportedProperties<VkExtensionProperties>(supportedExtensions, extensions);

	VkPhysicalDeviceFeatures deviceFeatures{};
	vkGetPhysicalDeviceFeatures(m_PhysicalDevice.Handle, &deviceFeatures);

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
		&deviceFeatures
	};

	GfxVerifyVk(vkCreateDevice(physicalDevices[gpuIndex], &createInfo, vkMemoryAllocator, &m_LogicalDevice.Handle));

#if defined(UsingVkLoader)
	VulkanLoader::initializeDeviceFunctionTable(m_LogicalDevice.Handle);
#endif

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(physicalDevices[gpuIndex], &properties);
	m_Adapter.DeviceName = properties.deviceName;
	m_Adapter.DeviceID = properties.deviceID;
	m_Adapter.VendorID = properties.vendorID;
	Logger::instance().log(Logger::eInfo, "Created Vulkan device on adapter: \"%s %s\", DeviceID = %d. VulkanAPI Version: %d.%d.%d",
		GfxAdapter::vendorName(m_Adapter.VendorID).c_str(),
		m_Adapter.DeviceName.c_str(),
		m_Adapter.DeviceID,
		VK_VERSION_MAJOR(properties.apiVersion),
		VK_VERSION_MINOR(properties.apiVersion),
		VK_VERSION_PATCH(properties.apiVersion));

	VulkanAsyncPool::initialize(m_LogicalDevice.Handle);
	VulkanQueueManager::initialize(
		m_LogicalDevice.Handle, 
		graphicsQueueFamilyIndex, 
		computeQueueFamilyIndex, 
		transferQueueFamilyIndex);
	VulkanBufferPool::initialize(m_LogicalDevice.Handle, m_PhysicalDevice.Handle);
	VulkanPipelinePool::initialize(m_LogicalDevice.Handle);
	VulkanMainDescriptorPool::initialize(m_LogicalDevice.Handle, m_PhysicalDevice.Handle);
	VulkanRenderPassManager::initialize(m_LogicalDevice.Handle);
}

void VulkanDevice::destroy()
{
	waitIdle();

	VulkanAsyncPool::instance()->finalize();
	VulkanQueueManager::instance()->finalize();
	VulkanBufferPool::instance()->finalize();
	VulkanPipelinePool::instance()->finalize();
	VulkanMainDescriptorPool::instance()->finalize();
	VulkanRenderPassManager::instance()->finalize();

	vkDestroyDevice(m_LogicalDevice.Handle, vkMemoryAllocator);
	m_LogicalDevice.Handle = VK_NULL_HANDLE;
	m_PhysicalDevice.Handle = VK_NULL_HANDLE;
}

void VulkanDevice::waitIdle()
{
	if (m_LogicalDevice.isValid())
	{
		GfxVerifyVk(vkDeviceWaitIdle(m_LogicalDevice.Handle));
	}
}
