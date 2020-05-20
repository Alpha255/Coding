#pragma once

#include "Colorful/Vulkan/VulkanBuffer.h"
#include "Colorful/Vulkan/VulkanCommand.h"

class VulkanQueue : public VulkanObject<VkQueue>
{
public:
	VulkanQueue(VkDevice device, uint32_t queueFamilyIndex);

	//void submit(const VulkanCommandBufferPtr& cmdBuffer);

	//void present(
	//	const VulkanCommandBufferPtr& cmdBuffer,
	//	const class VulkanSwapchain& swapchain,
	//	VkFence fence);

	//void submit(const class VulkanSwapchain &swapchain);

	void waitIdle();

protected:
private:
	uint32_t m_FamilyIndex = std::numeric_limits<uint32_t>().max();
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

	void queueGfxCommand(const VulkanCommandBufferPtr& cmdBuffer);

	void queueBufferCopyCommand(VkBuffer dstBuffer, size_t size, const void* data);

	void queueImageCopyCommand(
		VkImage dstImage,
		VulkanImage::eImageLayout srcLayout,
		VulkanImage::eImageLayout dstLayout,
		const VkImageSubresourceRange& subresrouceRange,
		const std::vector<VkBufferImageCopy>& imageCopies,
		size_t size,
		const void* data);

	void submitQueuedCommands();

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

	struct VulkanBufferCopyCommand
	{
		VkBuffer DstBuffer = VK_NULL_HANDLE;
		std::shared_ptr<VulkanBuffer> StagingBuffer = nullptr;
		VkBufferCopy CopyInfo{};
	};

	struct VulkanImageCopyCommand
	{
		VkImage DstImage = VK_NULL_HANDLE;
		std::shared_ptr<VulkanBuffer> StagingBuffer = nullptr;
		VulkanImage::eImageLayout SrcLayout = VulkanImage::eImageLayout_MaxEnum;
		VulkanImage::eImageLayout DstLayout = VulkanImage::eImageLayout_MaxEnum;
		VkImageSubresourceRange SubresourceRange{};
		std::vector<VkBufferImageCopy> ImageCopies;
	};
private:
	const VkDevice m_Device;

	VulkanQueuePtr m_GfxQueue = nullptr;
	VulkanQueuePtr m_ComputeQueue = nullptr;
	VulkanQueuePtr m_TransferQueue = nullptr;

	VulkanCommandPoolPtr m_GfxCmdBufferPool = nullptr;
	VulkanCommandPoolPtr m_ComputeCmdBufferPool = nullptr;
	VulkanCommandPoolPtr m_TransferCmdBufferPool = nullptr;

	std::queue<VulkanBufferCopyCommand> m_BufferCopyCommandQueue;
	std::queue<VulkanImageCopyCommand> m_ImageCopyCommandQueue;
	std::vector<VulkanCommandBufferPtr> m_GfxCommandQueue;
};
