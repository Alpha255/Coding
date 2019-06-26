#include "VulkanPool.h"
#include "VulkanEngine.h"

void VulkanCommandPool::Create(ePoolType type)
{
	assert(!IsValid());

	m_Type = type;

	uint32_t flags = VK_COMMAND_POOL_CREATE_FLAG_BITS_MAX_ENUM;
	switch (type)
	{
	case eTemp: 
		flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;  
		break;
	case eGeneral: 
		flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;  
		break;
	case eProtected: 
		flags = VK_COMMAND_POOL_CREATE_PROTECTED_BIT; 
		break;
	case eGeneralTemp: 
		flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; 
		break;
	default:
		assert(0);
		break;
	}

	VkCommandPoolCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		flags,
		VulkanEngine::Instance().GetDevice().GetQueueFamilyIndex()
	};

	Check(vkCreateCommandPool(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));
}

void VulkanCommandPool::Destory()
{
	assert(IsValid());

	/// When a pool is destroyed, all command buffers allocated from the pool are freed
	vkDestroyCommandPool(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	m_BufferCount = 0U;

	Reset();
}

VulkanCommandBuffer VulkanCommandPool::Alloc(eBufferType type, uint32_t count)
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

	VulkanCommandBuffer cmdBuffer(count);
	Check(vkAllocateCommandBuffers(VulkanEngine::Instance().GetDevice().Get(), &allocInfo, &cmdBuffer));

	cmdBuffer.SetPoolType(m_Type);

	m_BufferCount += count;

	return cmdBuffer;
}

void VulkanCommandPool::Free(VulkanCommandBuffer &cmdBuffer)
{
	assert(IsValid());

	vkFreeCommandBuffers(VulkanEngine::Instance().GetDevice().Get(), m_Handle, cmdBuffer.GetBufferCount(), &cmdBuffer);

	m_BufferCount -= cmdBuffer.GetBufferCount();

	cmdBuffer.Clear();
}

void VulkanDescriptorSetLayout::Create(const std::vector<VkDescriptorSetLayoutBinding> &layoutBindings)
{
	assert(!IsValid());

	VkDescriptorSetLayoutCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		nullptr,
		0U,
		(uint32_t)layoutBindings.size(),
		layoutBindings.data()
	};

	Check(vkCreateDescriptorSetLayout(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));
}

void VulkanDescriptorSetLayout::Destory()
{
	assert(IsValid());

	vkDestroyDescriptorSetLayout(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	Reset();
}

void VulkanDescriptorSet::SetupImage(const VulkanImageView &image, uint32_t slot)
{
	assert(IsValid());

	/// Members of VkDescriptorImageInfo that are not used in an update(as described above) are ignored.
	VkDescriptorImageInfo descriptorImageInfo
	{
		VK_NULL_HANDLE,
		image.Get(),
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	};

	VkWriteDescriptorSet writeDescriptorSet
	{
		VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		nullptr,
		m_Handle,
		slot,
		0U,
		1U,
		VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
		&descriptorImageInfo,
		nullptr,
		nullptr
	};

	vkUpdateDescriptorSets(VulkanEngine::Instance().GetDevice().Get(), 1U, &writeDescriptorSet, 0U, nullptr);
}

void VulkanDescriptorSet::SetupSampler(const VulkanSamplerState &sampler, uint32_t slot)
{
	assert(IsValid());

	/// Members of VkDescriptorImageInfo that are not used in an update(as described above) are ignored.
	VkDescriptorImageInfo descriptorImageInfo
	{
		sampler.Get(),
		VK_NULL_HANDLE,
		VK_IMAGE_LAYOUT_UNDEFINED
	};

	VkWriteDescriptorSet writeDescriptorSet
	{
		VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		nullptr,
		m_Handle,
		slot,
		0U,
		1U,
		VK_DESCRIPTOR_TYPE_SAMPLER,
		&descriptorImageInfo,
		nullptr,
		nullptr
	};

	vkUpdateDescriptorSets(VulkanEngine::Instance().GetDevice().Get(), 1U, &writeDescriptorSet, 0U, nullptr);
}

void VulkanDescriptorSet::SetupCombinedImage(const VulkanImageView &image, uint32_t slot)
{
	assert(IsValid());

	/// Members of VkDescriptorImageInfo that are not used in an update(as described above) are ignored.
	VkDescriptorImageInfo descriptorImageInfo
	{
		image.GetSampler().Get(),
		image.Get(),
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	};

	VkWriteDescriptorSet writeDescriptorSet
	{
		VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		nullptr,
		m_Handle,
		slot,
		0U,
		1U,
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		&descriptorImageInfo,
		nullptr,
		nullptr
	};

	vkUpdateDescriptorSets(VulkanEngine::Instance().GetDevice().Get(), 1U, &writeDescriptorSet, 0U, nullptr);
}

void VulkanDescriptorSet::SetupUniformBuffer(const VulkanBuffer &uniformBuffer, uint32_t slot)
{
	assert(IsValid());

	VkDescriptorBufferInfo bufferInfo = uniformBuffer.GetDescriptorInfo();
	VkWriteDescriptorSet writeDescriptorSet
	{
		VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		nullptr,
		m_Handle,
		slot,
		0U,
		1U,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		nullptr,
		&bufferInfo,
		nullptr
	};

	vkUpdateDescriptorSets(VulkanEngine::Instance().GetDevice().Get(), 1U, &writeDescriptorSet, 0U, nullptr);
}

void VulkanDescriptorPool::Create()
{
	assert(!IsValid());

	std::vector<VkDescriptorPoolSize> descriptorPoolSize
	{
		{
			(VkDescriptorType)eImage,
			eMaxSamplers
		},

		{
			(VkDescriptorType)eSampler,
			eMaxSamplers
		},

		{
			(VkDescriptorType)eCombinedImageSampler,
			eMaxSamplers
		},
		{
			(VkDescriptorType)eUniformBuffer,
			eMaxUniformBuffers
		},
	};

	VkDescriptorPoolCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		nullptr,
		0U,
		eMasDescriptorSets,
		(uint32_t)descriptorPoolSize.size(),
		descriptorPoolSize.data()
	};
	Check(vkCreateDescriptorPool(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));
}

VulkanDescriptorSet VulkanDescriptorPool::Alloc(const std::vector<VkDescriptorSetLayoutBinding> &layoutBindings)
{
	VulkanDescriptorSet descriptorSet;
	descriptorSet.CreateLayout(layoutBindings);

	VkDescriptorSetAllocateInfo allocInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		m_Handle,
		1U,
		&descriptorSet.GetLayout()
	};

	Check(vkAllocateDescriptorSets(VulkanEngine::Instance().GetDevice().Get(), &allocInfo, &descriptorSet));

	return descriptorSet;
}

void VulkanDescriptorPool::Free(VulkanDescriptorSet &descriptorSet)
{
	assert(descriptorSet.IsValid());

	Check(vkFreeDescriptorSets(VulkanEngine::Instance().GetDevice().Get(), m_Handle, 1U, &descriptorSet));
}

void VulkanDescriptorPool::ResetPool()
{
	assert(IsValid());

	Check(vkResetDescriptorPool(VulkanEngine::Instance().GetDevice().Get(), m_Handle, 0U));
}

void VulkanDescriptorPool::Destory()
{
	assert(IsValid());

	vkDestroyDescriptorPool(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	Reset();
}