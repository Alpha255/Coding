#include "VulkanQueue.h"
#include "VulkanEngine.h"
#include "VulkanSwapChain.h"
#include "VulkanAsync.h"

void vkDeviceQueue::create(uint32_t queueFamilyIndex, const VulkanDevice &device)
{
	assert(!isValid() && device.isValid());

	/// vkGetDeviceQueue must only be used to get queues that were created with the flags parameter of VkDeviceQueueCreateInfo set to zero. 
	/// To get queues that were created with a non-zero flags parameter use vkGetDeviceQueue2.
	m_FamilyIndex = queueFamilyIndex;
	vkGetDeviceQueue(device.Handle, queueFamilyIndex, 0u, &Handle);

	/// All queues associated with a logical device are destroyed when vkDestroyDevice is called on that device
	m_RenderCompleteSemaphore = device.createSemaphore();
}

void vkDeviceQueue::submit(vkCommandBuffer &cmdBuffer)
{
	VkSubmitInfo submitInfo
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		0u,
		nullptr,
		nullptr,
		1u,
		&cmdBuffer.Handle,
		0u,
		nullptr
	};
	rVerifyVk(vkQueueSubmit(Handle, 1u, &submitInfo, cmdBuffer.getFence()->Handle));
	cmdBuffer.waitFence(vkEngine::instance().getDevice());
}

void vkDeviceQueue::present(
	vkCommandBuffer &cmdBuffer,
	const vkSwapchain &swapchain,
	VkFence fence)
{
	VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo submitInfo
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		1u,
		&swapchain.getPresentCompleteSemaphore()->Handle,
		&waitStageMask,
		1u,
		&cmdBuffer.Handle,
		1u,
		&m_RenderCompleteSemaphore->Handle
	};

	/// Submission can be a high overhead operation, and applications should attempt to batch work together into as few calls to vkQueueSubmit as possible.
	/// vkQueueSubmit is a queue submission command, with each batch defined by an element of pSubmits. 
	/// Batches begin execution in the order they appear in pSubmits, but may complete out of order.
	rVerifyVk(vkQueueSubmit(Handle, 1u, &submitInfo, fence));
	cmdBuffer.setState(vkCommandBuffer::ePending);
	swapchain.present(*this, *m_RenderCompleteSemaphore);
}

void vkDeviceQueue::queueCommandBuffer(vkCommandBuffer *cmdBuffer)
{
	bool8_t contain = false;
	for (uint32_t i = 0u; i < m_QueuedCmdBuffers.size(); ++i)
	{
		if (m_QueuedCmdBuffers[i] == cmdBuffer)
		{
			contain = true;
			break;
		}
	}

	if (!contain)
	{
		m_QueuedCmdBuffers.emplace_back(cmdBuffer);
	}
}

void vkDeviceQueue::submit(const vkSwapchain &swapchain)
{
	assert(m_QueuedCmdBuffers.size() > 0u);

	VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkFence fence = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> submitCmdBuffers;
	for (uint32_t i = 0u; i < m_QueuedCmdBuffers.size(); ++i)
	{
		submitCmdBuffers.emplace_back(m_QueuedCmdBuffers[i]->Handle);

		if (m_QueuedCmdBuffers[i]->isInsideRenderPass())
		{
			m_QueuedCmdBuffers[i]->endRenderPass();
		}
		fence = m_QueuedCmdBuffers[i]->getFence()->Handle;
		m_QueuedCmdBuffers[i]->setState(vkCommandBuffer::eExecutable);
	}

	VkSubmitInfo submitInfo
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		1u,
		&swapchain.getPresentCompleteSemaphore()->Handle,
		&waitStageMask,
		(uint32_t)submitCmdBuffers.size(),
		submitCmdBuffers.data(),
		1u,
		&m_RenderCompleteSemaphore->Handle
	};

	/// Submission can be a high overhead operation, and applications should attempt to batch work together into as few calls to vkQueueSubmit as possible.
	/// vkQueueSubmit is a queue submission command, with each batch defined by an element of pSubmits. 
	/// Batches begin execution in the order they appear in pSubmits, but may complete out of order.
	rVerifyVk(vkQueueSubmit(Handle, 1u, &submitInfo, fence));
	swapchain.present(*this, *m_RenderCompleteSemaphore);
}

void vkDeviceQueue::waitIdle()
{
	/// To wait on the host for the completion of outstanding queue operations for a given queue
	/// equivalent to submitting a fence to a queue and waiting with an infinite timeout for that fence to signal.
	if (isValid())
	{
		rVerifyVk(vkQueueWaitIdle(Handle));
	}
}

void vkDeviceQueue::destroy(const VulkanDevice &device)
{
	m_RenderCompleteSemaphore->destroy(device);
}
