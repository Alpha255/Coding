#include "Colorful/Vulkan/VulkanCommandPool.h"
#include "Colorful/Vulkan/VulkanDevice.h"

NAMESPACE_START(Gfx)

VulkanCommandBufferPool::VulkanCommandBufferPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags)
{
	/*************************
		VK_COMMAND_POOL_CREATE_TRANSIENT_BIT:
			specifies that command buffers allocated from the pool will be short - lived,
			meaning that they will be reset or freed in a relatively short timeframe.
			This flag may be used by the implementation to control memory allocation behavior within the pool.

		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT:
			allows any command buffer allocated from a pool to be individually reset to the initial state;
			either by calling vkResetCommandBuffer, or via the implicit reset when calling vkBeginCommandBuffer.
			If this flag is not set on a pool, then vkResetCommandBuffer must not be called for any command buffer allocated from that pool.

		VK_COMMAND_POOL_CREATE_PROTECTED_BIT:
			specifies that command buffers allocated from the pool are protected command buffers.
			If the protected memory feature is not enabled, the VK_COMMAND_POOL_CREATE_PROTECTED_BIT bit of flags must not be set.
	**************************/
	VkCommandPoolCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		flags,
		queueFamilyIndex  /// All command buffers allocated from this command pool must be submitted on queues from the same queue family.
	};
	VERIFY_VK(vkCreateCommandPool(s_Device->get(), &createInfo, VK_MEMORY_ALLOCATOR, reference()));
}

void VulkanCommandBufferPool::reset()
{
	assert(isValid());

	/// Resetting a command pool recycles all of the resources from all of the command buffers allocated from the command pool back to the command pool. 
	/// All command buffers that have been allocated from the command pool are put in the initial state.
	/// Any primary command buffer allocated from another VkCommandPool that is in the recording or executable state and has a secondary command buffer allocated from commandPool recorded into it, becomes invalid.
	/// VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT specifies that resetting a command pool recycles all of the resources from the command pool back to the system.
	/// All VkCommandBuffer objects allocated from commandPool must not be in the pending state

	VERIFY_VK(vkResetCommandPool(s_Device->get(), get(), VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT));
}

VulkanCommandBufferPtr VulkanCommandBufferPool::getOrAlloc(VkCommandBufferLevel level, bool8_t autoBegin)
{
	assert(isValid());

	VulkanCommandBufferPtr command = nullptr;
	if (!m_CommandBuffers.empty())
	{
		command = m_CommandBuffers.front();
		m_CommandBuffers.pop_front();
		assert(command->state() == VulkanCommandBuffer::EState::Initial || command->state() == VulkanCommandBuffer::EState::Invalid);
	}
	else
	{
		VkCommandBufferAllocateInfo allocateInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			nullptr,
			get(),
			level,
			1u
		};

		command = create_shared<VulkanCommandBuffer>();
		VERIFY_VK(vkAllocateCommandBuffers(s_Device->get(), &allocateInfo, command->reference()));
		m_CommandBuffers.push_back(command);
	}

	if (autoBegin)
	{
		command->begin();
	}

	return command;
}

void VulkanCommandBufferPool::free(VulkanCommandBufferPtr& command)
{
	assert(isValid());
	assert(command->state() != VulkanCommandBuffer::EState::Pending);

	/// Any primary command buffer that is in the recording or executable state and has any element of pCommandBuffers recorded into it, becomes invalid.
	/// All elements of pCommandBuffers must not be in the pending state
	/// vkFreeCommandBuffers(device, Handle, 1u, commandBuffers);
	m_CommandBuffers.push_back(std::move(command));
}

VulkanCommandBufferPool::~VulkanCommandBufferPool()
{
	/// When a pool is destroyed, all command buffers allocated from the pool are freed.

	/// Any primary command buffer allocated from another VkCommandPool that is in the recording or 
	/// executable state and has a secondary command buffer allocated from commandPool recorded into it, becomes invalid.

	/// All VkCommandBuffer objects allocated from commandPool must not be in the pending state.
	for (auto& cmd : m_CommandBuffers)
	{
		assert(cmd->state() != VulkanCommandBuffer::EState::Pending);
	}
	m_CommandBuffers.clear();

	vkDestroyCommandPool(s_Device->get(), get(), VK_MEMORY_ALLOCATOR);
}

NAMESPACE_END(Gfx)