#pragma once

#include "VulkanLoader.h"

class vkCommandBufferArray
{
public:
	vkCommandBufferArray(VkCommandBufferLevel level, uint32_t count)
		: m_Level(level)
	{
		m_CommandBuffers.resize(count);
	}

	inline VkCommandBuffer get(uint32_t index = 0u) const
	{
		assert(m_CommandBuffers.size() > 0u && index < m_CommandBuffers.size());
		return m_CommandBuffers[index];
	}

	void begin();
	void end(bool8_t submit = true);
	void queueSubmit();
protected:
	inline void destory()
	{
		m_CommandBuffers.clear();
		m_Level = VK_COMMAND_BUFFER_LEVEL_MAX_ENUM;
	}
private:
	friend class vkCommandPool;

	std::vector<VkCommandBuffer> m_CommandBuffers;
	VkCommandBufferLevel m_Level = VK_COMMAND_BUFFER_LEVEL_MAX_ENUM;
};

class vkCommandPool : public vkDeviceObject<VkCommandPool>
{
public:
	void create(const class vkDevice &device, uint32_t queueIndex);
	void destroy(const class vkDevice &device) override final;

	vkCommandBufferArray allocCommandBuffers(const class vkDevice &device, VkCommandBufferLevel level, uint32_t count) const;
	void freeCommandBuffers(const class vkDevice &device, vkCommandBufferArray &commandBuffers) const;
protected:
private:
};
