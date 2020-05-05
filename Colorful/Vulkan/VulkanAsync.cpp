#include "VulkanAsync.h"
#include "VulkanEngine.h"

vkFence::vkFence(const VulkanDevice &device, eFenceStatus status)
{
	assert(!isValid() && device.isValid() && status < eFenceStatus_MaxEnum);

	/// To create a fence whose payload can be exported to external handles, 
	/// add a VkExportFenceCreateInfo structure to the pNext chain of the VkFenceCreateInfo structure.

	/// To specify additional attributes of NT handles exported from a fence, 
	/// add a VkExportFenceWin32HandleInfoKHR structure to the pNext chain of the VkFenceCreateInfo structure.

	VkFenceCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		eUnsignaled == status ? 0u : VK_FENCE_CREATE_SIGNALED_BIT
	};

	rVerifyVk(vkCreateFence(device.Handle, &createInfo, vkMemoryAllocator, &Handle));
}

void vkFence::resetStatus(const VulkanDevice &device)
{
	/// Resets the fence to the unsignaled state.
	/// If any member of pFences is already in the unsignaled state when vkResetFences is executed, then vkResetFences has no effect on that fence.
	assert(device.isValid() && isValid());
	rVerifyVk(vkResetFences(device.Handle, 1u, &Handle));
}

vkFence::eFenceStatus vkFence::getStatus(const VulkanDevice &device)
{
	assert(device.isValid() && isValid());
	VkResult result = vkGetFenceStatus(device.Handle, Handle);

	/// If a queue submission command is pending execution, then the value returned by this command may immediately be out of date.
	/// If the device has been lost, vkGetFenceStatus may return any of the (VK_SUCCESS|VK_NOT_READY|VK_ERROR_DEVICE_LOST). 
	/// If the device has been lost and vkGetFenceStatus is called repeatedly, it will eventually return either VK_SUCCESS or VK_ERROR_DEVICE_LOST.

	eFenceStatus status = eFenceStatus_MaxEnum;
	switch (result)
	{
	case VK_SUCCESS:
		status = eSignaled;
		break;
	case VK_NOT_READY:
		status = eUnsignaled;
		break;
	case VK_ERROR_DEVICE_LOST:
	default:
		assert(0);
		break;
	}

	return status;
}

void vkFence::destroy(const VulkanDevice &device)
{
	/// All queue submission commands that refer to fence must have completed execution

	assert(device.isValid());

	if (isValid())
	{
		vkDestroyFence(device.Handle, Handle, vkMemoryAllocator);
		Handle = VK_NULL_HANDLE;
	}
}

vkSemaphore::vkSemaphore(const VulkanDevice &device)
{
	/// Semaphores have two states - signaled and unsignaled. The state of a semaphore can be signaled after execution of a batch of commands is completed. 
	/// A batch can wait for a semaphore to become signaled before it begins execution, and the semaphore is also unsignaled before the batch begins execution.
	assert(!isValid() && device.isValid());

	VkSemaphoreCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		nullptr,
		0u  /// flags is reserved for future use
	};

	rVerifyVk(vkCreateSemaphore(device.Handle, &createInfo, vkMemoryAllocator, &Handle));

	/// When a batch is submitted to a queue via a queue submission, and it includes semaphores to be signaled, 
	/// it defines a memory dependency on the batch, and defines semaphore signal operations which set the semaphores to the signaled state.

	/// Before waiting on a semaphore, the application must ensure the semaphore is in a valid state for a wait operation. Specifically, 
	/// when a semaphore wait and unsignal operation is submitted to a queue:
	/// 1. The semaphore must be signaled, or have an associated semaphore signal operation that is pending execution.
	/// 2. There must be no other queue waiting on the same semaphore when the operation executes.
}

void vkSemaphore::destroy(const VulkanDevice &device)
{
	/// All submitted batches that refer to semaphore must have completed execution
	assert(device.isValid());

	if (isValid())
	{
		vkDestroySemaphore(device.Handle, Handle, vkMemoryAllocator);
		Handle = VK_NULL_HANDLE;
	}
}

vkEvent::vkEvent(const VulkanDevice &device)
{
	/// An application can signal an event, or unsignal it, on either the host or the device. 
	/// A device can wait for an event to become signaled before executing further operations. 
	/// No command exists to wait for an event to become signaled on the host.
	assert(!isValid() && device.isValid());

	VkEventCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		nullptr,
		0u  /// flags is reserved for future use
	};

	rVerifyVk(vkCreateEvent(device.Handle, &createInfo, vkMemoryAllocator, &Handle));

	/// The state of an event can also be updated on the device by commands inserted in command buffers.
	/// To set the state of an event to signaled from a device, call: vkCmdSetEvent
}

vkEvent::eEventStatus vkEvent::getStatus(const VulkanDevice &device)
{
	assert(!isValid() && device.isValid());

	VkResult result = vkGetEventStatus(device.Handle, Handle);
	eEventStatus status = eEventStatus_MaxEnum;
	switch (result)
	{
	case VK_EVENT_SET:
		status = eSignaled;
		break;
	case VK_EVENT_RESET:
		status = eUnsignaled;
		break;
	default:
		assert(0);
		break;
	}

	return status;
}

void vkEvent::setStatus(const VulkanDevice &device, eEventStatus status)
{
	assert(!isValid() && device.isValid() && status < eEventStatus_MaxEnum);

	if (eSignaled == status)
	{
		/// Set the state of an event to signaled from the host
		rVerifyVk(vkSetEvent(device.Handle, Handle));
	}
	else if (eUnsignaled == status)
	{
		/// Set the state of an event to unsignaled from the host
		rVerifyVk(vkResetEvent(device.Handle, Handle));
	}
}

void vkEvent::destroy(const VulkanDevice &device)
{
	/// All submitted commands that refer to event must have completed execution
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyEvent(device.Handle, Handle, vkMemoryAllocator);
		Handle = VK_NULL_HANDLE;
	}
}

