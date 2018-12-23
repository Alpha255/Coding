#include "VulkanObject.h"
#include "VulkanPool.h"
#include "VulkanEngine.h"

void VulkanInstance::InitLayerProperties()
{
	uint32_t count = 0U;
	VkResult res = VK_NOT_READY;

	do
	{
		res = vkEnumerateInstanceLayerProperties(&count, nullptr);

		if (VK_SUCCESS == res || 0U == count)
		{
			return;
		}

		m_LayerProperties.resize(count);
		res = vkEnumerateInstanceLayerProperties(&count, m_LayerProperties.data());
	} while (VK_INCOMPLETE == res);

	for (auto &layerProperty : m_LayerProperties)
	{
		do
		{
			res = vkEnumerateInstanceExtensionProperties(layerProperty.layerName, &count, nullptr);
			
			if (VK_SUCCESS == res || 0U == count)
			{
				return;
			}

			m_ExtensionProperties.resize(count);
			res = vkEnumerateInstanceExtensionProperties(layerProperty.layerName, &count, m_ExtensionProperties.data());
		} while (VK_INCOMPLETE == res);
	}
}

void VulkanInstance::Create(const char *pInstName)
{
	InitLayerProperties();

	VkApplicationInfo appInfo
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		pInstName,
		1U,
		pInstName,
		1U,
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
		_countof(pExtensionNames),
		pExtensionNames
	};

	VKCheck(vkCreateInstance(&instInfo, nullptr, &m_Handle));
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

	VKCheck(vkAllocateCommandBuffers(VulkanEngine::Instance().GetDevice().Get(), &allocInfo, &m_Handle));
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

	VKCheck(vkCreateRenderPass(VulkanEngine::Instance().GetDevice().Get(), &renderPassCreateInfo, nullptr, &m_Handle));
}

void VulkanSemaphore::Create()
{
	VkSemaphoreCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		nullptr,
		0U
	};

	VKCheck(vkCreateSemaphore(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));
}

void VulkanFence::Create()
{
	VkFenceCreateInfo createInfo 
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		0U
	};

	VKCheck(vkCreateFence(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));
}