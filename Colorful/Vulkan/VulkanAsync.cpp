#include "Colorful/Vulkan/VulkanEngine.h"

VulkanFence::VulkanFence(VkDevice device, bool8_t signaled)
{
	assert(!isValid());

	VkFenceCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0u
	};

	GfxVerifyVk(vkCreateFence(device, &createInfo, vkMemoryAllocator, &Handle));
}

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

VulkanFencePtr VulkanAsyncPool::allocFence(bool8_t signaled)
{
	if (m_FreeFences.size() > 0u)
	{
		auto fence = m_FreeFences.end();
		m_FreeFences.pop_back();
		return *fence;
	}

	auto fencePtr = std::make_shared<VulkanFence>(m_Device, signaled);
	m_Fences.emplace(std::make_pair(fencePtr->Handle, fencePtr));
	return fencePtr;
}

void VulkanAsyncPool::freeFence(const VulkanFencePtr& fence)
{
	auto it = m_Fences.find(fence->Handle);
	assert(it != m_Fences.end());

	m_FreeFences.emplace_back(it->second);
	m_Fences.erase(it);
}

void VulkanAsyncPool::resetFence(const VulkanFencePtr& fence) const
{
	assert(fence && fence->isValid());
	GfxVerifyVk(vkResetFences(m_Device, 1u, &fence->Handle));
}

void VulkanAsyncPool::waitFence(const VulkanFencePtr& fence, uint64_t timeoutInNanosecond) const
{
	assert(fence && fence->isValid());
	GfxVerifyVk(vkWaitForFences(m_Device, 1u, &fence->Handle, true, timeoutInNanosecond));
	GfxVerifyVk(vkResetFences(m_Device, 1u, &fence->Handle));
}

VulkanFence::eFenceState VulkanAsyncPool::fenceState(const VulkanFencePtr& fence) const
{
	assert(fence && fence->isValid());
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

VulkanSemaphorePtr VulkanAsyncPool::allocSemaphore()
{
	if (m_FreeSemaphores.size() > 0u)
	{
		auto semaphore = m_FreeSemaphores.end();
		m_FreeSemaphores.pop_back();
		return *semaphore;
	}

	auto semaphorePtr = std::make_shared<VulkanSemaphore>(m_Device);
	m_Semaphores.emplace(std::make_pair(semaphorePtr->Handle, semaphorePtr));
	return semaphorePtr;
}

void VulkanAsyncPool::freeSemaphore(const VulkanSemaphorePtr& semaphore)
{
	auto it = m_Semaphores.find(semaphore->Handle);
	assert(it != m_Semaphores.end());

	m_FreeSemaphores.emplace_back(it->second);
	m_Semaphores.erase(it);
}

VulkanEventPtr VulkanAsyncPool::allocEvent()
{
	if (m_FreeEvents.size() > 0u)
	{
		auto event = m_FreeEvents.end();
		m_FreeEvents.pop_back();
		return *event;
	}

	auto eventPtr = std::make_shared<VulkanEvent>(m_Device);
	m_Events.emplace(std::make_pair(eventPtr->Handle, eventPtr));
	return eventPtr;
}

void VulkanAsyncPool::freeEvent(const VulkanEventPtr& event)
{
	auto it = m_Events.find(event->Handle);
	assert(it != m_Events.end());

	m_FreeEvents.emplace_back(it->second);
	m_Events.erase(it);
}

VulkanEvent::eEventState VulkanAsyncPool::eventState(const VulkanEventPtr& event) const
{
	assert(event && event->isValid());

	VkResult result = vkGetEventStatus(m_Device, event->Handle);
	VulkanEvent::eEventState state = VulkanEvent::eEventStatus_MaxEnum;
	switch (result)
	{
	case VK_EVENT_SET:
		state = VulkanEvent::eSignaled;
		break;
	case VK_EVENT_RESET:
		state = VulkanEvent::eUnsignaled;
		break;
	default:
		assert(0);
		break;
	}

	return state;
}

void VulkanAsyncPool::setEventState(const VulkanEventPtr& event, VulkanEvent::eEventState state) const
{
	assert(event && event->isValid() && state < VulkanEvent::eEventStatus_MaxEnum);

	if (VulkanEvent::eSignaled == state)
	{
		/// Set the state of an event to signaled from the host
		GfxVerifyVk(vkSetEvent(m_Device, event->Handle));
	}
	else if (VulkanEvent::eUnsignaled == state)
	{
		/// Set the state of an event to unsignaled from the host
		GfxVerifyVk(vkResetEvent(m_Device, event->Handle));
	}
}

template<class TMap, class TVector> void free(TMap& map, TVector& vector, VkDevice device)
{
	for (auto it = vector.begin(); it != vector.end(); ++it)
	{
		(*it)->destroy(device);
	}

	for (auto it = map.begin(); it != map.end(); ++it)
	{
		it->second->destroy(device);
	}

	vector.clear();
	map.clear();
}

void VulkanAsyncPool::cleanup()
{
	/// All queue submission commands that refer to fence must have completed execution
	free(m_Fences, m_FreeFences, m_Device);
	free(m_Semaphores, m_FreeSemaphores, m_Device);
	free(m_Events, m_FreeEvents, m_Device);
}
