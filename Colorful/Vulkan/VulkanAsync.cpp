#include "VulkanAsync.h"
#include "VulkanEngine.h"

vkFence::vkFence(const vkDevice &device, eFenceState state)
{
	assert(!isValid() && device.isValid());

	VkFenceCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		eUnsignaled == state ? 0u : VK_FENCE_CREATE_SIGNALED_BIT
	};

	VkFence handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateFence(*device, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);
}

void vkFence::destory(const vkDevice &device)
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

void vkSemaphore::destory(const vkDevice &device)
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

void vkEvent::destory(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyEvent(*device, **this, vkMemoryAllocator);
		reset();
	}
}

