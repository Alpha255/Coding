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

class VulkanFence : public VulkanDeviceObject<VkFence>
{
public:
	enum eFenceState
	{
		eUnsignaled,
		eSignaled,
		eFenceState_MaxEnum
	};

	VulkanFence(VkDevice device, bool8_t signaled);

	void destroy(VkDevice device) override final
	{
		if (isValid())
		{
			vkDestroyFence(device, Handle, vkMemoryAllocator);
			Handle = VK_NULL_HANDLE;
		}
	}
protected:
private:
};
using VulkanFencePtr = std::shared_ptr<VulkanFence>;

class VulkanSemaphore : public VulkanDeviceObject<VkSemaphore>
{
public:
	VulkanSemaphore(VkDevice device);

	void destroy(VkDevice device) override final
	{
		/// All submitted batches that refer to semaphore must have completed execution
		if (isValid())
		{
			vkDestroySemaphore(device, Handle, vkMemoryAllocator);
			Handle = VK_NULL_HANDLE;
		}
	}
};
using VulkanSemaphorePtr = std::shared_ptr<VulkanSemaphore>;

class VulkanEvent : public VulkanDeviceObject<VkEvent>
{
public:
	enum eEventState
	{
		eUnsignaled,
		eSignaled,
		eEventStatus_MaxEnum
	};

	VulkanEvent(VkDevice device);

	void destroy(VkDevice device) override final
	{
		/// All submitted commands that refer to event must have completed execution
		if (isValid())
		{
			vkDestroyEvent(device, Handle, vkMemoryAllocator);
			Handle = VK_NULL_HANDLE;
		}
	}
};
using VulkanEventPtr = std::shared_ptr<VulkanEvent>;

class VulkanAsyncPool : public LazySingleton<VulkanAsyncPool>
{
	lazySingletonDeclare(VulkanAsyncPool);
public:
	VulkanFencePtr allocFence(bool8_t signaled);
	void freeFence(const VulkanFencePtr& fence);
	void resetFence(const VulkanFencePtr& fence) const;
	void waitFence(const VulkanFencePtr& fence, uint64_t timeoutInNanosecond = std::numeric_limits<uint64_t>().max()) const;
	VulkanFence::eFenceState fenceState(const VulkanFencePtr& fence) const;

	VulkanSemaphorePtr allocSemaphore();
	void freeSemaphore(const VulkanSemaphorePtr& semaphore);

	VulkanEventPtr allocEvent();
	void freeEvent(const VulkanEventPtr& event);
	VulkanEvent::eEventState eventState(const VulkanEventPtr& event) const;
	void setEventState(const VulkanEventPtr& event, VulkanEvent::eEventState state) const;

	void cleanup() override final;
protected:
	VulkanAsyncPool(const VkDevice device)
		: m_Device(device)
	{
		assert(device != VK_NULL_HANDLE);
	}
private:
	const VkDevice m_Device;
	std::unordered_map<VkFence, VulkanFencePtr> m_Fences;
	std::unordered_map<VkSemaphore, VulkanSemaphorePtr> m_Semaphores;
	std::unordered_map<VkEvent, VulkanEventPtr> m_Events;

	std::vector<VulkanFencePtr> m_FreeFences;
	std::vector<VulkanSemaphorePtr> m_FreeSemaphores;
	std::vector<VulkanEventPtr> m_FreeEvents;
};
