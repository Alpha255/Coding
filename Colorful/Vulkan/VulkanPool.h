#pragma once

#include "VulkanBuffer.h"

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

	VulkanCommandBuffer Alloc(eBufferType type, uint32_t count);

	void Free(VulkanCommandBuffer &cmdBuffer);

	void Destory() override;
protected:
private:
	uint32_t m_BufferCount = 0U;
};

#if 0
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
#endif
