#include "Colorful/Vulkan/VulkanBuffer.h"
#include "Colorful/Vulkan/VulkanDevice.h"
#include "Colorful/Vulkan/VulkanMemoryAllocator.h"
#include "Colorful/Vulkan/VulkanQueue.h"

NAMESPACE_START(Gfx)

VulkanBuffer::VulkanBuffer(uint32_t bindFlags, EBufferUsage usage, size_t size, const void* data)
	: GpuBuffer(bindFlags & EBindFlags::UniformBuffer ? EDescriptorType::UniformBuffer : EDescriptorType::Null)
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
	if (bindFlags & EBindFlags::VertexBuffer)
	{
		usageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	}
	if (bindFlags & EBindFlags::IndexBuffer)
	{
		usageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	}
	if (bindFlags & EBindFlags::UniformBuffer)
	{
		usageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	}
	if (bindFlags & EBindFlags::ShaderResource)
	{
		usageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	}
	if (bindFlags & EBindFlags::UnorderedAccess)
	{
		usageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	}
	if (bindFlags & EBindFlags::IndirectBuffer)
	{
		usageFlags |=  VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
	}

	if (usage == EBufferUsage::Staging)
	{
		usageFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
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
	VERIFY_VK(vkCreateBuffer(s_Device->get(), &createInfo, VK_MEMORY_ALLOCATOR, reference()));

	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(s_Device->get(), get(), &memReq);
	VkMemoryAllocateInfo allocateInfo
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		nullptr,
		memReq.size,
		VulkanMemoryAllocator::instance().getMemoryTypeIndex(usage, memReq.memoryTypeBits)
	};

	m_Size = memReq.size;
	VERIFY_VK(vkAllocateMemory(s_Device->get(), &allocateInfo, VK_MEMORY_ALLOCATOR, &m_Memory));
	VERIFY_VK(vkBindBufferMemory(s_Device->get(), get(), m_Memory, 0u));

	if (data)
	{
		if (usage == EBufferUsage::Staging)
		{
			update_native(data, size, 0u, false);
		}
		else
		{
			VkQueues::instance().enqueueTransferCommand(
				m_Handle,
				VkBufferCopy
				{
					0u,
					0u,
					size
				},
				data);
		}
	}
}

void* VulkanBuffer::map_native(size_t size, size_t offset)
{
	assert(isValid() && offset < m_Size && (size == VK_WHOLE_SIZE || offset + size <= m_Size));

	if (!m_MappedMemory.Memory)
	{
		m_MappedMemory.Offset = offset;
		m_MappedMemory.Size = size;
		if (size != VK_WHOLE_SIZE)
		{
			m_MappedMemory.Size = std::min<size_t>(align(size, s_Device->limits().nonCoherentAtomSize),
				m_Size - offset);
			assert(m_MappedMemory.Offset + m_MappedMemory.Size <= m_Size);
		}
		VERIFY_VK(vkMapMemory(s_Device->get(), m_Memory, offset, m_MappedMemory.Size, 0u, &m_MappedMemory.Memory));
	}

	return m_MappedMemory.Memory;
}

void VulkanBuffer::unmap()
{
	assert(isValid());

	if (m_MappedMemory.Memory)
	{
		vkUnmapMemory(s_Device->get(), m_Memory);
		m_MappedMemory.Memory = nullptr;
		m_MappedMemory.Offset = 0u;
		m_MappedMemory.Size = VK_WHOLE_SIZE;
	}
}

void VulkanBuffer::update_native(const void* data, size_t size, size_t offset, bool8_t persistence)
{
	assert(isValid() && offset < m_Size && (size == VK_WHOLE_SIZE || offset + size <= m_Size));

	map(size, offset);
	auto sizeOffset = insureMappedRanges(size, offset);
	VERIFY(memcpy_s(reinterpret_cast<byte8_t*>(m_MappedMemory.Memory) + sizeOffset.second, size, reinterpret_cast<const byte8_t*>(data), size) == 0);
	if (persistence)
	{
		m_MappedMemory.Size = sizeOffset.first;
		m_MappedMemory.Offset = sizeOffset.second;
		flushMappedRange(size, offset);
	}
	else
	{
		unmap();
	}
}

void VulkanBuffer::flushMappedRange_native(size_t size, size_t offset)
{
	assert(isValid() && offset < m_Size && (size == VK_WHOLE_SIZE || offset + size <= m_Size));

	/// To flush ranges of non-coherent memory from the host caches

	assert(m_MappedMemory.Memory);

	auto sizeOffset = insureMappedRanges(size, offset);
	VkMappedMemoryRange ranges[]
	{
		{
			VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			nullptr,
			m_Memory,
			sizeOffset.second,
			sizeOffset.first
		}
	};
	VERIFY_VK(vkFlushMappedMemoryRanges(s_Device->get(), 1u, ranges));
}

void VulkanBuffer::invalidateMappedRange(size_t offset, size_t size)
{
	assert(isValid() && offset < m_Size && (size == VK_WHOLE_SIZE || offset + size <= m_Size));

	/// Invalidate ranges of mapped memory objects

	assert(m_MappedMemory.Memory);

	auto sizeOffset = insureMappedRanges(size, offset);
	VkMappedMemoryRange ranges[]
	{
		{
			VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			nullptr,
			m_Memory,
			sizeOffset.second,
			sizeOffset.first,
		}
	};
	VERIFY_VK(vkInvalidateMappedMemoryRanges(s_Device->get(), 1u, ranges));
}

std::pair<size_t, size_t> VulkanBuffer::insureMappedRanges(size_t size, size_t offset)
{
	assert(m_MappedMemory.Memory && (size == VK_WHOLE_SIZE || offset + size <= m_Size));

	size_t realOffset = align(offset, s_Device->limits().nonCoherentAtomSize);
	size_t realSize = size;
	if (size != VK_WHOLE_SIZE)
	{
		realSize = align(size, s_Device->limits().nonCoherentAtomSize);
		assert((size + offset == m_Size) || (realOffset < m_MappedMemory.Size && realSize <= m_MappedMemory.Size));
		assert(realOffset + realSize <= m_MappedMemory.Size);
	}

	return std::pair<size_t, size_t>{ realSize, realOffset };
}

VulkanBuffer::~VulkanBuffer()
{
	unmap();
	vkFreeMemory(s_Device->get(), m_Memory, VK_MEMORY_ALLOCATOR);
	vkDestroyBuffer(s_Device->get(), get(), VK_MEMORY_ALLOCATOR);
	m_Memory = VK_NULL_HANDLE;
}

NAMESPACE_END(Gfx)
