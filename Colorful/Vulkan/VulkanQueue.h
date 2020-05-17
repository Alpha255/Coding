#pragma once

#include "Colorful/Vulkan/VulkanBuffer.h"
#include "Colorful/Vulkan/VulkanCommand.h"

class VulkanQueue : public VulkanObject<VkQueue>
{
public:
	VulkanQueue(VkDevice device, uint32_t queueFamilyIndex);

	void submit(const VulkanCommandBufferPtr& cmdBuffer);

	void present(
		const VulkanCommandBufferPtr& cmdBuffer,
		const class VulkanSwapchain& swapchain,
		VkFence fence);

	void queueCommandBuffer(const VulkanCommandBufferPtr& cmdBuffer);
	void submit(const class VulkanSwapchain &swapchain);

	void queueSubmitCopyCommand(VkBuffer dstBuffer, const VulkanBuffer& stagingBuffer, const VkBufferCopy& copyInfo)
	{
		VulkanCopyCommand copyCommand
		{
			dstBuffer,
			stagingBuffer,
			copyInfo
		};
		m_CopyQueue.push(std::move(copyCommand));
	}

	void queueSubmitImageLayoutTransfer()
	{

	}

	void waitIdle();

protected:
	struct VulkanCopyCommand
	{
		VkBuffer DstBuffer = VK_NULL_HANDLE;
		VulkanBuffer StagingBuffer;
		VkBufferCopy CopyInfo{};
	};
private:
	uint32_t m_FamilyIndex = std::numeric_limits<uint32_t>().max();
	class VulkanSemaphore *m_RenderCompleteSemaphore = nullptr;
	std::vector<VulkanCommandBufferPtr> m_QueuedCmdBuffers;
	std::stack<VulkanCopyCommand> m_CopyQueue;
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

	inline VulkanCommandBufferPtr allocGfxCommandBuffer(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY)
	{
		return m_GfxCmdBufferPool->alloc(level);
	}

	inline VulkanCommandBufferPtr allocTransferCommandBuffer(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY)
	{
		return m_TransferCmdBufferPool->alloc(level);
	}

	void cleanup() override final
	{
		m_GfxCmdBufferPool->destroy();
		m_ComputeCmdBufferPool->destroy();
		m_TransferCmdBufferPool->destroy();

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

	VulkanCommandPoolPtr m_GfxCmdBufferPool = nullptr;
	VulkanCommandPoolPtr m_ComputeCmdBufferPool = nullptr;
	VulkanCommandPoolPtr m_TransferCmdBufferPool = nullptr;
};
