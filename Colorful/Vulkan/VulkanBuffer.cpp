#include "VulkanBuffer.h"
#include "VulkanEngine.h"

uint32_t VulkanDeviceMemory::GetMemoryTypeIndex(uint32_t memTypeBits, uint32_t usage)
{
	VkMemoryPropertyFlags required = 0;
	VkMemoryPropertyFlags preferred = 0;

	switch (usage) 
	{
	case eGpuReadWrite:
	case eGpuReadOnly:
		preferred |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case eGpuReadCpuWrite:
		required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		break;
	case eGpuCopyToCpu:
		required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		preferred |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		break;
	default:
		assert(0);
		break;
	}

	auto &deviceMemProperties = VulkanEngine::Instance().GetDevice().GetDeviceMemoryProperties();

	for (uint32_t i = 0; i < deviceMemProperties.memoryTypeCount; ++i) 
	{
		if (((memTypeBits >> i) & 1) == 0) 
		{
			continue;
		}

		const VkMemoryPropertyFlags properties = deviceMemProperties.memoryTypes[i].propertyFlags;
		if ((properties & required) != required) 
		{
			continue;
		}

		if ((properties & preferred) != preferred) 
		{
			continue;
		}

		m_MemPropertyFlags = required | preferred;
		return i;
	}

	for (uint32_t i = 0; i < deviceMemProperties.memoryTypeCount; ++i) 
	{
		if (((memTypeBits >> i) & 1) == 0) 
		{
			continue;
		}

		const VkMemoryPropertyFlags properties = deviceMemProperties.memoryTypes[i].propertyFlags;
		if ((properties & required) != required) 
		{
			continue;
		}

		m_MemPropertyFlags = required;
		return i;
	}

	return UINT32_MAX;
}

void VulkanDeviceMemory::Alloc(size_t size, uint32_t memTypeBits, uint32_t usage)
{
	assert(!IsValid());

	uint32_t memTypeIndex = GetMemoryTypeIndex(memTypeBits, usage);
	assert(memTypeIndex != UINT32_MAX);

	VkMemoryAllocateInfo allocInfo
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		nullptr,
		size,
		memTypeIndex
	};
	Check(vkAllocateMemory(VulkanEngine::Instance().GetDevice().Get(), &allocInfo, nullptr, &m_Handle));
}

void VulkanDeviceMemory::Update(const void *pMemory, size_t size, size_t offset)
{
	/// vkCmdCopyBuffer
	assert(IsValid() && pMemory);

	void *pDeviceMemory = nullptr;
	Check(vkMapMemory(VulkanEngine::Instance().GetDevice().Get(), m_Handle, offset, size, 0U, &pDeviceMemory));

	memcpy(pDeviceMemory, pMemory, size);

	if (!(m_MemPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
	{
		Flush(size, offset);
	}

	vkUnmapMemory(VulkanEngine::Instance().GetDevice().Get(), m_Handle);
}

void VulkanDeviceMemory::Flush(size_t size, size_t offset)
{
	assert(IsValid());

	VkMappedMemoryRange memRange
	{
		VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
		nullptr,
		m_Handle,
		offset,
		size
	};
	Check(vkFlushMappedMemoryRanges(VulkanEngine::Instance().GetDevice().Get(), 1U, &memRange));
}

void VulkanDeviceMemory::Invalidate(size_t size, size_t offset)
{
	assert(IsValid());

	VkMappedMemoryRange memRange
	{
		VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
		nullptr,
		m_Handle,
		offset,
		size
	};
	Check(vkInvalidateMappedMemoryRanges(VulkanEngine::Instance().GetDevice().Get(), 1, &memRange));
}

void VulkanDeviceMemory::Destory()
{
	assert(IsValid());

	vkFreeMemory(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	Reset();
}

void VulkanBuffer::Create(size_t size, uint32_t bufferUsageFlags, uint32_t usage)
{
	assert(!IsValid());

	VkBufferCreateInfo createInfo = 
	{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		nullptr,
		0U,
		size,
		(VkBufferUsageFlags)bufferUsageFlags,
		VK_SHARING_MODE_EXCLUSIVE,
		0U,
		nullptr
	};
	Check(vkCreateBuffer(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));

	VkMemoryRequirements memoryRequirements = {};
	vkGetBufferMemoryRequirements(VulkanEngine::Instance().GetDevice().Get(), m_Handle, &memoryRequirements);
	
	m_Memory.Alloc(memoryRequirements.size, memoryRequirements.memoryTypeBits, usage);

	Check(vkBindBufferMemory(VulkanEngine::Instance().GetDevice().Get(), m_Handle, m_Memory.Get(), 0U));

	m_DescriptorInfo = VkDescriptorBufferInfo
	{
		m_Handle,
		0U,
		VK_WHOLE_SIZE
	};
}

void VulkanBuffer::Destory()
{
	assert(IsValid());

	m_Memory.Destory();

	vkDestroyBuffer(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	Reset();
}

void VulkanFrameBuffer::Create(uint32_t width, uint32_t height, const std::vector<VkImageView> &views)
{
	assert(!IsValid());

	VkFramebufferCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		nullptr,
		0U,
		VulkanEngine::Instance().GetRenderPass().Get(),
		(uint32_t)views.size(),
		views.data(),
		width,
		height, 
		1U
	};

	Check(vkCreateFramebuffer(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));
}

void VulkanFrameBuffer::Destory()
{
	assert(IsValid());

	vkDestroyFramebuffer(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	Reset();
}