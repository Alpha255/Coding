#include "VulkanRenderpass.h"
#include "VulkanEngine.h"

void vkRenderPass::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyRenderPass(*device, **this, vkMemoryAllocator);
		reset();
	}
}
