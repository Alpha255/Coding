#include "VulkanBuffer.h"
#include "VulkanEngine.h"

void VulkanDeviceMemory::Create(size_t size, uint32_t memoryType, uint32_t flags)
{
	VkMemoryAllocateInfo allocInfo
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		nullptr,
		size,
		memoryType
	};
	VKCheck(vkAllocateMemory(VulkanEngine::Instance().GetDevice().Get(), &allocInfo, nullptr, &m_Handle));
}

void VulkanDeviceMemory::Update(const void *pMemory, size_t size, size_t offset)
{
	assert(IsValid());

	void *pDeviceMemory = nullptr;
	VKCheck(vkMapMemory(VulkanEngine::Instance().GetDevice().Get(), m_Handle, offset, size, 0U, &pDeviceMemory));

	memcpy(pDeviceMemory, pMemory, size);

	vkUnmapMemory(VulkanEngine::Instance().GetDevice().Get(), m_Handle);
}

void VulkanDeviceMemory::Destory()
{
	vkFreeMemory(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);
}

void VulkanBuffer::Create(size_t size, VkBufferUsageFlagBits usage)
{
	VkBufferCreateInfo createInfo = 
	{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		nullptr,
		0U,
		size,
		usage,
		VK_SHARING_MODE_EXCLUSIVE,
		0U,
		nullptr
	};
	VKCheck(vkCreateBuffer(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));

	VkMemoryRequirements memoryRequirements{};
	vkGetBufferMemoryRequirements(VulkanEngine::Instance().GetDevice().Get(), m_Handle, &memoryRequirements);

	/// Search memtypes to find first index with those properties
	bool bValidMemoryType = false;
	auto deviceMemoryProperties = VulkanEngine::Instance().GetPhysicalDevice().GetDeviceMemoryProperties();
	uint32_t requirementsMask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	uint32_t memoryType = 0U;
	uint32_t memoryTypeBits = memoryRequirements.memoryTypeBits;
	for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((memoryTypeBits & 1) == 1)
		{
			/// Type is available, does it match user properties?
			if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask) 
			{
				memoryType = i;
				bValidMemoryType = true;
				break;
			}
		}
		memoryTypeBits >>= 1;
	}
	assert(bValidMemoryType);
	
	m_Memory.Create(size, memoryType);
}

void VulkanBuffer::Destory()
{
	vkDestroyBuffer(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	m_Memory.Destory();
}