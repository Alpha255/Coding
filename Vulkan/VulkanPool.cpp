#include "VulkanPool.h"
#include "VulkanEngine.h"

void VulkanCommandPool::Create(ePoolType type)
{
	assert(!IsValid());

	uint32_t flags = VK_COMMAND_POOL_CREATE_FLAG_BITS_MAX_ENUM;
	switch (type)
	{
	case eTemp: flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;  break;
	case eGeneral: flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;  break;
	case eProtected: flags = VK_COMMAND_POOL_CREATE_PROTECTED_BIT; break;
	case eGeneralTemp: flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; break;
	default:
		assert(0);
		break;
	}
	VkCommandPoolCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		flags,
		VulkanEngine::Instance().GetPhysicalDevice().GetQueueFamilyIndex()
	};

	VKCheck(vkCreateCommandPool(VulkanEngine::Instance().GetDevice(), &createInfo, nullptr, &m_Handle));
}

void VulkanCommandPool::Destory()
{
	assert(IsValid());

	/// When a pool is destroyed, all command buffers allocated from the pool are freed
	vkDestroyCommandPool(VulkanEngine::Instance().GetDevice(), m_Handle, nullptr);

	Reset();
}

VulkanCommandBuffer VulkanCommandPool::Alloc(eBufferType type)
{
	assert(IsValid());

	VkCommandBufferAllocateInfo allocInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		m_Handle,
		(VkCommandBufferLevel)type,
		1U
	};

	VulkanCommandBuffer newCmdBuffer;
	VKCheck(vkAllocateCommandBuffers(VulkanEngine::Instance().GetDevice(), &allocInfo, &newCmdBuffer));

	return newCmdBuffer;
}

VulkanCommandBufferList VulkanCommandPool::Alloc(eBufferType type, uint32_t count)
{
	assert(IsValid());

	VkCommandBufferAllocateInfo allocInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		m_Handle,
		(VkCommandBufferLevel)type,
		count
	};

	std::vector<VkCommandBuffer> bufferList(count);
	VKCheck(vkAllocateCommandBuffers(VulkanEngine::Instance().GetDevice(), &allocInfo, bufferList.data()));

	VulkanCommandBufferList newCmdBufferList(bufferList);

	return newCmdBufferList;
}

void VulkanCommandPool::Free(VulkanCommandBuffer &cmdBuffer)
{
	assert(IsValid());

	vkFreeCommandBuffers(VulkanEngine::Instance().GetDevice(), m_Handle, 1U, &cmdBuffer);

	cmdBuffer.Reset();
}

void VulkanCommandPool::Free(VulkanCommandBufferList &cmdBufferList)
{
	assert(IsValid());

	vkFreeCommandBuffers(VulkanEngine::Instance().GetDevice(), m_Handle, cmdBufferList.GetCount(), &cmdBufferList);

	cmdBufferList.Reset();
}

void VulkanDescriptorPool::Create()
{
	assert(!IsValid());

	auto deviceLimits = VulkanEngine::Instance().GetPhysicalDevice().GetDeviceProperties().limits;

	VkDescriptorPoolSize descriptorPoolSize[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER,                deviceLimits.maxPerStageDescriptorSamplers },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, deviceLimits.maxPerStageDescriptorSamplers },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          deviceLimits.maxPerStageDescriptorSampledImages },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          deviceLimits.maxPerStageDescriptorStorageImages },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   deviceLimits.maxPerStageDescriptorSampledImages },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   deviceLimits.maxPerStageDescriptorStorageImages },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         deviceLimits.maxPerStageDescriptorUniformBuffers },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         deviceLimits.maxPerStageDescriptorStorageBuffers },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, deviceLimits.maxPerStageDescriptorUniformBuffers },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, deviceLimits.maxPerStageDescriptorStorageBuffers },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       deviceLimits.maxPerStageDescriptorInputAttachments }
	};

	VkDescriptorPoolCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		nullptr,
		VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		eMaxSetCount,
		_countof(descriptorPoolSize),
		nullptr
	};
	VKCheck(vkCreateDescriptorPool(VulkanEngine::Instance().GetDevice(), &createInfo, nullptr, &m_Handle));
}

VulkanDescriptorSet VulkanDescriptorPool::AllocDescriptorSet(VulkanDescriptorSetLayout &layout)
{
	VkDescriptorSetAllocateInfo allocInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		m_Handle,
		1U,
		&layout
	};

	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

	VKCheck(vkAllocateDescriptorSets(VulkanEngine::Instance().GetDevice(), &allocInfo, &descriptorSet));

	return VulkanDescriptorSet(descriptorSet);
}

void VulkanDescriptorPool::ResetPool()
{
	assert(IsValid());

	VKCheck(vkResetDescriptorPool(VulkanEngine::Instance().GetDevice(), m_Handle, 0U));
}

void VulkanDescriptorPool::Destory()
{
	assert(IsValid());

	vkDestroyDescriptorPool(VulkanEngine::Instance().GetDevice(), m_Handle, nullptr);
}