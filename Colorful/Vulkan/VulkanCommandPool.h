#pragma once

#include "Colorful/Vulkan/VulkanCommand.h"

NAMESPACE_START(Gfx)

class VulkanCommandBufferPool final : public VkDeviceObject<VkCommandPool_T>
{
public:
	VulkanCommandBufferPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags);
	~VulkanCommandBufferPool();

	VulkanCommandBufferPtr getOrAlloc(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY, bool8_t autoBegin = true);
	void free(VulkanCommandBufferPtr& commandBuffer);

	void reset();
protected:
private:
	std::deque<VulkanCommandBufferPtr> m_CommandBuffers; /// Ring buffer ???
};
DECLARE_SHARED_PTR(VulkanCommandBufferPool)

NAMESPACE_END(Gfx)
