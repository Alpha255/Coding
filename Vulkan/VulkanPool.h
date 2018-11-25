#pragma once

#include "VulkanObject.h"

class VulkanCommandPool : public VulkanObject<VkCommandPool>
{
public:
	void Create(uint32_t flags);

	inline VulkanCommandBuffer AllocCommandBuffer(uint32_t level, uint32_t count)
	{
		VulkanCommandBuffer buffer;
		buffer.Create(*this, level, count);

		return buffer;
	}
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
