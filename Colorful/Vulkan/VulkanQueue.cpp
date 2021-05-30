#include "Colorful/Vulkan/VulkanQueue.h"
#include "Colorful/Vulkan/VulkanDevice.h"

NAMESPACE_START(Gfx)

VkQueues::VkTransferCommand::VkTransferCommand(
	const TextureDesc& desc,
	VkImage image,
	const VkImageSubresourceRange& subresourceRange,
	Texture::EImageLayout srcLayout,
	Texture::EImageLayout dstLayout)
	: DstImage(image)
	, ImageSubresourceRange(subresourceRange)
	, SrcLayout(srcLayout)
	, DstLayout(dstLayout)
{
	assert(desc.Data && desc.DataSize && image && dstLayout != Texture::EImageLayout::Undefined);
	StagingBuffer = create_shared<VulkanBuffer>(0u, EBufferUsage::Staging, desc.DataSize, desc.Data);

	ImageCopyRanges.resize(desc.Subresources.size());
	assert(desc.Subresources.size() == (static_cast<size_t>(desc.MipLevels) * static_cast<size_t>(desc.ArraySize)));

	for (uint32_t i = 0u; i < desc.Subresources.size(); ++i)
	{
		ImageCopyRanges[i] = VkBufferImageCopy
		{
			desc.Subresources[i].Offset,
			desc.Subresources[i].RowBytes,
			desc.Subresources[i].Height,
			{
				ImageSubresourceRange.aspectMask,
				desc.Subresources[i].MipIndex,
				desc.Subresources[i].ArrayIndex,
				1u
			},
			{ 0u, 0u, 0u },
			{
				desc.Subresources[i].Width,
				desc.Subresources[i].Height,
				desc.Subresources[i].Depth
			}
		};
	}
}

VkQueues::VkTransferCommand::VkTransferCommand(VkBuffer buffer, const VkBufferCopy& bufferRange, const void* data)
	: DstBuffer(buffer)
	, BufferRange(bufferRange)
{
	assert(buffer && BufferRange.size && data);
	StagingBuffer = create_shared<VulkanBuffer>(0u, EBufferUsage::Staging, BufferRange.size, data);
}

void VkQueues::VkTransferCommand::getImageBarrierMasks(
	Texture::EImageLayout layout, 
	__out VkPipelineStageFlags& stageFlags, 
	__out VkAccessFlags& accessFlags, 
	__out VkImageLayout& imageLayout)
{
	stageFlags = 0u;
	accessFlags = 0u;
	imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;

	switch (layout)
	{
	case Texture::EImageLayout::Undefined:
		accessFlags = 0;
		stageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		break;
	case Texture::EImageLayout::TransferDst:
		accessFlags = VK_ACCESS_TRANSFER_WRITE_BIT;
		stageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		imageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		break;
	case Texture::EImageLayout::ColorAttachment:
		accessFlags = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		stageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		break;
	case Texture::EImageLayout::DepthStencilAttachment:
		accessFlags = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		stageFlags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		break;
	case Texture::EImageLayout::TransferSrc:
		accessFlags = VK_ACCESS_TRANSFER_READ_BIT;
		stageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		imageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		break;
	case Texture::EImageLayout::Present:
		accessFlags = 0u;
		stageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		break;
	case Texture::EImageLayout::FragmentShaderRead:
		accessFlags = VK_ACCESS_SHADER_READ_BIT;
		stageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		break;
	case Texture::EImageLayout::PixelDepthStencilRead:
		accessFlags = VK_ACCESS_SHADER_READ_BIT;
		stageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		break;
	case Texture::EImageLayout::ComputeShaderReadWrite:
		accessFlags = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
		stageFlags = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		break;
	case Texture::EImageLayout::FragmentShaderReadWrite:
		accessFlags = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
		stageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		break;
	}
}

VulkanQueue::VulkanQueue(uint32_t queueFamilyIndex, uint32_t commandPoolCreateFlags)
	: m_QueueFamilyIndex(queueFamilyIndex)
{
	/// vkGetDeviceQueue must only be used to get queues that were created with the flags parameter of VkDeviceQueueCreateInfo set to zero. 
	/// To get queues that were created with a non-zero flags parameter use vkGetDeviceQueue2.
	
	/// All queues associated with a logical device are destroyed when vkDestroyDevice is called on that device
	assert(queueFamilyIndex != ~0u);
	vkGetDeviceQueue(s_Device->get(), queueFamilyIndex, 0u, reference());

	m_MainCommandBufferPool = create_shared<VulkanCommandBufferPool>(queueFamilyIndex, commandPoolCreateFlags);
}

void VulkanQueue::waitIdle()
{
	/// To wait on the host for the completion of outstanding queue operations for a given queue
	/// equivalent to submitting a fence to a queue and waiting with an infinite timeout for that fence to signal.
	if (isValid())
	{
		VERIFY_VK(vkQueueWaitIdle(get()));
	}
}

void VulkanQueue::submit(VulkanCommandBufferPtr cmdBuffer)
{
	//VkSubmitInfo submitInfo
	//{
	//	VK_STRUCTURE_TYPE_SUBMIT_INFO,
	//	nullptr,
	//	0u,
	//	nullptr,
	//	nullptr,
	//	1u,
	//	&cmdBuffer->Handle,
	//	0u,
	//	nullptr
	//};
	//GfxVerifyVk(vkQueueSubmit(Handle, 1u, &submitInfo, cmdBuffer->fence()->Handle));
	//VulkanAsyncPool::instance()->waitFence(cmdBuffer->fence());
}

VkQueues& VkQueues::instance()
{
	static VkQueues s_Instance;
	return s_Instance;
}

void VkQueues::initialize(uint32_t graphicsQueueFamilyIndex, uint32_t transferQueueFamilyIndex, uint32_t computeQueueFamilyIndex)
{
	m_GraphicsQueue = create_shared<VulkanQueue>(
		graphicsQueueFamilyIndex, 
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	m_TransferQueue = create_shared<VulkanQueue>( 
		transferQueueFamilyIndex, 
		VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);

	m_ComputeQueue = create_shared<VulkanQueue>(
		computeQueueFamilyIndex,
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
}

void VkQueues::queuePresent(VkSwapchainKHR swapchain, VkSemaphore presentCompleteSemaphore, uint32_t* imageIndices)
{
	assert(swapchain && presentCompleteSemaphore && imageIndices);

	submitTransferCommands();
	///m_ComputeQueue->submit(nullptr);
	m_GraphicsQueue->submit(nullptr);

	VkSemaphore complete = m_GraphicsQueue->executionsCompleteSemaphore();
	VkPresentInfoKHR presentInfo
	{
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		nullptr,
		1u,
		&complete,
		1u,
		&swapchain,
		imageIndices,
		nullptr
	};
	VERIFY_VK(vkQueuePresentKHR(m_GraphicsQueue->get(), &presentInfo));
}

void VkQueues::submitTransferCommands()
{
}

void VkQueues::finalize()
{
	///submit();

	while (!m_TransferCommands.empty())
	{
		m_TransferCommands.pop();
	}

	m_GraphicsQueue.reset();
	m_TransferQueue.reset();
	m_ComputeQueue.reset();
}

#if 0
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

NAMESPACE_END(Gfx)
