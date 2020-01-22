#include "VulkanAsync.h"
#include "VulkanEngine.h"

vkFence::vkFence(const vkDevice &device, eFenceStatus status)
{
	assert(!isValid() && device.isValid());

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

	VkFence handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateFence(*device, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);
}

void vkFence::resetFence(const vkDevice &device)
{
	assert(device.isValid() && isValid());

	const VkFence fences[] =
	{
		**this
	};
	rVerifyVk(vkResetFences(*device, _countof(fences), fences));
}

vkFence::eFenceStatus vkFence::getStatus(const vkDevice &device)
{
	assert(device.isValid() && isValid());
	VkResult result = vkGetFenceStatus(*device, **this);

	/// If the device has been lost and vkGetFenceStatus is called repeatedly, it will eventually return either VK_SUCCESS or VK_ERROR_DEVICE_LOST.

	eFenceStatus status = eFenceState_MaxEnum;
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

void vkFence::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyFence(*device, **this, vkMemoryAllocator);
		reset();
	}
}

vkSemaphore::vkSemaphore(const vkDevice &device)
{
	assert(!isValid() && device.isValid());

	VkSemaphoreCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		nullptr,
		0u  /// flags is reserved for future use
	};

	VkSemaphore handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateSemaphore(*device, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);
}

void vkSemaphore::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroySemaphore(*device, **this, vkMemoryAllocator);
		reset();
	}
}

vkEvent::vkEvent(const vkDevice &device)
{
	assert(!isValid() && device.isValid());

	VkEventCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		nullptr,
		0u  /// flags is reserved for future use
	};

	VkEvent handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateEvent(*device, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);
}

void vkEvent::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyEvent(*device, **this, vkMemoryAllocator);
		reset();
	}
}

