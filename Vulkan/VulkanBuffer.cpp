#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanRenderPass.h"
#include "VulkanEngine.h"

uint32_t VulkanDeviceMemory::GetMemoryTypeIndex(uint32_t memoryTypeBits, uint32_t memoryPropertyFlagBits)
{
	/// Search memtypes to find first index with those properties
	bool bValidMemoryType = false;
	auto &deviceMemoryProperties = VulkanEngine::Instance().GetVulkanDevice().GetPhysicalDeviceMemoryProperties();
	uint32_t memoryTypeIndex = 0U;
	for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; ++i)
	{
		if (memoryTypeBits & 1)
		{
			/// Type is available, does it match user properties?
			if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlagBits) == memoryPropertyFlagBits)
			{
				memoryTypeIndex = i;
				bValidMemoryType = true;
				break;
			}
		}
		memoryTypeBits >>= 1;
	}
	assert(bValidMemoryType);

	return memoryTypeIndex;
}

void VulkanDeviceMemory::Create(size_t size, uint32_t memoryTypeIndex)
{
	assert(!IsValid());

	VkMemoryAllocateInfo allocInfo
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		nullptr,
		size,
		memoryTypeIndex
	};
	VKCheck(vkAllocateMemory(VulkanEngine::Instance().GetDevice(), &allocInfo, nullptr, &m_Handle));
}

void VulkanDeviceMemory::Update(const void *pMemory, size_t size, size_t offset)
{
	assert(IsValid() && pMemory && size);

	void *pDeviceMemory = nullptr;
	VKCheck(vkMapMemory(VulkanEngine::Instance().GetDevice(), m_Handle, offset, size, 0U, &pDeviceMemory));

	memcpy(pDeviceMemory, pMemory, size);

	vkUnmapMemory(VulkanEngine::Instance().GetDevice(), m_Handle);
}

void VulkanDeviceMemory::Destory()
{
	///assert(IsValid());

	vkFreeMemory(VulkanEngine::Instance().GetDevice(), m_Handle, nullptr);

	Reset();
}

void VulkanBuffer::Create(size_t size, uint32_t usage, uint32_t memoryPropertyFlags)
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
	VKCheck(vkCreateBuffer(VulkanEngine::Instance().GetDevice(), &createInfo, nullptr, &m_Handle));

	VkMemoryRequirements memoryRequirements = {};
	vkGetBufferMemoryRequirements(VulkanEngine::Instance().GetDevice(), m_Handle, &memoryRequirements);
	
	m_Memory.Create(size, VulkanDeviceMemory::GetMemoryTypeIndex(memoryRequirements.memoryTypeBits, memoryPropertyFlags));

	VKCheck(vkBindBufferMemory(VulkanEngine::Instance().GetDevice(), m_Handle, m_Memory.Get(), 0));
}

void VulkanBuffer::Destory()
{
	assert(IsValid());

	vkDestroyBuffer(VulkanEngine::Instance().GetDevice(), m_Handle, nullptr);

	Reset();
}

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