#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

class vkDeviceQueue : public VulkanObject<VkQueue>
{
public:
	void create(uint32_t queueFamilyIndex, const class VulkanDevice &device);

	void submit(class vkCommandBuffer &cmdBuffer);

	void present(
		class vkCommandBuffer &cmdBuffer,
		const class vkSwapchain &swapchain,
		VkFence fence);

	void queueCommandBuffer(class vkCommandBuffer *cmdBuffer);
	void submit(const class vkSwapchain &swapchain);

	void waitIdle();

	void destroy(const class VulkanDevice &device);
protected:
private:
	uint32_t m_FamilyIndex = std::numeric_limits<uint32_t>().max();
	class vkSemaphore *m_RenderCompleteSemaphore = nullptr;
	std::vector<class vkCommandBuffer *> m_QueuedCmdBuffers;
};
