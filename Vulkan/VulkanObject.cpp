#include "VulkanObject.h"
#include "VulkanPool.h"
#include "VulkanEngine.h"
#include "VulkanUtil.h"

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
	System::Log("[Vulkan] ObjectType: %i\nMessage: %s", objectType, pMessage);

	return VK_FALSE;
}

void VulkanInstance::Create(const char *pApplicationName, const char *pEngineName)
{
	assert(!IsValid() && pApplicationName && pEngineName);

	uint32_t count = 0U;
	VKCheck(vkEnumerateInstanceLayerProperties(&count, nullptr));
	m_LayerProperties.resize(count);
	VKCheck(vkEnumerateInstanceLayerProperties(&count, m_LayerProperties.data()));

	VKCheck(vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));
	m_ExtensionProperties.resize(count);
	VKCheck(vkEnumerateInstanceExtensionProperties(nullptr, &count, m_ExtensionProperties.data()));

	std::vector<const char *> enabledLayers = VulkanUtil::FilterLayers(m_LayerProperties);
	std::vector<const char *> enabledExtensions = VulkanUtil::FilterExtensions(m_ExtensionProperties);

	VkApplicationInfo appInfo
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		pApplicationName,
		1U,
		pEngineName,
		1U,
		VK_API_VERSION_1_0
	};

	VkInstanceCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,
		0U,
		&appInfo,
		(uint32_t)enabledLayers.size(),
		enabledLayers.data(),
		(uint32_t)enabledExtensions.size(),
		enabledExtensions.data()
	};

	VKCheck(vkCreateInstance(&createInfo, nullptr, &m_Handle));

	auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Handle, "vkCreateDebugReportCallbackEXT");
	assert(vkCreateDebugReportCallbackEXT != NULL);

	VkDebugReportCallbackCreateInfoEXT debugReportInfo = {};
	debugReportInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	debugReportInfo.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;
	debugReportInfo.pfnCallback = VulkanDebugReportFunc;
	debugReportInfo.pUserData = NULL;
	VKCheck(vkCreateDebugReportCallbackEXT(m_Handle, &debugReportInfo, nullptr, &m_DebugReportCallback));
}

void VulkanPhysicalDevice::VerifyDeviceFeatures(VkPhysicalDeviceFeatures &enabledFeatures)
{
	const VkBool32 *pStart = &m_Features.robustBufferAccess;
	const VkBool32 *pEnd = &m_Features.inheritedQueries;

	const VkBool32 *pNeedVerify = &enabledFeatures.robustBufferAccess;
	do
	{
		assert((VK_FALSE == *pNeedVerify) || (VK_TRUE == *pNeedVerify && VK_TRUE == *pStart));
		++pNeedVerify;
		++pStart;
	} while (pStart != pEnd);
}

void VulkanPhysicalDevice::Create()
{
	uint32_t count = 0U;
	VKCheck(vkEnumeratePhysicalDevices(VulkanEngine::Instance().GetInstance(), &count, nullptr));

	assert(count > 0U);

	m_PhysicalDevices.resize(count);
	VKCheck(vkEnumeratePhysicalDevices(VulkanEngine::Instance().GetInstance(), &count, m_PhysicalDevices.data()));

	for each (auto &physicalDevice in m_PhysicalDevices)
	{
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilyProperties(count);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queueFamilyProperties.data());

		for (uint32_t i = 0U; i < queueFamilyProperties.size(); ++i)
		{
			if (queueFamilyProperties[i].queueFlags & (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT))
			{
				m_QueueFamilyIndex = i;
				break;
			}
		}
	}

	assert(m_QueueFamilyIndex != UINT32_MAX);

	Reset(m_PhysicalDevices[m_QueueFamilyIndex]);

	VKCheck(vkEnumerateDeviceLayerProperties(m_Handle, &count, nullptr));
	m_LayerProperties.resize(count);
	VKCheck(vkEnumerateDeviceLayerProperties(m_Handle, &count, m_LayerProperties.data()));

	VKCheck(vkEnumerateDeviceExtensionProperties(m_Handle, nullptr, &count, nullptr));
	m_ExtensionProperties.resize(count);
	VKCheck(vkEnumerateDeviceExtensionProperties(m_Handle, nullptr, &count, m_ExtensionProperties.data()));

	vkGetPhysicalDeviceFeatures(m_Handle, &m_Features);
	VkPhysicalDeviceFeatures enabledFeatures = {};
	enabledFeatures.shaderClipDistance = VK_TRUE;
	enabledFeatures.shaderCullDistance = VK_TRUE;
	VerifyDeviceFeatures(enabledFeatures);

	vkGetPhysicalDeviceProperties(m_Handle, &m_Properties);

	vkGetPhysicalDeviceMemoryProperties(m_Handle, &m_MemoryProperties);
}

uint32_t VulkanPhysicalDevice::GetOptimalSurfaceFormat(uint32_t flags, bool bDepthSurface) const
{
	assert(IsValid());

	VkFormat result = VK_FORMAT_UNDEFINED;

	static const VkFormat colorFormats[] =
	{
		VK_FORMAT_B8G8R8A8_UNORM,
	};

	static const VkFormat depthFormats[] =
	{
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
	};

	const VkFormat *pTargetFormats = bDepthSurface ? depthFormats : colorFormats;
	uint32_t count = bDepthSurface ? _countof(depthFormats) : _countof(colorFormats);
	for (uint32_t i = 0; i < count; ++i)
	{
		VkFormatProperties formatProperties = {};
		vkGetPhysicalDeviceFormatProperties(m_Handle, *pTargetFormats, &formatProperties);
		if (formatProperties.optimalTilingFeatures & flags)
		{
			result = *pTargetFormats;
			break;
		}
		++pTargetFormats;
	}


	return result;
}

void VulkanDevice::Create()
{
	assert(!IsValid());

	m_PhysicalDevice.Create();

	std::vector<const char *> enabledLayers = VulkanUtil::FilterLayers(m_PhysicalDevice.GetLayerProperties());
	std::vector<const char *> enabledExtensions = VulkanUtil::FilterExtensions(m_PhysicalDevice.GetExtensionProperties());

	float queuePriorities[] = { 1.0f };
	VkDeviceQueueCreateInfo deviceQueueCreateInfo
	{
		VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		nullptr,
		0U,
		m_PhysicalDevice.GetQueueFamilyIndex(),
		1U,
		queuePriorities
	};

	VkDeviceCreateInfo deviceCreateInfo
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		nullptr,
		0U,
		1U,
		&deviceQueueCreateInfo,
		(uint32_t)enabledLayers.size(),
		enabledLayers.data(),
		(uint32_t)enabledExtensions.size(),
		enabledExtensions.data(),
		&m_PhysicalDevice.GetDeviceFeatures()
	};

	VKCheck(vkCreateDevice(m_PhysicalDevice.Get(), &deviceCreateInfo, nullptr, &m_Handle));

	vkGetDeviceQueue(m_Handle, m_PhysicalDevice.GetQueueFamilyIndex(), 0U, &m_DeviceQueue);
}

void VulkanSemaphore::Create()
{
	assert(!IsValid());

	VkSemaphoreCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		nullptr,
		0U  /// Flags is reserved for future use
	};

	VKCheck(vkCreateSemaphore(VulkanEngine::Instance().GetDevice(), &createInfo, nullptr, &m_Handle));
}

void VulkanSemaphore::Destory()
{
	assert(IsValid());

	vkDestroySemaphore(VulkanEngine::Instance().GetDevice(), m_Handle, nullptr);

	Reset();
}

void VulkanFence::Create(eFenceState state)
{
	assert(!IsValid());

	VkFenceCreateInfo createInfo 
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		eUnSignaled == state ? 0U : VK_FENCE_CREATE_SIGNALED_BIT
	};

	VKCheck(vkCreateFence(VulkanEngine::Instance().GetDevice(), &createInfo, nullptr, &m_Handle));
}

void VulkanFence::Destory()
{
	assert(IsValid());

	vkDestroyFence(VulkanEngine::Instance().GetDevice(), m_Handle, nullptr);

	Reset();
}

void VulkanDescriptorPool::Create()
{
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};
	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000 * _countof(pool_sizes);
	pool_info.poolSizeCount = _countof(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;
	VKCheck(vkCreateDescriptorPool(VulkanEngine::Instance().GetDevice(), &pool_info, nullptr, &m_Handle));
}

void VulkanDescriptorPool::Destory()
{

}

void VulkanDescriptorSetLayout::Create(VulkanSamplerState sampler)
{
	VkDescriptorSetLayoutBinding binding[1] = {};
	binding[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	binding[0].descriptorCount = 1;
	binding[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	binding[0].pImmutableSamplers = &sampler;
	VkDescriptorSetLayoutCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info.bindingCount = 1;
	info.pBindings = binding;
	VKCheck(vkCreateDescriptorSetLayout(VulkanEngine::Instance().GetDevice(), &info, nullptr, &m_Handle));
}

void VulkanDescriptorSetLayout::Destory()
{

}

void VulkanDescriptorSet::Create(VulkanDescriptorPool pool, VulkanDescriptorSetLayout layout)
{
	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = pool.Get();
	alloc_info.descriptorSetCount = 1;
	alloc_info.pSetLayouts = &layout;
	VKCheck(vkAllocateDescriptorSets(VulkanEngine::Instance().GetDevice(), &alloc_info, &m_Handle));
}