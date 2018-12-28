#pragma once

#include "VulkanObject.h"
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

	VulkanCommandPool() = default;
	inline ~VulkanCommandPool()
	{
		Destory();
	}

	void Create(ePoolType type);

	VulkanCommandBuffer Alloc(eBufferType type);
	VulkanCommandBufferList Alloc(eBufferType type, uint32_t count);

	void Free(VulkanCommandBuffer &cmdBuffer);
	void Free(VulkanCommandBufferList &cmdBufferList);

	void Destory();
protected:
private:
};

//class VulkanDescriptorPool : public VulkanObject<VkDescriptorPool>
//{
//public:
//	void Create(bool bUseTex);
//protected:
//private:
//};
