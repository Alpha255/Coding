#include "VulkanDescriptor.h"
#include "VulkanEngine.h"

void vkDescriptorPool::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyDescriptorPool(*device, **this, vkMemoryAllocator);
		reset();
	}
}

void vkDescriptorSetLayout::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyDescriptorSetLayout(*device, **this, vkMemoryAllocator);
		reset();
	}
}

void vkDescriptorUpdateTemplate::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyDescriptorUpdateTemplate(*device, **this, vkMemoryAllocator);
		reset();
	}
}
