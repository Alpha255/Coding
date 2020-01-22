#include "VulkanRenderpass.h"
#include "VulkanEngine.h"

/*************************************************
	When defining a memory dependency, using only VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT or VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT would never 
	make any accesses available and/or visible because these stages do not access memory.

	VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT and VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT are useful for accomplishing layout transitions and queue 
	ownership operations when the required execution dependency is satisfied by other means - for example, semaphore operations between queues.
**************************************************/

void vkRenderPass::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyRenderPass(*device, **this, vkMemoryAllocator);
		reset();
	}
}
