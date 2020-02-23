#include "VulkanBuffer.h"
#include "VulkanEngine.h"

void vkDeviceMemory::create(const vkDevice &device, eRBufferUsage usage, const VkMemoryRequirements &memoryRequirements)
{
	assert(!isValid() && device.isValid());

	VkMemoryAllocateInfo allocateInfo
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		nullptr,
		memoryRequirements.size,
		device.getMemoryTypeIndex(usage, memoryRequirements.memoryTypeBits)
	};

	VkDeviceMemory handle = VK_NULL_HANDLE;
	rVerifyVk(vkAllocateMemory(*device, &allocateInfo, vkMemoryAllocator, &handle));
	reset(handle);

	/// If a memory object does not have the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT property, 
	/// then vkFlushMappedMemoryRanges must be called in order to guarantee that writes to the memory object from the host are made available to the host domain, 
	/// where they can be further made available to the device domain via a domain operation. Similarly, 
	/// vkInvalidateMappedMemoryRanges must be called to guarantee that writes which are available to the host domain are made visible to host operations.
	/// If the memory object does have the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT property flag, 
	/// writes to the memory object from the host are automatically made available to the host domain.Similarly, 
	/// writes made available to the host domain are automatically made visible to the host.
}

void vkDeviceMemory::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkFreeMemory(*device, **this, vkMemoryAllocator);
		reset();
	}
}

void vkDeviceMemory::update(const vkDevice &device, const void *pData, size_t size, size_t offset)
{
	assert(isValid());

	/// VkMemoryMapFlags is a bitmask type for setting a mask, but is currently reserved for future use.
	void *pGpuMemory = nullptr;
	rVerifyVk(vkMapMemory(*device, **this, offset, size, 0u, &pGpuMemory));
	verify(memcpy_s(pGpuMemory, size, pData, size) == 0);
	vkUnmapMemory(*device, **this);
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

vkStagingBuffer::vkStagingBuffer(const vkDevice &device, VkBufferUsageFlags usageFlagBits, size_t size, const void *pData)
{
	assert(!isValid() && device.isValid() && pData);
	assert(usageFlagBits == VK_BUFFER_USAGE_TRANSFER_SRC_BIT || usageFlagBits == VK_BUFFER_USAGE_TRANSFER_DST_BIT);

	/// VK_SHARING_MODE_EXCLUSIVE specifies that access to any range or image subresource of the object will be exclusive to a single queue family at a time
	/// VK_SHARING_MODE_CONCURRENT specifies that concurrent access to any range or image subresource of the object from multiple queue families is supported
	VkBufferCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		nullptr,
		0u,
		size,
		usageFlagBits,
		VK_SHARING_MODE_EXCLUSIVE,
		0u,
		nullptr  /// Ignored if sharingMode is not VK_SHARING_MODE_CONCURRENT
	};
	VkBuffer handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateBuffer(*device, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);

	VkMemoryRequirements memoryRequirements{};
	vkGetBufferMemoryRequirements(*device, handle, &memoryRequirements);
	m_Memory.create(device, eGpuReadCpuWrite, memoryRequirements);
	m_Memory.update(device, pData);
	rVerifyVk(vkBindBufferMemory(*device, **this, *m_Memory, 0u));
}

vkGpuBuffer::vkGpuBuffer(const vkDevice &device, eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void *pData)
{
	assert(!isValid() && device.isValid());

	VkBufferUsageFlagBits usageFlagBits = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
	switch (bindFlags)
	{
	case eVertexBuffer:
		usageFlagBits = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		break;
	case eIndexBuffer:
		usageFlagBits = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		break;
	case eUniformBuffer:
		usageFlagBits = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		break;
	default:
		assert(0);  /// Don't know yet
		break;
	}

	bool8_t useStagingBuffer = (bindFlags == eVertexBuffer || bindFlags == eIndexBuffer) && 
		(usage == eGpuReadWrite || usage == eGpuReadOnly);

	VkBufferCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		nullptr,
		0u,
		size,
		VkBufferUsageFlags(useStagingBuffer ? (usageFlagBits | VK_BUFFER_USAGE_TRANSFER_DST_BIT) : usageFlagBits),
		VK_SHARING_MODE_EXCLUSIVE,
		0u,
		nullptr
	};
	VkBuffer handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateBuffer(*device, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);

	VkMemoryRequirements memoryRequirements{};
	vkGetBufferMemoryRequirements(*device, handle, &memoryRequirements);
	m_Memory.create(device, usage, memoryRequirements);
	
	if (useStagingBuffer)
	{
		rVerifyVk(vkBindBufferMemory(*device, **this, *m_Memory, 0u));

		vkStagingBuffer stagingBuffer(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size, pData);
		vkCommandBuffer commandBuffer = device.allocCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		commandBuffer.begin();
		VkBufferCopy bufferCopy
		{
			0u,
			0u,
			size
		};
		vkCmdCopyBuffer(*commandBuffer, *stagingBuffer, handle, 1u, &bufferCopy);
		commandBuffer.end();
		device.freeCommandBuffer(commandBuffer);

		stagingBuffer.destroy(device);
	}
	else
	{
		m_Memory.update(device, pData);
		rVerifyVk(vkBindBufferMemory(*device, **this, *m_Memory, 0u));
	}
}

void vkFrameBuffer::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyFramebuffer(*device, **this, vkMemoryAllocator);
		reset();
	}
}
