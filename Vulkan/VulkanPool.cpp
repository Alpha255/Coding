#include "VulkanPool.h"
#include "VulkanEngine.h"

void VulkanCommandPool::Create(uint32_t flags)
{
	VkCommandPoolCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		flags,
		VulkanEngine::Instance().GetSwapchain().GetGraphicsQueueFamilyIndex()
	};

	VKCheck(vkCreateCommandPool(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));
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

	VKCheck(vkCreateDescriptorPool(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));
}