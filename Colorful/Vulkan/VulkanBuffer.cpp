#include "VulkanBuffer.h"
#include "VulkanEngine.h"
#include "VulkanRenderPass.h"

void vkDeviceMemory::create(const VulkanDevice &device, eRBufferUsage usage, const VkMemoryRequirements &memoryRequirements)
{
	assert(!isValid() && device.isValid());

	VkMemoryAllocateInfo allocateInfo
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		nullptr,
		memoryRequirements.size,
		device.getMemoryTypeIndex(usage, memoryRequirements.memoryTypeBits)
	};

	rVerifyVk(vkAllocateMemory(device.Handle, &allocateInfo, vkMemoryAllocator, &Handle));

	/// If a memory object does not have the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT property, 
	/// then vkFlushMappedMemoryRanges must be called in order to guarantee that writes to the memory object from the host are made available to the host domain, 
	/// where they can be further made available to the device domain via a domain operation. Similarly, 
	/// vkInvalidateMappedMemoryRanges must be called to guarantee that writes which are available to the host domain are made visible to host operations.
	/// If the memory object does have the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT property flag, 
	/// writes to the memory object from the host are automatically made available to the host domain.Similarly, 
	/// writes made available to the host domain are automatically made visible to the host.
}

void vkDeviceMemory::destroy(const VulkanDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkFreeMemory(device.Handle, Handle, vkMemoryAllocator);
		Handle = VK_NULL_HANDLE;
	}
}

void vkDeviceMemory::update(const VulkanDevice &device, const void *pData, size_t size, size_t offset)
{
	assert(isValid());

	/// VkMemoryMapFlags is a bitmask type for setting a mask, but is currently reserved for future use.
	void *pGpuMemory = nullptr;
	rVerifyVk(vkMapMemory(device.Handle, Handle, offset, size, 0u, &pGpuMemory));
	assert(pGpuMemory);
	verify(memcpy_s(pGpuMemory, size, pData, size) == 0);
	vkUnmapMemory(device.Handle, Handle);
}

void vkBuffer::destroy(const VulkanDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		m_Memory.destroy(device);
		vkDestroyBuffer(device.Handle, Handle, vkMemoryAllocator);
		Handle = VK_NULL_HANDLE;
	}
}

vkStagingBuffer::vkStagingBuffer(const VulkanDevice &device, VkBufferUsageFlags usageFlagBits, size_t size, const void *pData)
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
	rVerifyVk(vkCreateBuffer(device.Handle, &createInfo, vkMemoryAllocator, &Handle));

	VkMemoryRequirements memoryRequirements{};
	vkGetBufferMemoryRequirements(device.Handle, Handle, &memoryRequirements);
	m_Memory.create(device, eGpuReadCpuWrite, memoryRequirements);
	m_Memory.update(device, pData, size);
	rVerifyVk(vkBindBufferMemory(device.Handle, Handle, m_Memory.Handle, 0u));
}

vkGpuBuffer::vkGpuBuffer(const VulkanDevice &device, eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void *pData)
{
	assert(!isValid() && device.isValid());

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
	rVerifyVk(vkCreateBuffer(device.Handle, &createInfo, vkMemoryAllocator, &Handle));

	VkMemoryRequirements memoryRequirements{};
	vkGetBufferMemoryRequirements(device.Handle, Handle, &memoryRequirements);
	m_Memory.create(device, usage, memoryRequirements);
	
	if (useStagingBuffer)
	{
		rVerifyVk(vkBindBufferMemory(device.Handle, Handle, m_Memory.Handle, 0u));

		vkStagingBuffer stagingBuffer(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size, pData);
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
		vkEngine::instance().getQueue().submit(commandBuffer);
		device.freeCommandBuffer(commandBuffer);

		stagingBuffer.destroy(device);
	}
	else
	{
		if (pData)
		{
			m_Memory.update(device, pData, size, 0u);
		}
		rVerifyVk(vkBindBufferMemory(device.Handle, Handle, m_Memory.Handle, 0u));
	}
}

void vkFrameBuffer::create(const VulkanDevice &device, const vkRenderPass &renderPass, const rFrameBufferDesc &desc)
{
	assert(device.isValid() && renderPass.isValid() && !isValid());

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

	rVerifyVk(vkCreateFramebuffer(device.Handle, &createInfo, vkMemoryAllocator, &Handle));
}

void vkFrameBuffer::destroy(const VulkanDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyFramebuffer(device.Handle, Handle, vkMemoryAllocator);
		Handle = VK_NULL_HANDLE;
	}
}
