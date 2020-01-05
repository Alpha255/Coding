#include "VulkanPipeline.h"
#include "VulkanEngine.h"

void vkPipelineLayout::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyPipelineLayout(*device, **this, vkMemoryAllocator);
		reset();
	}
}

void vkPipelineCache::create(const vkDevice &device)
{
	assert(device.isValid() && !isValid());

	VkPipelineCacheCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
		nullptr,
		0u,   /// flags is reserved for future use
		0u,
		nullptr
	};

	VkPipelineCache handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreatePipelineCache(*device, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);
}

void vkPipelineCache::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyPipelineCache(*device, **this, vkMemoryAllocator);
		reset();
	}
}

void vkPipeline::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyPipeline(*device, **this, vkMemoryAllocator);
		reset();
	}
}
