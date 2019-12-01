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
