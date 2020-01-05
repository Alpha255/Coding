#include "VulkanLayout.h"
#include "VulkanEngine.h"

void vkDescriptorSetLayout::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyDescriptorSetLayout(*device, **this, vkMemoryAllocator);
		reset();
	}
}
