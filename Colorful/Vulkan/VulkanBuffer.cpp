#include "VulkanBuffer.h"
#include "VulkanEngine.h"

void vkDeviceMemory::create(const vkDevice &device, eRBufferUsage usage, uint32_t memoryTypeBits, size_t size)
{
	assert(!isValid() && device.isValid());

	VkMemoryAllocateInfo allocateInfo
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		nullptr,
		size,
		device.getMemoryTypeIndex(usage, memoryTypeBits)
	};

	VkDeviceMemory handle = VK_NULL_HANDLE;
	rVerifyVk(vkAllocateMemory(*device, &allocateInfo, vkMemoryAllocator, &handle));
	reset(handle);
}

void vkDeviceMemory::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkFreeMemory(*device, **this, vkMemoryAllocator);
	}
}

void vkBuffer::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		m_Memory.destroy(device);
		vkDestroyBuffer(*device, **this, vkMemoryAllocator);
		reset();
	}
}

void vkStagingBuffer::create(const vkDevice &device, VkBufferUsageFlagBits usageFlagBits, size_t size)
{
	assert(!isValid() && device.isValid());
	assert(usageFlagBits == VK_BUFFER_USAGE_TRANSFER_SRC_BIT || usageFlagBits == VK_BUFFER_USAGE_TRANSFER_DST_BIT);

	VkBufferCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		nullptr,
		0u,
		size,
		usageFlagBits,
		VK_SHARING_MODE_EXCLUSIVE,
		0u,
		nullptr
	};

	VkBuffer handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateBuffer(*device, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);
}

vkGpuBuffer::vkGpuBuffer(const vkDevice &device, eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void *pData)
{
	assert(!isValid() && device.isValid());
}
