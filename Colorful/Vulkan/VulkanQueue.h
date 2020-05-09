#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

class VulkanQueue : public VulkanObject<VkQueue>
{
public:
	VulkanQueue(VkDevice device, uint32_t queueFamilyIndex);

	void submit(class vkCommandBuffer &cmdBuffer);

	void present(
		class vkCommandBuffer &cmdBuffer,
		const class VulkanSwapchain &swapchain,
		VkFence fence);

	void queueCommandBuffer(class vkCommandBuffer *cmdBuffer);
	void submit(const class VulkanSwapchain &swapchain);

	void waitIdle();

	void destroy(VkDevice device);
protected:
private:
	uint32_t m_FamilyIndex = std::numeric_limits<uint32_t>().max();
	class vkSemaphore *m_RenderCompleteSemaphore = nullptr;
	std::vector<class vkCommandBuffer *> m_QueuedCmdBuffers;
};
using VulkanQueuePtr = std::shared_ptr<VulkanQueue>;

class VulkanQueueManager : public LazySingleton<VulkanQueueManager>
{
	lazySingletonDeclare(VulkanQueueManager);
public:
	inline VulkanQueuePtr& gfxQueue()
	{
		return m_GfxQueue;
	}

	inline VulkanQueuePtr& computeQueue()
	{
		return m_ComputeQueue;
	}

	inline VulkanQueuePtr& transferQueue()
	{
		return m_TransferQueue;
	}

	void cleanup() override final
	{
		m_GfxQueue->Handle = VK_NULL_HANDLE;
		m_ComputeQueue->Handle = VK_NULL_HANDLE;
		m_TransferQueue->Handle = VK_NULL_HANDLE;
	}
protected:
	VulkanQueueManager(
		const VkDevice device,
		uint32_t gfxQueueFamilyIndex,
		uint32_t computeQueueFamilyIndex,
		uint32_t transferQueueFamilyIndex);
private:
	VulkanQueuePtr m_GfxQueue = nullptr;
	VulkanQueuePtr m_ComputeQueue = nullptr;
	VulkanQueuePtr m_TransferQueue = nullptr;
};
