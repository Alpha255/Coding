#include "VulkanBuffer.h"
#include "VulkanEngine.h"

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
	VKCheck(vkCreateBuffer(VulkanEngine::Instance().GetDevice().GetRef(), &createInfo, nullptr, &m_Object));
}