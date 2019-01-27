#pragma once

#include "VulkanBuffer.h"
#include <unordered_map>

class VulkanCommandPool : public VulkanObject<VkCommandPool>
{
public:
	enum ePoolType
	{
		eTemp,
		eGeneral,
		eProtected,
		eGeneralTemp,
		ePoolTypeCount
	};

	enum eBufferType
	{
		ePrimary = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		eSecondary = VK_COMMAND_BUFFER_LEVEL_SECONDARY
	};

	void Create(ePoolType type);

	VulkanCommandBuffer Alloc(eBufferType type);
	VulkanCommandBufferList Alloc(eBufferType type, uint32_t count);

	void Free(VulkanCommandBuffer &cmdBuffer);
	void Free(VulkanCommandBufferList &cmdBufferList);

	void Destory();
protected:
private:
	std::unordered_map<uint32_t, VkCommandBuffer> m_CommandBuffers;
	uint32_t m_BufferCount = 0U;
};

class VulkanDescriptorPool : public VulkanObject<VkDescriptorPool>
{
public:
	void Create();

	VulkanDescriptorSet AllocDescriptorSet(VulkanDescriptorSetLayout &layout);

	void ResetPool();

	void Destory();
protected:
	enum eLimits
	{
		eMaxSetCount = 1024
	};
private:
};
