#include "VulkanObject.h"
#include "VulkanPool.h"
#include "VulkanEngine.h"

void VulkanInstance::Create(const char *pApplicationName, const char *pEngineName)
{
	assert(pApplicationName && pEngineName);

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

	uint32_t count = 0U;
	VKCheck(vkEnumerateInstanceLayerProperties(&count, nullptr));
	m_LayerProperties.resize(count);
	VKCheck(vkEnumerateInstanceLayerProperties(&count, m_LayerProperties.data()));

	VKCheck(vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));
	m_ExtensionProperties.resize(count);
	VKCheck(vkEnumerateInstanceExtensionProperties(nullptr, &count, m_ExtensionProperties.data()));

	for each (auto layer in m_LayerProperties)
	{
		m_SupportedLayers.emplace_back(layer.layerName);
	}

	for each (auto extension in m_ExtensionProperties)
	{
		m_SupportedExtension.emplace_back(extension.extensionName);
	}

	std::vector<std::string> layers;
#if defined(_DEBUG)
	/// https://vulkan.lunarg.com/doc/view/1.0.13.0/windows/layers.html
	///layers.push_back("VK_LAYER_LUNARG_api_dump");
	///layers.push_back("VK_LAYER_LUNARG_standard_validation");
	///layers.push_back("VK_LAYER_GOOGLE_threading");
	///layers.push_back("VK_LAYER_GOOGLE_unique_objects");
	layers.emplace_back("VK_LAYER_LUNARG_parameter_validation");
	layers.emplace_back("VK_LAYER_LUNARG_object_tracker");
	layers.emplace_back("VK_LAYER_LUNARG_image");
	layers.emplace_back("VK_LAYER_LUNARG_device_limits");
	layers.emplace_back("VK_LAYER_LUNARG_core_validation");
	layers.emplace_back("VK_LAYER_LUNARG_swapchain");
#endif

	std::vector<const char *> enabledLayers;
	for each (auto layer in layers)
	{
		for each (auto supportedLayer in m_SupportedLayers)
		{
			if (layer == supportedLayer)
			{
				enabledLayers.emplace_back(supportedLayer.c_str());
			}
		}
	}

	/// https://vulkan.lunarg.com/doc/view/1.0.39.1/windows/LoaderAndLayerInterface.html
	std::vector<std::string> extensions;
	extensions.push_back("VK_KHR_xcb_surface");  /// Linux
	extensions.push_back("VK_KHR_xlib_surface"); /// Linux
	extensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);;
	extensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
	extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
#if VK_EXT_debug_report
	extensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif
#if VK_EXT_debug_marker
	extensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

	std::vector<const char *> enabledExtensions;
	for each (auto extension in extensions)
	{
		for each (auto supportedExtension in m_SupportedExtension)
		{
			if (extension == supportedExtension)
			{
				enabledExtensions.emplace_back(supportedExtension.c_str());
			}
		}
	}

	VkInstanceCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,
		0U,
		&appInfo,
		enabledLayers.size(),
		enabledLayers.data(),
		enabledExtensions.size(),
		enabledExtensions.data()
	};

	VKCheck(vkCreateInstance(&createInfo, nullptr, &m_Handle));
}

void VulkanPhysicalDevice::InitExtensionProperties()
{
	const std::vector<VkLayerProperties> layerProperties = VulkanEngine::Instance().GetInstance().GetLayerProperties();

	VkResult res = VK_NOT_READY;
	uint32_t count = 0U;
	for (auto layerProperty : layerProperties)
	{
		do
		{
			res = vkEnumerateDeviceExtensionProperties(m_Handle, layerProperty.layerName, &count, nullptr);

			if (VK_SUCCESS == res || 0U == count)
			{
				return;
			}

			m_ExtensionProperties.resize(count);
			res = vkEnumerateDeviceExtensionProperties(m_Handle, layerProperty.layerName, &count, m_ExtensionProperties.data());
		} while (VK_INCOMPLETE == res);
	}
}

void VulkanPhysicalDevice::Create()
{
	uint32_t count = 0U;
	VKCheck(vkEnumeratePhysicalDevices(VulkanEngine::Instance().GetInstance().Get(), &count, nullptr));
	assert(count == 1U);

	VKCheck(vkEnumeratePhysicalDevices(VulkanEngine::Instance().GetInstance().Get(), &count, &m_Handle));
	assert(count == 1U);

	vkGetPhysicalDeviceQueueFamilyProperties(m_Handle, &count, nullptr);
	assert(count >= 1U);

	m_QueueFamilyProperties.resize(count);
	vkGetPhysicalDeviceQueueFamilyProperties(m_Handle, &count, m_QueueFamilyProperties.data());
	assert(count >= 1U);

	vkGetPhysicalDeviceMemoryProperties(m_Handle, &m_DeviceMemoryProperties);

	vkGetPhysicalDeviceProperties(m_Handle, &m_DeviceProperties);

	InitExtensionProperties();
}

void VulkanDevice::Create()
{
	const float queuePriorities[] = { 1.0 };
	VkDeviceQueueCreateInfo deviceQueueCreateInfo
	{
		VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		nullptr,
		0U,
		VulkanEngine::Instance().GetSwapchain().GetGraphicsQueueFamilyIndex(),
		1U,
		queuePriorities
	};

	VkPhysicalDeviceFeatures features = {};
	std::vector<const char *>extensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	VkDeviceCreateInfo deviceCreateInfo
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		nullptr,
		0U,
		1U,
		&deviceQueueCreateInfo,
		0U,
		nullptr,
		extensionNames.size(),
		extensionNames.data(),
		&features
	};

	VKCheck(vkCreateDevice(VulkanEngine::Instance().GetPhysicalDevice().Get(), &deviceCreateInfo, nullptr, &m_Handle));

	vkGetDeviceQueue(m_Handle, VulkanEngine::Instance().GetSwapchain().GetGraphicsQueueFamilyIndex(), VulkanEngine::Instance().GetSwapchain().GetPresentQueueFamilyIndex(), &m_Queue);
	assert(m_Queue);
}

void VulkanCommandBuffer::Create(const VulkanCommandPool &pool, uint32_t level, uint32_t count)
{
	VkCommandBufferAllocateInfo allocInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		pool.Get(),
		(VkCommandBufferLevel)level,
		count
	};

	VKCheck(vkAllocateCommandBuffers(VulkanEngine::Instance().GetDevice(), &allocInfo, &m_Handle));
}

void VulkanCommandBuffer::Begin(uint32_t flags)
{
	VkCommandBufferBeginInfo beginInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		nullptr,
		(VkCommandBufferUsageFlags)flags,
		nullptr
	};

	VKCheck(vkBeginCommandBuffer(m_Handle, &beginInfo));
}

void VulkanCommandBuffer::End()
{
	VKCheck(vkEndCommandBuffer(m_Handle));
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