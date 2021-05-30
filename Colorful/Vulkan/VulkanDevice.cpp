#include "Colorful/Vulkan/VulkanDevice.h"
#include "Colorful/Vulkan/VulkanQueue.h"

NAMESPACE_START(Gfx)

std::shared_ptr<VulkanDevice> VkResource::s_Device = nullptr;

VulkanDevice::VulkanDevice(VkInstance instance)
	: m_Instance(instance)
{
	assert(instance);

	uint32_t count = 0u;
	VERIFY_VK(vkEnumeratePhysicalDevices(instance, &count, nullptr));
	assert(count > 0u);

	std::vector<VkPhysicalDevice> physicalDevices(count);
	VERIFY_VK(vkEnumeratePhysicalDevices(instance, &count, physicalDevices.data()));

	uint32_t gpuIndex = std::numeric_limits<uint32_t>().max();
	uint32_t discreteGpuIndex = std::numeric_limits<uint32_t>().max();
	VkPhysicalDeviceType gpuType = VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM;

	uint32_t graphicsQueueFamilyIndex = ~0u;
	uint32_t computeQueueFamilyIndex = ~0u;
	uint32_t transferQueueFamilyIndex = ~0u;
	uint32_t numQueuePriority = 0u;

	auto getGpuTypeName = [](VkPhysicalDeviceType type)-> const char8_t* const
	{
		switch (type)
		{
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:          return "Other";
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "Integrated Gpu";
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   return "Discrete Gpu";
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:    return "Virtual Gpu";
		case VK_PHYSICAL_DEVICE_TYPE_CPU:            return "Cpu";
		}

		return "Unknown Device";
	};

	std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
	for (uint32_t i = 0u; i < physicalDevices.size(); ++i)
	{
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);
		gpuType = properties.deviceType;

		gpuIndex = i;
		discreteGpuIndex = gpuType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? i : discreteGpuIndex;

		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &count, nullptr);
		assert(count > 0u);

		std::vector<VkQueueFamilyProperties> queueFamilyProperties(count);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &count, queueFamilyProperties.data());

		for (uint32_t j = 0U; j < queueFamilyProperties.size(); ++j)
		{
			if (((queueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT) &&
				graphicsQueueFamilyIndex == ~0u)
			{
				graphicsQueueFamilyIndex = j;
			}

			if (((queueFamilyProperties[j].queueFlags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT) &&
				computeQueueFamilyIndex == ~0u &&
				computeQueueFamilyIndex != j &&
				graphicsQueueFamilyIndex != j)
			{
				computeQueueFamilyIndex = j;
			}

			if (((queueFamilyProperties[j].queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT) &&
				transferQueueFamilyIndex == ~0u &&
				transferQueueFamilyIndex != j &&
				((queueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) &&
				((queueFamilyProperties[j].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
			{
				transferQueueFamilyIndex = j;
			}

			if (graphicsQueueFamilyIndex == ~0u &&
				computeQueueFamilyIndex == ~0u &&
				transferQueueFamilyIndex == ~0u)
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

		if (gpuType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) /// preference for discrete gpu
		{
			break;
		}
	}

	assert(gpuIndex == discreteGpuIndex);
	if (gpuIndex != discreteGpuIndex)
	{
		LOG_INFO("Can't find discrete gpu, the valid gpu type is \"%s\"", getGpuTypeName(gpuType));
	}
	
	m_PhysicalDevice = physicalDevices[gpuIndex];

	std::vector<float32_t> queuePriorities(numQueuePriority, 1.0f);

	for (auto& deviceQueueCreateInfo : deviceQueueCreateInfos)
	{
		deviceQueueCreateInfo.pQueuePriorities = queuePriorities.data();  /// Each element of pQueuePriorities must be between 0.0 and 1.0 inclusive
	}

	std::vector<const char8_t*> expectLayers
	{
#if defined(_DEBUG)
		"VK_LAYER_LUNARG_standard_validation",
		"VK_LAYER_LUNARG_parameter_validation",
		"VK_LAYER_LUNARG_object_tracker",
		"VK_LAYER_LUNARG_core_validation"
#endif
	};

	std::vector<const char8_t*> expectExtensions
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_EXT_DEBUG_MARKER_EXTENSION_NAME
	};

	VERIFY_VK(vkEnumerateDeviceLayerProperties(m_PhysicalDevice, &count, nullptr));
	std::vector<VkLayerProperties> supportedLayers(count);
	VERIFY_VK(vkEnumerateDeviceLayerProperties(m_PhysicalDevice, &count, supportedLayers.data()));
	auto layers = getSupportedProperties<VkLayerProperties>(supportedLayers, expectLayers);

	VERIFY_VK(vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &count, nullptr));
	std::vector<VkExtensionProperties> supportedExtensions(count);
	VERIFY_VK(vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &count, supportedExtensions.data()));
	auto extensions = getSupportedProperties<VkExtensionProperties>(supportedExtensions, expectExtensions);

	for (auto extension : extensions)
	{
		if (strcmp(extension, VK_EXT_DEBUG_MARKER_EXTENSION_NAME) == 0)
		{
			m_EnableProfiler = true;
			break;
		}
	}

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &deviceFeatures);

	VkDeviceCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		nullptr,
		0u,
		static_cast<uint32_t>(deviceQueueCreateInfos.size()),
		deviceQueueCreateInfos.data(),
		static_cast<uint32_t>(layers.size()),
		layers.data(),
		static_cast<uint32_t>(extensions.size()),
		extensions.data(),
		&deviceFeatures
	};

	VERIFY_VK(vkCreateDevice(m_PhysicalDevice, &createInfo, VK_MEMORY_ALLOCATOR, reference()));

	vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_Features);

	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(m_PhysicalDevice, &properties);

	uint32_t VendorID = properties.vendorID;
	uint32_t DeviceID = properties.deviceID;
	std::string DeviceName = properties.deviceName;

	m_PhysicalLimits = std::move(properties.limits);
	m_PhysicalDeviceSparseProperties = std::move(properties.sparseProperties);

	LOG_INFO("Create Vulkan device on adapter: \"%s %s\", DeviceID = %d. VulkanAPI Version: %d.%d.%d",
		getVendorName(VendorID),
		DeviceName.c_str(),
		DeviceID,
		VK_VERSION_MAJOR(properties.apiVersion),
		VK_VERSION_MINOR(properties.apiVersion),
		VK_VERSION_PATCH(properties.apiVersion));

	m_QueueFamilyIndices[EQueueFamilyType::Graphics] = graphicsQueueFamilyIndex;
	m_QueueFamilyIndices[EQueueFamilyType::Transfer] = transferQueueFamilyIndex;
	m_QueueFamilyIndices[EQueueFamilyType::Compute] = computeQueueFamilyIndex;
}

CommandBufferPtr VulkanDevice::getActiveCommandBuffer()
{
	return VkQueues::instance().getActiveGraphicsCommand();
}

NAMESPACE_END(Gfx)
