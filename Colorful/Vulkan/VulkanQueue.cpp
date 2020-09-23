#if 0
#include "Colorful/Vulkan/VulkanEngine.h"

VulkanQueue::VulkanQueue(VkDevice device, uint32_t queueFamilyIndex)
{
	assert(!isValid());

	/// vkGetDeviceQueue must only be used to get queues that were created with the flags parameter of VkDeviceQueueCreateInfo set to zero. 
	/// To get queues that were created with a non-zero flags parameter use vkGetDeviceQueue2.
	m_FamilyIndex = queueFamilyIndex;
	vkGetDeviceQueue(device, queueFamilyIndex, 0u, &Handle);

	/// All queues associated with a logical device are destroyed when vkDestroyDevice is called on that device
}

void VulkanQueue::waitIdle()
{
	/// To wait on the host for the completion of outstanding queue operations for a given queue
	/// equivalent to submitting a fence to a queue and waiting with an infinite timeout for that fence to signal.
	if (isValid())
	{
		GfxVerifyVk(vkQueueWaitIdle(Handle));
	}
}

void VulkanQueue::submit(const VulkanCommandBufferPtr& cmdBuffer)
{
	VkSubmitInfo submitInfo
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		0u,
		nullptr,
		nullptr,
		1u,
		&cmdBuffer->Handle,
		0u,
		nullptr
	};
	GfxVerifyVk(vkQueueSubmit(Handle, 1u, &submitInfo, cmdBuffer->fence()->Handle));
	VulkanAsyncPool::instance()->waitFence(cmdBuffer->fence());
}

VulkanQueueManager::VulkanQueueManager(
	const VkDevice device, 
	uint32_t gfxQueueFamilyIndex, 
	uint32_t computeQueueFamilyIndex, 
	uint32_t transferQueueFamilyIndex)
	: m_Device(device)
{
	assert(gfxQueueFamilyIndex != std::numeric_limits<uint32_t>().max());
	m_GfxQueue = std::make_shared<VulkanQueue>(device, gfxQueueFamilyIndex);
	m_GfxCmdBufferPool = std::make_shared<VulkanCommandPool>(device, gfxQueueFamilyIndex);

	if (computeQueueFamilyIndex != std::numeric_limits<uint32_t>().max())
	{
		m_ComputeQueue = std::make_shared<VulkanQueue>(device, computeQueueFamilyIndex);
		m_ComputeCmdBufferPool = std::make_shared<VulkanCommandPool>(device, computeQueueFamilyIndex);
	}

	if (transferQueueFamilyIndex != std::numeric_limits<uint32_t>().max())
	{
		m_TransferQueue = std::make_shared<VulkanQueue>(device, transferQueueFamilyIndex);
		m_TransferCmdBufferPool = std::make_shared<VulkanCommandPool>(device, transferQueueFamilyIndex);
	}

	m_RenderCompleteSemaphore = VulkanAsyncPool::instance()->allocSemaphore();
}

void VulkanQueueManager::queueGfxCommand(const VulkanCommandBufferPtr& cmdBuffer)
{
	bool8_t contain = false;
	for (uint32_t i = 0u; i < m_GfxCommandQueue.size(); ++i)
	{
		if (m_GfxCommandQueue[i] == cmdBuffer)
		{
			contain = true;
			break;
		}
	}

	if (!contain)
	{
		m_GfxCommandQueue.emplace_back(cmdBuffer);
	}
}

void VulkanQueueManager::queueBufferCopyCommand(VkBuffer dstBuffer, size_t size, const void* data)
{
	assert(dstBuffer != VK_NULL_HANDLE && size > 0u && data);

	VulkanBufferCopyCommand command{};
	command.DstBuffer = dstBuffer;
	command.StagingBuffer = VulkanBufferPool::instance()->allocStagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size, data);
	command.CopyInfo.size = size;

	m_BufferCopyCommandQueue.emplace(std::move(command));
}

void VulkanQueueManager::queueImageCopyCommand(
	VkImage dstImage, 
	VulkanImage::eImageLayout srcLayout, 
	VulkanImage::eImageLayout dstLayout, 
	const VkImageSubresourceRange& subresrouceRange, 
	const std::vector<VkBufferImageCopy>& imageCopies,
	size_t size,
	const void* data)
{
	assert(dstImage != VK_NULL_HANDLE);
	assert(srcLayout < VulkanImage::eImageLayout_MaxEnum && dstLayout != VulkanImage::eImageLayout_MaxEnum);
	assert(imageCopies.size() > 0u);

	VulkanImageCopyCommand command;
	command.DstImage = dstImage;
	command.StagingBuffer = VulkanBufferPool::instance()->allocStagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size, data);
	command.SrcLayout = srcLayout;
	command.DstLayout = dstLayout;
	command.SubresourceRange = subresrouceRange;
	command.ImageCopies = imageCopies;

	m_ImageCopyCommandQueue.emplace(std::move(command));
}

void VulkanQueueManager::submitBufferCopyCommands()
{
	if (m_BufferCopyCommandQueue.empty())
	{
		return;
	}

	auto cmdBuffer = m_TransferCmdBufferPool->alloc();
	cmdBuffer->begin();

	while (!m_BufferCopyCommandQueue.empty())
	{
		auto& cmd = m_BufferCopyCommandQueue.front();

		vkCmdCopyBuffer(cmdBuffer->Handle, cmd.StagingBuffer->Handle, cmd.DstBuffer, 1u, &cmd.CopyInfo);

		VulkanBufferPool::instance()->free(cmd.StagingBuffer);
		m_BufferCopyCommandQueue.pop();
	}

	cmdBuffer->end();

	m_TransferQueue->submit(cmdBuffer);

	m_TransferCmdBufferPool->free(cmdBuffer);
}

void VulkanQueueManager::submitImageCopyCommands()
{
	if (m_ImageCopyCommandQueue.empty())
	{
		return;
	}

	auto cmdBuffer = m_GfxCmdBufferPool->alloc();
	cmdBuffer->begin();

	while (!m_ImageCopyCommandQueue.empty())
	{
		auto &cmd = m_ImageCopyCommandQueue.front();

		VkPipelineStageFlags stageFlags = 0u;
		VkAccessFlags accessFlags = 0u;
		VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VulkanImage::makeMemoryBarrierFlags(cmd.SrcLayout, stageFlags, accessFlags, imageLayout);

		VkImageMemoryBarrier imageMemoryBarrier
		{
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			nullptr,
			0u,
			accessFlags,
			VK_IMAGE_LAYOUT_UNDEFINED,
			imageLayout,
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED,
			cmd.DstImage,
			cmd.SubresourceRange
		};
		vkCmdPipelineBarrier(cmdBuffer->Handle, VK_PIPELINE_STAGE_HOST_BIT, stageFlags, 0u, 0u, nullptr, 0u, nullptr, 1u, &imageMemoryBarrier);

		vkCmdCopyBufferToImage(cmdBuffer->Handle, cmd.StagingBuffer->Handle, cmd.DstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, (uint32_t)cmd.ImageCopies.size(), cmd.ImageCopies.data());

		VkPipelineStageFlags oldStageFlags = stageFlags;
		VulkanImage::makeMemoryBarrierFlags(cmd.DstLayout, stageFlags, accessFlags, imageLayout);

		imageMemoryBarrier.srcAccessMask = imageMemoryBarrier.dstAccessMask;
		imageMemoryBarrier.oldLayout = imageMemoryBarrier.newLayout;
		imageMemoryBarrier.dstAccessMask = accessFlags;
		imageMemoryBarrier.newLayout = imageLayout;

		vkCmdPipelineBarrier(cmdBuffer->Handle, oldStageFlags, stageFlags, 0u, 0u, nullptr, 0u, nullptr, 1u, &imageMemoryBarrier);

		VulkanBufferPool::instance()->free(cmd.StagingBuffer);
		m_ImageCopyCommandQueue.pop();
	}

	cmdBuffer->end();

	m_GfxQueue->submit(cmdBuffer);

	m_GfxCmdBufferPool->free(cmdBuffer);
}

void VulkanQueueManager::submitGfxCommands(VkSemaphore presentCompleteSemaphore)
{
	/// Submission can be a high overhead operation, and applications should attempt to batch work together into as few calls to vkQueueSubmit as possible.
	/// vkQueueSubmit is a queue submission command, with each batch defined by an element of pSubmits. 
	/// Batches begin execution in the order they appear in pSubmits, but may complete out of order.

	std::vector<VkCommandBuffer> cmdBuffers(m_GfxCommandQueue.size());
	for (uint32_t i = 0u; i < m_GfxCommandQueue.size(); ++i)
	{
		cmdBuffers[i] = m_GfxCommandQueue[i]->Handle;

		if (m_GfxCommandQueue[i]->isInsideRenderPass())
		{
			m_GfxCommandQueue[i]->endRenderPass();
		}

		assert(m_GfxCommandQueue[i]->state() == VulkanCommandBuffer::eExecutable);

		m_GfxCommandQueue[i]->setState(VulkanCommandBuffer::eInitial);
	}

	VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submitInfo
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		1u,
		&presentCompleteSemaphore,
		&waitStageMask,
		(uint32_t)cmdBuffers.size(),
		cmdBuffers.data(),
		1u,
		&m_RenderCompleteSemaphore->Handle
	};
	GfxVerifyVk(vkQueueSubmit(m_GfxQueue->Handle, 1u, &submitInfo, VK_NULL_HANDLE));

	m_GfxCommandQueue.clear();
}
#endif
