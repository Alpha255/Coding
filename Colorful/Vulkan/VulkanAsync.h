#pragma once

#include "VulkanLoader.h"

/*****************
	Fences
		Fences can be used to communicate to the host that execution of some task on the device has completed.

	Semaphores
		Semaphores can be used to control resource access across multiple queues.

	Events
		Events provide a fine-grained synchronization primitive which can be signaled either within a command buffer or by the host, and can be waited upon within a command buffer or queried on the host.

	Pipeline Barriers
		Pipeline barriers also provide synchronization control within a command buffer, but at a single point, rather than with separate signal and wait operations.

	Render Passes
		Render passes provide a useful synchronization framework for most rendering tasks, built upon the concepts in this chapter. 
		Many cases that would otherwise need an application to use other synchronization primitives can be expressed more efficiently as part of a render pass.
********************/

class vkFence : public vkDeviceObject<VkFence>
{
public:
	enum eFenceState
	{
		eUnsignaled,
		eSignaled,
		eFenceState_MaxEnum
	};

	vkFence(const class vkDevice &device, eFenceState state);
	void destroy(const class vkDevice &device) override final;
};

class vkSemaphore : public vkDeviceObject<VkSemaphore>
{
public:
	vkSemaphore(const class vkDevice &device);
	void destroy(const class vkDevice &device) override final;
};

class vkEvent : public vkDeviceObject<VkEvent>
{
public:
	vkEvent(const class vkDevice &device);
	void destroy(const class vkDevice &device) override final;
};
