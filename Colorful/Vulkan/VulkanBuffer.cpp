#include "VulkanBuffer.h"
#include "VulkanEngine.h"
#include "VulkanRenderPass.h"

void VulkanBuffer::VulkanDeviceMemory::create(VkDevice device, eRBufferUsage usage, const VkMemoryRequirements &memoryRequirements)
{
	assert(!isValid());

	VkMemoryAllocateInfo allocateInfo
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		nullptr,
		memoryRequirements.size,
		device.getMemoryTypeIndex(usage, memoryRequirements.memoryTypeBits)
	};

	rVerifyVk(vkAllocateMemory(device, &allocateInfo, vkMemoryAllocator, &Handle));

	/// If a memory object does not have the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT property, 
	/// then vkFlushMappedMemoryRanges must be called in order to guarantee that writes to the memory object from the host are made available to the host domain, 
	/// where they can be further made available to the device domain via a domain operation. Similarly, 
	/// vkInvalidateMappedMemoryRanges must be called to guarantee that writes which are available to the host domain are made visible to host operations.
	/// If the memory object does have the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT property flag, 
	/// writes to the memory object from the host are automatically made available to the host domain.Similarly, 
	/// writes made available to the host domain are automatically made visible to the host.
}

void VulkanBuffer::VulkanDeviceMemory::update(VkDevice device, const void *pData, size_t size, size_t offset)
{
	assert(isValid());

	/// VkMemoryMapFlags is a bitmask type for setting a mask, but is currently reserved for future use.
	void *pGpuMemory = nullptr;
	rVerifyVk(vkMapMemory(device, Handle, offset, size, 0u, &pGpuMemory));
	assert(pGpuMemory);

	verify(memcpy_s(pGpuMemory, size, pData, size) == 0);
	vkUnmapMemory(device, Handle);
}

void VulkanBuffer::destroy(VkDevice device)
{
	if (isValid())
	{
		vkFreeMemory(device, m_Memory.Handle, vkMemoryAllocator);
		m_Memory.Handle = VK_NULL_HANDLE;

		vkDestroyBuffer(device, Handle, vkMemoryAllocator);
		Handle = VK_NULL_HANDLE;
	}
}

VulkanStagingBuffer::VulkanStagingBuffer(VkDevice device, VkBufferUsageFlags usageFlagBits, size_t size, const void *pData)
{
	assert(!isValid() && pData);
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
	rVerifyVk(vkCreateBuffer(device, &createInfo, vkMemoryAllocator, &Handle));

	VkMemoryRequirements memoryRequirements{};
	vkGetBufferMemoryRequirements(device, Handle, &memoryRequirements);
	m_Memory.create(device, eGpuReadCpuWrite, memoryRequirements);
	m_Memory.update(device, pData, size);
	rVerifyVk(vkBindBufferMemory(device, Handle, m_Memory.Handle, 0u));
}

VulkanGpuBuffer::VulkanGpuBuffer(VkDevice device, eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void *pData)
{
	assert(!isValid());

	m_Size = size;
	m_Offset = 0u;

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
	rVerifyVk(vkCreateBuffer(device, &createInfo, vkMemoryAllocator, &Handle));

	VkMemoryRequirements memoryRequirements{};
	vkGetBufferMemoryRequirements(device, Handle, &memoryRequirements);
	m_Memory.create(device, usage, memoryRequirements);
	
	if (useStagingBuffer)
	{
		rVerifyVk(vkBindBufferMemory(device, Handle, m_Memory.Handle, 0u));

		VulkanStagingBuffer stagingBuffer(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size, pData);
		vkCommandBuffer commandBuffer = device.allocCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, false);
		commandBuffer.begin();
		VkBufferCopy bufferCopy
		{
			0u,
			0u,
			size
		};
		vkCmdCopyBuffer(commandBuffer.Handle, stagingBuffer.Handle, Handle, 1u, &bufferCopy);
		commandBuffer.end();
		VulkanQueueManager::instance()->transferQueue()->submit(commandBuffer);
		device.freeCommandBuffer(commandBuffer);

		stagingBuffer.destroy(device);
	}
	else
	{
		if (pData)
		{
			m_Memory.update(device, pData, size, 0u);
		}
		rVerifyVk(vkBindBufferMemory(device, Handle, m_Memory.Handle, 0u));
	}
}

void VulkanFrameBuffer::create(VkDevice device, const vkRenderPass &renderPass, const GfxFrameBufferDesc &desc)
{
	assert(renderPass.isValid() && !isValid());

	std::vector<VkImageView> attachments;
	for (uint32_t i = 0u; i < eMaxRenderTargets; ++i)
	{
		if (desc.ColorSurface[i])
		{
			auto imageView = static_cast<vkImageView *>(desc.ColorSurface[i]);
			assert(imageView);
			attachments.emplace_back(imageView->Handle);
		}
	}
	if (desc.DepthSurface)
	{
		auto depthImageView = static_cast<vkImageView *>(desc.DepthSurface);
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

	rVerifyVk(vkCreateFramebuffer(device, &createInfo, vkMemoryAllocator, &Handle));
}

void VulkanFrameBuffer::destroy(VkDevice device)
{
	if (isValid())
	{
		vkDestroyFramebuffer(device, Handle, vkMemoryAllocator);
		Handle = VK_NULL_HANDLE;
	}
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

void VulkanBufferPool::cleanup()
{
}
