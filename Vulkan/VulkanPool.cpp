#include "VulkanPool.h"
#include "VulkanEngine.h"

void VulkanCommandPool::Create(ePoolType type)
{
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
		VulkanEngine::Instance().GetVulkanDevice().GetQueueFamilyIndex()
	};

	VKCheck(vkCreateCommandPool(VulkanEngine::Instance().GetDevice(), &createInfo, nullptr, &m_Handle));
}

void VulkanCommandPool::Destory()
{
	assert(IsValid());

	/// When a pool is destroyed, all command buffers allocated from the pool are freed
	vkDestroyCommandPool(VulkanEngine::Instance().GetDevice(), m_Handle, nullptr);
}

void VulkanDescriptorPool::Create(bool bUseTex)
{
	VkDescriptorPoolSize poolSize[2]{};
	poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize[0].descriptorCount = 1U;

	if (bUseTex)
	{
		poolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize[1].descriptorCount = 1;
	}

	VkDescriptorPoolCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		nullptr,
		0U,
		1U,
		bUseTex ? 2 : 1,
		poolSize
	};

	VKCheck(vkCreateDescriptorPool(VulkanEngine::Instance().GetDevice(), &createInfo, nullptr, &m_Handle));
}