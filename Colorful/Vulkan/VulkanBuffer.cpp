#include "Colorful/Vulkan/VulkanBuffer.h"

NAMESPACE_START(Gfx)

VulkanBuffer::VulkanBuffer(VkDevice device, uint32_t bindFlags, EBufferUsage usage, size_t size, const void* data)
{
	/// If a memory object does not have the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT property, 
	/// then vkFlushMappedMemoryRanges must be called in order to guarantee that writes to the memory object from the host are made available to the host domain, 
	/// where they can be further made available to the device domain via a domain operation. Similarly, 
	/// vkInvalidateMappedMemoryRanges must be called to guarantee that writes which are available to the host domain are made visible to host operations.
	/// If the memory object does have the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT property flag, 
	/// writes to the memory object from the host are automatically made available to the host domain.Similarly, 
	/// writes made available to the host domain are automatically made visible to the host.

	/// The memory type that allows us to access it from the CPU may not be the most optimal memory type for the graphics card itself to read from. 
	/// The most optimal memory has the VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT flag and is usually not accessible by the CPU on dedicated graphics cards. 
	/// One staging buffer in CPU accessible memory to upload the data from the vertex array to, and the final vertex buffer in device local memory.

	/// VK_SHARING_MODE_EXCLUSIVE specifies that access to any range or image subresource of the object will be exclusive to a single queue family at a time
	/// VK_SHARING_MODE_CONCURRENT specifies that concurrent access to any range or image subresource of the object from multiple queue families is supported

	VkBufferUsageFlags usageFlags = 0u;
	if (bindFlags & EBufferBindFlags::VertexBufferFlag)
	{
		usageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	}
	if (bindFlags & EBufferBindFlags::IndexBufferFlag)
	{
		usageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	}
	if (bindFlags & EBufferBindFlags::UniformBufferFlag)
	{
		usageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	}
	if (bindFlags & EBufferBindFlags::ShaderStorageFlag)
	{
		usageFlags |= (VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	}
	if (bindFlags & EBufferBindFlags::UnorderedAccessFlag)
	{
		usageFlags |= (VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	}
	if (bindFlags & EBufferBindFlags::IndirectBufferFlag)
	{
		usageFlags |=  VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
	}

	VkBufferCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		nullptr,
		0u,
		size,
		usageFlags, /// VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT
		VK_SHARING_MODE_EXCLUSIVE,
		0u,
		nullptr
	};
	VERIFY_VK(vkCreateBuffer(device, &createInfo, VK_MEMORY_ALLOCATOR, reference()));

	VkMemoryRequirements memReq{};
	vkGetBufferMemoryRequirements(device, get(), &memReq);
	VkMemoryAllocateInfo allocateInfo
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		nullptr,
		memReq.size,
		0u /// ??? 
		///VulkanBufferPool::instance()->memoryTypeIndex(usage, requirements.memoryTypeBits)
	};

	VERIFY_VK(vkAllocateMemory(device, &allocateInfo, VK_MEMORY_ALLOCATOR, &m_Memory));

	//if (useStagingBuffer)
	//{
	//	GfxVerifyVk(vkBindBufferMemory(device, Handle, m_Memory.Handle, 0u));
	//	VulkanQueueManager::instance()->queueBufferCopyCommand(Handle, size, data);
	//}
	//else
	{
		if (data)
		{
			/// VkMemoryMapFlags is a bitmask type for setting a mask, but is currently reserved for future use.
			void* GPUMemory = nullptr;
			VERIFY_VK(vkMapMemory(device, m_Memory, 0u, size, 0u, &GPUMemory));
			assert(GPUMemory);

			VERIFY(memcpy_s(GPUMemory, size, data, size) == 0);
			vkUnmapMemory(device, m_Memory);
		}

		VERIFY_VK(vkBindBufferMemory(device, get(), m_Memory, 0u));
	}
}

//void VulkanBuffer::update(const void* data, size_t size, size_t offset)
//{
//	VulkanBufferPool::instance()->updateBuffer(this, data, size, offset);
//}
//
//void VulkanBuffer::free()
//{
//	VulkanBufferPool::instance()->free(this);
//}

#if 0
uint32_t VulkanBufferPool::memoryTypeIndex(eRBufferUsage usage, uint32_t memoryTypeBits) const
{
	assert(usage < eRBufferUsage_MaxEnum);

	VkMemoryPropertyFlags memoryPropertyFlags = 0u;
	switch (usage)
	{
	case eGpuReadWrite:
	case eGpuReadOnly:
		memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case eGpuReadCpuWrite:
		memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		break;
	case eGpuCopyToCpu:
		memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		break;
	default:
		assert(0);
		break;
	}

	for (uint32_t i = 0u; i < m_DeviceMemoryProperties.memoryTypeCount; ++i)
	{
		if (((memoryTypeBits >> i) & 1u) == 1u)
		{
			if ((m_DeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags)
			{
				return i;
			}
		}
	}

	assert(0);
	return std::numeric_limits<uint32_t>().max();
}

void VulkanBufferPool::delayFree(bool8_t force)
{
	if (m_DelayFreeList.size() >= eMaxDelayList || force)
	{
		m_ListIndex = 0u;
		for (uint32_t i = 0u; i < m_DelayFreeList.size(); ++i)
		{
			if (m_DelayFreeList[i])
			{
				m_DelayFreeList[i]->destroy(m_Device);
				safeDelete(m_DelayFreeList[i]);
			}
		}
	}
}

void VulkanBufferPool::cleanup()
{
	delayFree(true);
	m_DelayFreeList.clear();

	for each (auto it in m_Buffers)
	{
		it.second->destroy(m_Device);
		safeDelete(it.second);
	}
	m_Buffers.clear();
}
#endif

NAMESPACE_END(Gfx)
