#include "VulkanBuffer.h"
#include "VulkanEngine.h"

//uint32_t VulkanDeviceMemory::GetMemoryTypeIndex(uint32_t memoryTypeBits, uint32_t memoryPropertyFlagBits)
//{
//	/// Search memtypes to find first index with those properties
//	bool bValidMemoryType = false;
//	auto &deviceMemoryProperties = VulkanEngine::Instance().GetPhysicalDevice().GetDeviceMemoryProperties();
//	uint32_t memoryTypeIndex = 0U;
//	for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; ++i)
//	{
//		if (memoryTypeBits & 1)
//		{
//			/// Type is available, does it match user properties?
//			if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlagBits) == memoryPropertyFlagBits)
//			{
//				memoryTypeIndex = i;
//				bValidMemoryType = true;
//				break;
//			}
//		}
//		memoryTypeBits >>= 1;
//	}
//	assert(bValidMemoryType);
//
//	return memoryTypeIndex;
//}

void VulkanDeviceMemory::Create(size_t size, uint32_t memTypeBits, uint32_t memPropertyFlags)
{
	assert(!IsValid());

	m_MemPropertyFlags = memPropertyFlags;

	auto &deviceMemProperties = VulkanEngine::Instance().GetDevice().GetDeviceMemoryProperties();

	uint32_t memTypeIndex = UINT32_MAX;
	for (uint32_t i = 0; i < deviceMemProperties.memoryTypeCount; ++i)
	{
		if ((memTypeBits & 1) == 1)
		{
			if ((deviceMemProperties.memoryTypes[i].propertyFlags & memPropertyFlags) == memPropertyFlags)
			{
				memTypeIndex = i;
				break;
			}
		}
		memTypeBits >>= 1;
	}

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
	Check(vkFlushMappedMemoryRanges(VulkanEngine::Instance().GetDevice().Get(), 1, &memRange));
}

void VulkanDeviceMemory::Destory()
{
	assert(IsValid());

	vkFreeMemory(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	Reset();
}

void VulkanBuffer::Create(size_t size, uint32_t usage, uint32_t memPropertyFlags)
{
	assert(!IsValid());

	VkBufferCreateInfo createInfo = 
	{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		nullptr,
		0U,
		size,
		(VkBufferUsageFlags)usage,
		VK_SHARING_MODE_EXCLUSIVE,
		0U,
		nullptr
	};
	Check(vkCreateBuffer(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));

	VkMemoryRequirements memoryRequirements = {};
	vkGetBufferMemoryRequirements(VulkanEngine::Instance().GetDevice().Get(), m_Handle, &memoryRequirements);
	
	m_Memory.Create(size, memoryRequirements.memoryTypeBits, memPropertyFlags);

	Check(vkBindBufferMemory(VulkanEngine::Instance().GetDevice().Get(), m_Handle, m_Memory.Get(), 0U));
}

void VulkanBuffer::Destory()
{
	assert(IsValid());

	m_Memory.Destory();

	vkDestroyBuffer(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	Reset();
}

#if 0
void VulkanFrameBuffer::Create(const std::vector<VulkanImageView> &imageViews, const VulkanRenderPass &renderPass, uint32_t width, uint32_t height, uint32_t layers)
{
	assert(!IsValid());

	std::vector<VkImageView> vkViews(imageViews.size());
	for (uint32_t i = 0U; i < imageViews.size(); ++i)
	{
		assert(imageViews[i].IsValid());
		vkViews[i] = imageViews[i].Get();
	}

	VkFramebufferCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		nullptr,
		0U, /// Reserved for future use
		renderPass.Get(),
		(uint32_t)vkViews.size(),
		vkViews.data(),
		width,
		height, 
		layers
	};

	VKCheck(vkCreateFramebuffer(VulkanEngine::Instance().GetDevice(), &createInfo, nullptr, &m_Handle));
}

void VulkanFrameBuffer::Destory()
{
	assert(IsValid());

	vkDestroyFramebuffer(VulkanEngine::Instance().GetDevice(), m_Handle, nullptr);

	Reset();
}
#endif