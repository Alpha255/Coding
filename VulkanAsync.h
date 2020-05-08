#pragma once

#include "VulkanLoader.h"

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

class VulkanFence : public VulkanObject<VkFence>
{
public:
	enum eFenceStatus
	{
		eUnsignaled,
		eSignaled,
		eFenceStatus_MaxEnum
	};

	VulkanFence(uint32_t ID, VkFence handle)
		: m_ID(ID)
	{
		Handle = handle;
	}

	inline uint32_t ID() const
	{
		return m_ID;
	}
protected:
private:
	uint32_t m_ID = 0u;
};
using VulkanFencePtr = std::shared_ptr<VulkanFence>;

class VulkanFenceManager : public LazySingleton<VulkanFenceManager>
{
	lazySingletonDeclare(VulkanFenceManager);
public:
	VulkanFencePtr allocFence(bool8_t signaled);
	void freeFence(VulkanFencePtr& fence);

	void resetFence(VulkanFencePtr& fence);
	void waitFence(VulkanFencePtr& fence, uint32_t timeout);
	VulkanFence::eFenceStatus fenceStatus(const VulkanFencePtr& fence);

	void cleanup() override final;
protected:
	VulkanFenceManager(const VkDevice device)
		: m_Device(device)
	{
		assert(device != VK_NULL_HANDLE);
	}
private:
	const VkDevice m_Device;
	uint32_t m_FenceID = 0u;
	std::unordered_map<uint32_t, VulkanFencePtr> m_Fences;
	std::vector<VulkanFencePtr> m_FreeFences;
};

class vkSemaphore : public VulkanDeviceObject<VkSemaphore>
{
public:
	vkSemaphore(const class VulkanDevice &device);
	void destroy(const class VulkanDevice &device) override final;
};

class vkEvent : public VulkanDeviceObject<VkEvent>
{
public:
	enum eEventStatus
	{
		eUnsignaled,
		eSignaled,
		eEventStatus_MaxEnum
	};

	vkEvent(const class VulkanDevice &device);
	void destroy(const class VulkanDevice &device) override final;
	eEventStatus getStatus(const class VulkanDevice &device);
	void setStatus(const class VulkanDevice &device, eEventStatus status);
};
