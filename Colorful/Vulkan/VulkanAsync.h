#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

/*****************
	Fences
		Fences can be used to communicate to the host that execution of some task on the device has completed.
		Fences are a synchronization primitive that can be used to insert a dependency from a queue to the host.

	Semaphores
		Semaphores can be used to control resource access across multiple queues.
		Semaphores are a synchronization primitive that can be used to insert a dependency between batches submitted to queues.

	Events
		Events provide a fine-grained synchronization primitive which can be signaled either within a command buffer or by the host, and can be waited upon within a command buffer or queried on the host.
		Events are a synchronization primitive that can be used to insert a fine-grained dependency between commands submitted to the same queue, 
		or between the host and a queue. Events must not be used to insert a dependency between commands submitted to different queues.

	Pipeline Barriers
		Pipeline barriers also provide synchronization control within a command buffer, but at a single point, rather than with separate signal and wait operations.
		vkCmdPipelineBarrier is a synchronization command that inserts a dependency between commands submitted to the same queue, or between commands in the same subpass.

	Memory Barriers
		Memory barriers are used to explicitly control access to buffer and image subresource ranges. 
		Memory barriers are used to transfer ownership between queue families, change image layouts, and define availability and visibility operations. 
		They explicitly define the access types and buffer and image subresource ranges that are included in the access scopes of a memory dependency that is created by a 
		synchronization command that includes them.
		VkMemoryBarrier/VkBufferMemoryBarrier/VkImageMemoryBarrier 

	Render Passes
		Render passes provide a useful synchronization framework for most rendering tasks, built upon the concepts in this chapter. 
		Many cases that would otherwise need an application to use other synchronization primitives can be expressed more efficiently as part of a render pass.
********************/

NAMESPACE_START(Gfx)

DECLARE_SHARED_PTR(VulkanFence)
class VulkanFence final : public VkObject<VkFence_T>, public VkDeviceResource
{
public:
	enum class EState
	{
		Unsignaled,
		Signaled
	};

	VulkanFence(VkDevice device, bool8_t signaled = false);

	void destroy(VkDevice device) override final
	{
		assert(device);
		vkDestroyFence(device, get(), VK_MEMORY_ALLOCATOR);
	}
protected:
private:
};

DECLARE_SHARED_PTR(VulkanSemaphore)
class VulkanSemaphore final : public VkObject<VkSemaphore_T>, public VkDeviceResource
{
public:
	VulkanSemaphore(VkDevice device);

	void destroy(VkDevice device) override final
	{
		assert(device);
		vkDestroySemaphore(device, get(), VK_MEMORY_ALLOCATOR);
	}
};

DECLARE_SHARED_PTR(VulkanEvent)
class VulkanEvent final : public VkObject<VkEvent_T>, public VkDeviceResource
{
public:
	enum class EState
	{
		Unsignaled,
		Signaled
	};

	VulkanEvent(VkDevice device);

	void destroy(VkDevice device) override final
	{
		assert(device);
		vkDestroyEvent(device, get(), VK_MEMORY_ALLOCATOR);
	}
};

NAMESPACE_END(Gfx)
