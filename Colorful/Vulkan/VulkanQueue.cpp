#include "VulkanQueue.h"
#include "VulkanEngine.h"
#include "VulkanSwapChain.h"
#include "VulkanAsync.h"

void vkDeviceQueue::create(uint32_t queueFamilyIndex, const vkDevice &device)
{
	assert(!isValid() && device.isValid());

	/// vkGetDeviceQueue must only be used to get queues that were created with the flags parameter of VkDeviceQueueCreateInfo set to zero. 
	/// To get queues that were created with a non-zero flags parameter use vkGetDeviceQueue2.
	m_FamilyIndex = queueFamilyIndex;
	VkQueue handle = VK_NULL_HANDLE;
	vkGetDeviceQueue(*device, queueFamilyIndex, 0u, &handle);
	reset(handle);

	/// All queues associated with a logical device are destroyed when vkDestroyDevice is called on that device
	m_RenderCompleteSemaphore = device.createSemaphore();
}

void vkDeviceQueue::submit(vkCommandBuffer &cmdBuffer)
{
	VkCommandBuffer cmdBufferHandle = *cmdBuffer;
	VkSubmitInfo submitInfo
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		0u,
		nullptr,
		nullptr,
		1u,
		&cmdBufferHandle,
		0u,
		nullptr
	};
	rVerifyVk(vkQueueSubmit(**this, 1u, &submitInfo, **cmdBuffer.getFence()));
	cmdBuffer.waitFence(vkEngine::instance().getDevice());
}

void vkDeviceQueue::present(
	vkCommandBuffer &cmdBuffer,
	const vkSwapchain &swapchain,
	VkFence fence)
{
	VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkCommandBuffer cmdBufferHandle = *cmdBuffer;
	VkSemaphore presentCompleteSemaphore = **(swapchain.getPresentCompleteSemaphore());
	VkSemaphore renderCompleteSemaphore = **m_RenderCompleteSemaphore;
	VkSubmitInfo submitInfo
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		1u,
		&presentCompleteSemaphore,
		&waitStageMask,
		1u,
		&cmdBufferHandle,
		1u,
		&renderCompleteSemaphore
	};

	/// Submission can be a high overhead operation, and applications should attempt to batch work together into as few calls to vkQueueSubmit as possible.
	/// vkQueueSubmit is a queue submission command, with each batch defined by an element of pSubmits. 
	/// Batches begin execution in the order they appear in pSubmits, but may complete out of order.
	rVerifyVk(vkQueueSubmit(**this, 1u, &submitInfo, fence));
	cmdBuffer.setState(vkCommandBuffer::ePending);
	swapchain.present(*this, *m_RenderCompleteSemaphore);
}

void vkDeviceQueue::waitIdle()
{
	/// To wait on the host for the completion of outstanding queue operations for a given queue
	/// equivalent to submitting a fence to a queue and waiting with an infinite timeout for that fence to signal.
	if (isValid())
	{
		rVerifyVk(vkQueueWaitIdle(**this));
	}
}

void vkDeviceQueue::destroy(const vkDevice &device)
{
	m_RenderCompleteSemaphore->destroy(device);
}
