#include "VulkanAsync.h"
#include "VulkanEngine.h"

VulkanSemaphore::VulkanSemaphore(VkDevice device)
{
	/// Semaphores have two states - signaled and unsignaled. The state of a semaphore can be signaled after execution of a batch of commands is completed. 
	/// A batch can wait for a semaphore to become signaled before it begins execution, and the semaphore is also unsignaled before the batch begins execution.
	assert(!isValid());

	VkSemaphoreCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		nullptr,
		0u  /// flags is reserved for future use
	};

	GfxVerifyVk(vkCreateSemaphore(device, &createInfo, vkMemoryAllocator, &Handle));

	/// When a batch is submitted to a queue via a queue submission, and it includes semaphores to be signaled, 
	/// it defines a memory dependency on the batch, and defines semaphore signal operations which set the semaphores to the signaled state.

	/// Before waiting on a semaphore, the application must ensure the semaphore is in a valid state for a wait operation. Specifically, 
	/// when a semaphore wait and unsignal operation is submitted to a queue:
	/// 1. The semaphore must be signaled, or have an associated semaphore signal operation that is pending execution.
	/// 2. There must be no other queue waiting on the same semaphore when the operation executes.
}

void VulkanSemaphore::destroy(VkDevice device)
{
	/// All submitted batches that refer to semaphore must have completed execution
	if (isValid())
	{
		vkDestroySemaphore(device, Handle, vkMemoryAllocator);
		Handle = VK_NULL_HANDLE;
	}
}

VulkanEvent::VulkanEvent(VkDevice device)
{
	/// An application can signal an event, or unsignal it, on either the host or the device. 
	/// A device can wait for an event to become signaled before executing further operations. 
	/// No command exists to wait for an event to become signaled on the host.
	assert(!isValid());

	VkEventCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		nullptr,
		0u  /// flags is reserved for future use
	};

	GfxVerifyVk(vkCreateEvent(device, &createInfo, vkMemoryAllocator, &Handle));

	/// The state of an event can also be updated on the device by commands inserted in command buffers.
	/// To set the state of an event to signaled from a device, call: vkCmdSetEvent
}

VulkanEvent::eEventStatus VulkanEvent::getStatus(VkDevice device)
{
	assert(!isValid());

	VkResult result = vkGetEventStatus(device, Handle);
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

void VulkanEvent::setStatus(VkDevice device, eEventStatus status)
{
	assert(!isValid() && status < eEventStatus_MaxEnum);

	if (eSignaled == status)
	{
		/// Set the state of an event to signaled from the host
		GfxVerifyVk(vkSetEvent(device, Handle));
	}
	else if (eUnsignaled == status)
	{
		/// Set the state of an event to unsignaled from the host
		GfxVerifyVk(vkResetEvent(device, Handle));
	}
}

void VulkanEvent::destroy(VkDevice device)
{
	/// All submitted commands that refer to event must have completed execution
	if (isValid())
	{
		vkDestroyEvent(device, Handle, vkMemoryAllocator);
		Handle = VK_NULL_HANDLE;
	}
}

VulkanFencePtr VulkanFencePool::allocFence(bool8_t signaled)
{
	if (m_FreeFences.size() > 0u)
	{
		auto fence = m_FreeFences.end();
		m_FreeFences.pop_back();
		return *fence;
	}

	VkFenceCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0u
	};

	VkFence fence = VK_NULL_HANDLE;
	GfxVerifyVk(vkCreateFence(m_Device, &createInfo, vkMemoryAllocator, &fence));
	auto fencePtr = std::make_shared<VulkanFence>(m_FenceID, fence);
	m_Fences.emplace(std::make_pair(m_FenceID, fencePtr));
	return fencePtr;
}

void VulkanFencePool::freeFence(VulkanFencePtr& fence)
{
	auto it = m_Fences.find(fence->ID());
	assert(it != m_Fences.end());

	m_FreeFences.emplace_back(it->second);
	m_Fences.erase(it->second->ID());
}

void VulkanFencePool::resetFence(VulkanFencePtr& fence)
{
	assert(fence);
	GfxVerifyVk(vkResetFences(m_Device, 1u, &fence->Handle));
}

void VulkanFencePool::waitFence(VulkanFencePtr& fence, uint64_t timeoutInNanosecond)
{
	assert(fence);

	GfxVerifyVk(vkWaitForFences(m_Device, 1u, &fence->Handle, true, timeoutInNanosecond));
	GfxVerifyVk(vkResetFences(m_Device, 1u, &fence->Handle));
}

VulkanFence::eFenceState VulkanFencePool::fenceState(const VulkanFencePtr& fence)
{
	assert(fence);
	VkResult result = vkGetFenceStatus(m_Device, fence->Handle);

	/// If a queue submission command is pending execution, then the value returned by this command may immediately be out of date.
	/// If the device has been lost, vkGetFenceStatus may return any of the (VK_SUCCESS|VK_NOT_READY|VK_ERROR_DEVICE_LOST). 
	/// If the device has been lost and vkGetFenceStatus is called repeatedly, it will eventually return either VK_SUCCESS or VK_ERROR_DEVICE_LOST.

	VulkanFence::eFenceState state = VulkanFence::eFenceState_MaxEnum;
	switch (result)
	{
	case VK_SUCCESS:
		state = VulkanFence::eSignaled;
		break;
	case VK_NOT_READY:
		state = VulkanFence::eUnsignaled;
		break;
	case VK_ERROR_DEVICE_LOST:
	default:
		assert(0);
		break;
	}

	return state;
}

void VulkanFencePool::cleanup()
{
	/// All queue submission commands that refer to fence must have completed execution
	for (auto it = m_FreeFences.begin(); it != m_FreeFences.end(); ++it)
	{
		vkDestroyFence(m_Device, (*it)->Handle, vkMemoryAllocator);
	}

	for (auto it = m_Fences.begin(); it != m_Fences.end(); ++it)
	{
		vkDestroyFence(m_Device, it->second->Handle, vkMemoryAllocator);
	}

	m_FreeFences.clear();
	m_Fences.clear();
}
