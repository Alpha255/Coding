#include "VulkanBuffer.h"
#include "VulkanEngine.h"
#include "VulkanRenderPass.h"

void VulkanDeviceMemory::create(VkDevice device, eRBufferUsage usage, const VkMemoryRequirements& memoryRequirements)
{
	assert(!isValid());

	VkMemoryAllocateInfo allocateInfo
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		nullptr,
		memoryRequirements.size,
		VulkanBufferPool::instance()->memoryTypeIndex(usage, memoryRequirements.memoryTypeBits)
	};

	GfxVerifyVk(vkAllocateMemory(device, &allocateInfo, vkMemoryAllocator, &Handle));

	/// If a memory object does not have the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT property, 
	/// then vkFlushMappedMemoryRanges must be called in order to guarantee that writes to the memory object from the host are made available to the host domain, 
	/// where they can be further made available to the device domain via a domain operation. Similarly, 
	/// vkInvalidateMappedMemoryRanges must be called to guarantee that writes which are available to the host domain are made visible to host operations.
	/// If the memory object does have the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT property flag, 
	/// writes to the memory object from the host are automatically made available to the host domain.Similarly, 
	/// writes made available to the host domain are automatically made visible to the host.
}

void VulkanDeviceMemory::update(VkDevice device, const void* data, size_t size, size_t offset)
{
	assert(isValid());

	/// VkMemoryMapFlags is a bitmask type for setting a mask, but is currently reserved for future use.
	void* gpuMemory = nullptr;
	GfxVerifyVk(vkMapMemory(device, Handle, offset, size, 0u, &gpuMemory));
	assert(gpuMemory);

	verify(memcpy_s(gpuMemory, size, data, size) == 0);
	vkUnmapMemory(device, Handle);
}

void VulkanBuffer::update(const void* data, size_t size, size_t offset)
{
	VulkanBufferPool::instance()->updateBuffer(this, data, size, offset);
}

void VulkanBuffer::free()
{
	VulkanBufferPool::instance()->free(this);
}

VulkanBuffer::VulkanBuffer(VkDevice device, VkBufferUsageFlags usageFlagBits, size_t size, const void* data)
{
	assert(!isValid() && data);
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
	GfxVerifyVk(vkCreateBuffer(device, &createInfo, vkMemoryAllocator, &Handle));

	VkMemoryRequirements memoryRequirements{};
	vkGetBufferMemoryRequirements(device, Handle, &memoryRequirements);
	m_Memory.create(device, eGpuReadCpuWrite, memoryRequirements);
	m_Memory.update(device, data, size);
	GfxVerifyVk(vkBindBufferMemory(device, Handle, m_Memory.Handle, 0u));
}

VulkanBuffer::VulkanBuffer(VkDevice device, eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void* data)
{
	/// The memory type that allows us to access it from the CPU may not be the most optimal memory type for the graphics card itself to read from. 
	/// The most optimal memory has the VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT flag and is usually not accessible by the CPU on dedicated graphics cards. 
	/// One staging buffer in CPU accessible memory to upload the data from the vertex array to, and the final vertex buffer in device local memory.
	assert(!isValid());

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
	GfxVerifyVk(vkCreateBuffer(device, &createInfo, vkMemoryAllocator, &Handle));

	VkMemoryRequirements memoryRequirements{};
	vkGetBufferMemoryRequirements(device, Handle, &memoryRequirements);
	m_Memory.create(device, usage, memoryRequirements);
	
	if (useStagingBuffer)
	{
		GfxVerifyVk(vkBindBufferMemory(device, Handle, m_Memory.Handle, 0u));

		VulkanBuffer stagingBuffer(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size, data);
		VkBufferCopy bufferCopy
		{
			0u,
			0u,
			size
		};

		VulkanQueueManager::instance()->transferQueue()->queueSubmitCopyCommand(Handle, stagingBuffer, bufferCopy);
	}
	else
	{
		if (data)
		{
			m_Memory.update(device, data, size, 0u);
		}
		GfxVerifyVk(vkBindBufferMemory(device, Handle, m_Memory.Handle, 0u));
	}
}

void VulkanFrameBuffer::create(VkDevice device, const VulkanRenderPass& renderPass, const GfxFrameBufferDesc& desc)
{
	assert(renderPass.isValid() && !isValid());

	std::vector<VkImageView> attachments;
	for (uint32_t i = 0u; i < eMaxRenderTargets; ++i)
	{
		if (desc.ColorSurface[i])
		{
			auto imageView = static_cast<VulkanImageView *>(desc.ColorSurface[i]);
			assert(imageView);
			attachments.emplace_back(imageView->Handle);
		}
	}
	if (desc.DepthSurface)
	{
		auto depthImageView = static_cast<VulkanImageView *>(desc.DepthSurface);
		assert(depthImageView);
		attachments.emplace_back(depthImageView->Handle);
	}

	VkFramebufferCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		nullptr,
		0u,
		renderPass.Handle,
		(uint32_t)attachments.size(),
		attachments.data(),
		desc.Width,
		desc.Height,
		desc.Layers
	};

	GfxVerifyVk(vkCreateFramebuffer(device, &createInfo, vkMemoryAllocator, &Handle));
}

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
			m_DelayFreeList[i]->destroy(m_Device);
			safeDelete(m_DelayFreeList[i]);
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
