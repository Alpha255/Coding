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
class VulkanFence : public VkObject<VkFence_T>
{
public:
	enum class EState : uint8_t
	{
		Unsignaled,
		Signaled
	};

	VulkanFence(VkDevice device, bool8_t signaled = false);
protected:
private:
};

DECLARE_SHARED_PTR(VulkanSemaphore)
class VulkanSemaphore : public VkObject<VkSemaphore_T>
{
public:
	VulkanSemaphore(VkDevice device);
};

DECLARE_SHARED_PTR(VulkanEvent)
class VulkanEvent : public VkObject<VkEvent_T>
{
public:
	enum class EState : uint8_t
	{
		Unsignaled,
		Signaled
	};

	VulkanEvent(VkDevice device);
};

//class VulkanAsyncPool : public LazySingleton<VulkanAsyncPool>
//{
//	lazySingletonDeclare(VulkanAsyncPool);
//public:
//	VulkanFencePtr allocFence(bool8_t signaled = false);
//	void freeFence(const VulkanFencePtr& fence);
//	void resetFence(const VulkanFencePtr& fence) const;
//	void waitFence(const VulkanFencePtr& fence, uint64_t timeoutInNanosecond = std::numeric_limits<uint64_t>().max()) const;
//	VulkanFence::eFenceState fenceState(const VulkanFencePtr& fence) const;
//
//	VulkanSemaphorePtr allocSemaphore();
//	void freeSemaphore(const VulkanSemaphorePtr& semaphore);
//
//	VulkanEventPtr allocEvent();
//	void freeEvent(const VulkanEventPtr& event);
//	VulkanEvent::eEventState eventState(const VulkanEventPtr& event) const;
//	void setEventState(const VulkanEventPtr& event, VulkanEvent::eEventState state) const;
//
//	void cleanup() override final;
//protected:
//	VulkanAsyncPool(const VkDevice device)
//		: m_Device(device)
//	{
//		assert(device != VK_NULL_HANDLE);
//	}
//private:
//	const VkDevice m_Device;
//	std::unordered_map<VkFence, VulkanFencePtr> m_Fences;
//	std::unordered_map<VkSemaphore, VulkanSemaphorePtr> m_Semaphores;
//	std::unordered_map<VkEvent, VulkanEventPtr> m_Events;
//
//	std::vector<VulkanFencePtr> m_FreeFences;
//	std::vector<VulkanSemaphorePtr> m_FreeSemaphores;
//	std::vector<VulkanEventPtr> m_FreeEvents;
//};

NAMESPACE_END(Gfx)
