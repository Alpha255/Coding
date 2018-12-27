#include "VulkanObject.h"
#include "VulkanPool.h"
#include "VulkanEngine.h"
#include "VulkanUtil.h"

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
}

void VulkanDevice::GetPhysicalDevice()
{
	uint32_t gpuCount = 0U;
	VKCheck(vkEnumeratePhysicalDevices(VulkanEngine::Instance().GetInstance(), &gpuCount, nullptr));

	assert(gpuCount > 0U);

	std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
	VKCheck(vkEnumeratePhysicalDevices(VulkanEngine::Instance().GetInstance(), &gpuCount, physicalDevices.data()));

	for each (auto physicalDevice in physicalDevices)
	{
		m_PhysicalDevices.emplace_back(physicalDevice);
	}
}

void VulkanDevice::VerifyPhysicalFeatures(VkPhysicalDeviceFeatures &enableFeatures)
{
	const VkBool32 *pStart = &m_EnabledFeatures.robustBufferAccess;
	const VkBool32 *pEnd = &m_EnabledFeatures.inheritedQueries;

	const VkBool32 *pNeedVerify = &enableFeatures.robustBufferAccess;
	do
	{
		assert((VK_FALSE == *pNeedVerify) || (VK_TRUE == *pNeedVerify && VK_TRUE == *pStart));
		++pNeedVerify;
		++pStart;
	} while (pStart != pEnd);
}

uint32_t VulkanDevice::GetOptimalSurfaceFormat(uint32_t flags, bool bDepthSurface) const
{
	assert(m_PhysicalDevices[m_QueueFamilyIndex].IsValid());

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
		vkGetPhysicalDeviceFormatProperties(m_PhysicalDevices[m_QueueFamilyIndex].Get(), *pTargetFormats, &formatProperties);
		if (formatProperties.optimalTilingFeatures & flags)
		{
			result = *pTargetFormats;
			break;
		}
		++pTargetFormats;
	}


	return result;
}

void VulkanDevice::CreateLogicalDevice()
{
	assert(!m_LogicalDevice.IsValid() && m_PhysicalDevices.size() > 0U);

	VkPhysicalDevice physicalDeviceInUsing = VK_NULL_HANDLE;
	for each (auto &physicalDevice in m_PhysicalDevices)
	{
		uint32_t count = 0U;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice.Get(), &count, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilyProperties(count);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice.Get(), &count, queueFamilyProperties.data());

		for (uint32_t i = 0U; i < queueFamilyProperties.size(); ++i)
		{
			if (queueFamilyProperties[i].queueFlags & (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT))
			{
				m_QueueFamilyIndex = i;
				break;
			}
		}

		if (m_QueueFamilyIndex != UINT_MAX)
		{
			physicalDeviceInUsing = m_PhysicalDevices[m_QueueFamilyIndex].Get();
			assert(physicalDeviceInUsing != VK_NULL_HANDLE);
			break;
		}
	}

	uint32_t count = 0U;
	VKCheck(vkEnumerateDeviceLayerProperties(physicalDeviceInUsing, &count, nullptr));
	m_LayerProperties.resize(count);
	VKCheck(vkEnumerateDeviceLayerProperties(physicalDeviceInUsing, &count, m_LayerProperties.data()));

	VKCheck(vkEnumerateDeviceExtensionProperties(physicalDeviceInUsing, nullptr, &count, nullptr));
	m_ExtensionProperties.resize(count);
	VKCheck(vkEnumerateDeviceExtensionProperties(physicalDeviceInUsing, nullptr, &count, m_ExtensionProperties.data()));

	std::vector<const char *> enabledLayers = VulkanUtil::FilterLayers(m_LayerProperties);
	std::vector<const char *> enabledExtensions = VulkanUtil::FilterExtensions(m_ExtensionProperties);

	VkPhysicalDeviceFeatures enableFeatures = {};
	enableFeatures.shaderClipDistance = VK_TRUE;
	enableFeatures.shaderCullDistance = VK_TRUE;

	vkGetPhysicalDeviceFeatures(physicalDeviceInUsing, &m_EnabledFeatures);

	VerifyPhysicalFeatures(enableFeatures);

	float queuePriorities[] = { 1.0f };
	VkDeviceQueueCreateInfo deviceQueueCreateInfo
	{
		VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		nullptr,
		0U,
		m_QueueFamilyIndex,
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
		&enableFeatures
	};

	VKCheck(vkCreateDevice(physicalDeviceInUsing, &deviceCreateInfo, nullptr, &m_LogicalDevice));

	vkGetDeviceQueue(m_LogicalDevice.Get(), m_QueueFamilyIndex, 0U, &m_DeviceQueue);

	vkGetPhysicalDeviceProperties(physicalDeviceInUsing, &m_PhysicalProperties);

	vkGetPhysicalDeviceMemoryProperties(physicalDeviceInUsing, &m_PhysicalMemoryProperties);
}

void VulkanRenderPass::Create(bool depth, uint32_t colorFormat, uint32_t depthFormat, bool clear, uint32_t imageLayout)
{
	VkAttachmentDescription layoutColor
	{
		0U,
		(VkFormat)colorFormat,
		VK_SAMPLE_COUNT_1_BIT,
		clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		(VkImageLayout)imageLayout
	};
	VkAttachmentReference refColor
	{
		0U,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkAttachmentDescription layoutDepth
	{
		0U,
		(VkFormat)depthFormat,
		VK_SAMPLE_COUNT_1_BIT,
		clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};
	VkAttachmentReference refDepth
	{
		1U,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	VkSubpassDescription subpassDes
	{
		0U,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		0U,
		nullptr,
		1U,
		&refColor,
		nullptr,
		depth ? &refDepth : nullptr,
		0U,
		nullptr
	};

	VkAttachmentDescription attachmentDesc[] = { layoutColor, layoutDepth };
	VkRenderPassCreateInfo renderPassCreateInfo
	{
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		nullptr,
		0U,
		depth ? 2U : 1U,
		attachmentDesc,
		1U,
		&subpassDes,
		0U,
		nullptr
	};

	VKCheck(vkCreateRenderPass(VulkanEngine::Instance().GetDevice(), &renderPassCreateInfo, nullptr, &m_Handle));
}

void VulkanSemaphore::Create()
{
	VkSemaphoreCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		nullptr,
		0U
	};

	VKCheck(vkCreateSemaphore(VulkanEngine::Instance().GetDevice(), &createInfo, nullptr, &m_Handle));
}

void VulkanFence::Create()
{
	VkFenceCreateInfo createInfo 
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		0U
	};

	VKCheck(vkCreateFence(VulkanEngine::Instance().GetDevice(), &createInfo, nullptr, &m_Handle));
}