#include "VulkanAsync.h"
#include "VulkanEngine.h"

void VulkanSemaphore::Create()
{
	assert(!IsValid());

	VkSemaphoreCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		nullptr,
		0U  /// Flags is reserved for future use
	};

	Check(vkCreateSemaphore(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));
}

void VulkanSemaphore::Destory()
{
	assert(IsValid());

	vkDestroySemaphore(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	Reset();
}

void VulkanFence::Create(eFenceState state)
{
	assert(!IsValid());

	VkFenceCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		eUnSignaled == state ? 0U : VK_FENCE_CREATE_SIGNALED_BIT
	};

	Check(vkCreateFence(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));
}

void VulkanFence::Destory()
{
	assert(IsValid());

	vkDestroyFence(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	Reset();
}