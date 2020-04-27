#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

class vkDeviceQueue : public vkObject<VkQueue>
{
public:
	void create(uint32_t queueFamilyIndex, const class vkDevice &device);

	void submit(const class vkCommandBuffer &cmdBuffer, const class vkSemaphore *waitSemaphores, uint32_t waitSemaphoreCount);

	void present(
		const class vkCommandBuffer &cmdBuffer,
		const class vkSwapchain &swapchain, 
		const class vkSemaphore *waitSemaphores, 
		uint32_t waitSemaphoreCount);

	void waitIdle();
protected:
private:
	uint32_t m_FamilyIndex = UINT32_MAX;
	vkSemaphore *m_RenderCompleteSemaphore = nullptr;
};
