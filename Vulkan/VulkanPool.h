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

	VulkanCommandPool() = default;
	inline ~VulkanCommandPool()
	{
		Destory();
	}

	void Create(ePoolType type);

	inline VulkanCommandBuffer Alloc(uint32_t level, uint32_t count)
	{
		VulkanCommandBuffer buffer;
		buffer.Create(*this, level, count);

		return buffer;
	}

	void Destory();
protected:
private:
};

class VulkanDescriptorPool : public VulkanObject<VkDescriptorPool>
{
public:
	void Create(bool bUseTex);
protected:
private:
};
