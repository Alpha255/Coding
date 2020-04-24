#include "VulkanQueue.h"
#include "VulkanEngine.h"
#include "VulkanSwapChain.h"

void vkDeviceQueue::create(uint32_t queueFamilyIndex, const class vkDevice &device)
{
	assert(!isValid() && device.isValid());

	/// vkGetDeviceQueue must only be used to get queues that were created with the flags parameter of VkDeviceQueueCreateInfo set to zero. 
	/// To get queues that were created with a non-zero flags parameter use vkGetDeviceQueue2.
	m_FamilyIndex = queueFamilyIndex;
	VkQueue handle = VK_NULL_HANDLE;
	vkGetDeviceQueue(*device, queueFamilyIndex, 0u, &handle);
	reset(handle);

	/// All queues associated with a logical device are destroyed when vkDestroyDevice is called on that device
}

void vkDeviceQueue::submit(const vkCommandBuffer &cmdBuffer, const vkSemaphore *waitSemaphores, uint32_t waitSemaphoreCount)
{
}

void vkDeviceQueue::present(const vkSwapchain &swapchain, const vkSemaphore *waitSemaphores, uint32_t waitSemaphoreCount)
{
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