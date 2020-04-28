#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

class vkDeviceQueue : public vkObject<VkQueue>
{
public:
	void create(uint32_t queueFamilyIndex, const class vkDevice &device);

	void submit(class vkCommandBuffer &cmdBuffer);

	void present(
		class vkCommandBuffer &cmdBuffer,
		const class vkSwapchain &swapchain,
		VkFence fence);

	void waitIdle();

	void destroy(const class vkDevice &device);
protected:
private:
	uint32_t m_FamilyIndex = UINT32_MAX;
	class vkSemaphore *m_RenderCompleteSemaphore = nullptr;
};
