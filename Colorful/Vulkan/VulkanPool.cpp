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

void VulkanDescriptorSetLayout::Create(uint32_t targetShader, uint32_t slot, uint32_t count)
{
	/// For test
	assert(!IsValid() && count == 1U);

	VkShaderStageFlags shaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	switch (targetShader)
	{
	case eVertexShader:
		shaderStage = VK_SHADER_STAGE_VERTEX_BIT;
		break;
	case ePixelShader:
		shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
		break;
	default:
		assert(0);
		break;
	}

	std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = 
	{
		{
			slot,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			1U,
			shaderStage,
			nullptr
		}
	};

	VkDescriptorSetLayoutCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		nullptr,
		0U,
		(uint32_t)setLayoutBindings.size(),
		setLayoutBindings.data()
	};

	Check(vkCreateDescriptorSetLayout(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));
}

void VulkanDescriptorSetLayout::Destory()
{
	assert(IsValid());

	vkDestroyDescriptorSetLayout(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	Reset();
}

void VulkanDescriptorSet::Update(const VulkanSamplerState &sampler, const VulkanImageView &imageView, uint32_t slot)
{
	assert(IsValid());

	VkDescriptorImageInfo descriptorImageInfo
	{
		sampler.Get(),
		imageView.Get(),
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

void VulkanDescriptorPool::Create()
{
	assert(!IsValid());

	m_Type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

	VkDescriptorPoolSize descriptorPoolSize
	{
		m_Type,
		1U
	};

	VkDescriptorPoolCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		nullptr,
		0U,
		2U,
		1U,
		&descriptorPoolSize
	};
	Check(vkCreateDescriptorPool(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));
}

VulkanDescriptorSet VulkanDescriptorPool::Alloc(uint32_t targetShader, uint32_t slot, uint32_t count)
{
	VulkanDescriptorSet descriptorSet;
	descriptorSet.CreateLayout(targetShader, slot, count);

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