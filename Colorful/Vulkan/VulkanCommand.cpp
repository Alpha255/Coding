#include "Colorful/Vulkan/VulkanEngine.h"

VulkanCommandBuffer::VulkanCommandBuffer(VkCommandBufferLevel level, VkCommandBuffer handle)
	: m_Level(level)
{
	assert(handle != VK_NULL_HANDLE);
	setState(eInitial);
	Handle = handle;

	m_Fence = VulkanAsyncPool::instance()->allocFence();
}

VulkanCommandPool::VulkanCommandPool(VkDevice device, uint32_t queueIndex)
	: m_Device(device)
{
	assert(!isValid());

	/*************************
		VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: 
			specifies that command buffers allocated from the pool will be short - lived, 
			meaning that they will be reset or freed in a relatively short timeframe.This flag may be used by the implementation to control memory allocation behavior within the pool.

		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT:
			allows any command buffer allocated from a pool to be individually reset to the initial state; either by calling vkResetCommandBuffer, 
			or via the implicit reset when calling vkBeginCommandBuffer. 
			If this flag is not set on a pool, then vkResetCommandBuffer must not be called for any command buffer allocated from that pool.

		VK_COMMAND_POOL_CREATE_PROTECTED_BIT: 
			specifies that command buffers allocated from the pool are protected command buffers. 
			If the protected memory feature is not enabled, the VK_COMMAND_POOL_CREATE_PROTECTED_BIT bit of flags must not be set.
	**************************/

	VkCommandPoolCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		queueIndex  /// All command buffers allocated from this command pool must be submitted on queues from the same queue family.
	};

	GfxVerifyVk(vkCreateCommandPool(device, &createInfo, vkMemoryAllocator, &Handle));
}

void VulkanCommandPool::reset()
{
	assert(isValid());

	/// Resetting a command pool recycles all of the resources from all of the command buffers allocated from the command pool back to the command pool. 
	/// All command buffers that have been allocated from the command pool are put in the initial state.

	/// Any primary command buffer allocated from another VkCommandPool that is in the recording or executable state and has a secondary command buffer allocated from commandPool recorded into it, becomes invalid.

	/// VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT specifies that resetting a command pool recycles all of the resources from the command pool back to the system.

	/// All VkCommandBuffer objects allocated from commandPool must not be in the pending state
	for (auto it = m_CmdBuffers.begin(); it != m_CmdBuffers.end(); ++it)
	{
		assert(it->second->state() != VulkanCommandBuffer::ePending);
		it->second->setState(VulkanCommandBuffer::eInitial);
	}

	GfxVerifyVk(vkResetCommandPool(m_Device, Handle, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT));
}

void VulkanCommandPool::trim()
{
	/// Trimming a command pool recycles unused memory from the command pool back to the system. 
	/// Command buffers allocated from the pool are not affected by the command. vkTrimCommandPool/vkTrimCommandPoolKHR
	assert(isValid());

	assert(0);
	/// flags is reserved for future use
	/// vkTrimCommandPool(device.Handle, Handle, 0u);
	/// vkTrimCommandPoolKHR(device.Handle, Handle, 0u);
}

/********************************
	Each command buffer is always in one of the following states:

	Initial:
		When a command buffer is allocated, it is in the initial state. Some commands are able to reset a command buffer, 
		or a set of command buffers, back to this state from any of the executable, recording or invalid state. 
		Command buffers in the initial state can only be moved to the recording state, or freed.

	Recording:
		vkBeginCommandBuffer changes the state of a command buffer from the initial state to the recording state. 
		Once a command buffer is in the recording state, vkCmd* commands can be used to record to the command buffer.

	Executable:
		vkEndCommandBuffer ends the recording of a command buffer, and moves it from the recording state to the executable state. 
		Executable command buffers can be submitted, reset, or recorded to another command buffer.

	Pending:
		Queue submission of a command buffer changes the state of a command buffer from the executable state to the pending state. 
		Whilst in the pending state, applications must not attempt to modify the command buffer in any way - as the device may be processing the commands recorded to it. 
		Once execution of a command buffer completes, the command buffer reverts back to either the executable state, or the invalid state if it was recorded with VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT. 
		A synchronization command should be used to detect when this occurs.

	Invalid:
		Some operations, such as modifying or deleting a resource that was used in a command recorded to a command buffer, will transition the state of that command buffer into the invalid state. 
		Command buffers in the invalid state can only be reset or freed.

		Resetting a command buffer is an operation that discards any previously recorded commands and puts a command buffer in the initial state. 
		Resetting occurs as a result of vkResetCommandBuffer or vkResetCommandPool, or as part of vkBeginCommandBuffer (which additionally puts the command buffer in the recording state).

		Secondary command buffers can be recorded to a primary command buffer via vkCmdExecuteCommands. This partially ties the lifecycle of the two command buffers together - if the primary is submitted to a queue, 
		both the primary and any secondaries recorded to it move to the pending state. Once execution of the primary completes, so does any secondary recorded within it, 
		and once all executions of each command buffer complete, they move to the executable state. If a secondary moves to any other state whilst it is recorded to another command buffer, 
		the primary moves to the invalid state. A primary moving to any other state does not affect the state of the secondary. 
		Resetting or freeing a primary command buffer removes the linkage to any secondary command buffers that were recorded to it.
********************************/

VulkanCommandBufferPtr VulkanCommandPool::alloc(VkCommandBufferLevel level)
{
	assert(isValid());

	VulkanCommandBufferPtr cmdBuffer = nullptr;
	for (auto it = m_FreeCmdBuffers.begin(); it != m_FreeCmdBuffers.end(); ++it)
	{
		if ((*it)->level() == level)
		{
			cmdBuffer = *it;
			m_FreeCmdBuffers.erase(it);
			break;
		}
	}
	if (cmdBuffer)
	{
		m_CmdBuffers.emplace(std::make_pair(cmdBuffer->Handle, cmdBuffer));
		return cmdBuffer;
	}

	VkCommandBufferAllocateInfo allocateInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		Handle,
		level,
		1u
	};

	VkCommandBuffer handle = VK_NULL_HANDLE;
	GfxVerifyVk(vkAllocateCommandBuffers(m_Device, &allocateInfo, &handle));

	cmdBuffer = std::make_shared<VulkanCommandBuffer>(level, handle);
	m_CmdBuffers.emplace(std::make_pair(cmdBuffer->Handle, cmdBuffer));

	return cmdBuffer;
}

void VulkanCommandPool::free(VulkanCommandBufferPtr& cmdBuffer)
{
	assert(isValid());
	assert(cmdBuffer->state() != VulkanCommandBuffer::ePending);

	auto it = m_CmdBuffers.find(cmdBuffer->Handle);
	assert(it != m_CmdBuffers.end());
	m_FreeCmdBuffers.emplace_back(std::move(it->second));
	m_CmdBuffers.erase(it);

	/// Any primary command buffer that is in the recording or executable state and has any element of pCommandBuffers recorded into it, becomes invalid.

	/// All elements of pCommandBuffers must not be in the pending state
	if (cmdBuffer->isValid())
	{
		//const VkCommandBuffer commandBuffers[]
		//{
		//	commandBuffer.Handle
		//};
		//vkFreeCommandBuffers(device, Handle, 1u, commandBuffers);
		//commandBuffer.Handle = VK_NULL_HANDLE;

		cmdBuffer->setState(VulkanCommandBuffer::eInvalid);
	}
}

void VulkanCommandBuffer::begin()
{
	assert(isValid() && m_State != ePending && m_State != eRecording);

	VkCommandBufferBeginInfo beginInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		nullptr,   /// pNext must be NULL or a pointer to a valid instance of VkDeviceGroupCommandBufferBeginInfo
		0u,
		nullptr
	};
	GfxVerifyVk(vkBeginCommandBuffer(Handle, &beginInfo));

	setState(eRecording);
}

void VulkanCommandBuffer::end()
{
	assert(isValid() && m_State == eRecording);

	GfxVerifyVk(vkEndCommandBuffer(Handle));

	setState(eExecutable);
}

void VulkanCommandBuffer::beginRenderPass(const VkRenderPassBeginInfo& beginInfo, VkSubpassContents subpassContents)
{
	assert(isValid() && m_State != ePending && m_State != eRecording);

	/// commandBuffer must not be in the recording or pending state.

	/// If commandBuffer was allocated from a VkCommandPool which did not have the VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT flag set, commandBuffer must be in the initial state.

	/********************
	VkCommandBufferUsageFlags: is a bitmask of VkCommandBufferUsageFlagBits specifying usage behavior for the command buffer.

		VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT:
			specifies that each recording of the command buffer will only be submitted once, and the command buffer will be reset and recorded again between each submission.

		VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT:
			specifies that a secondary command buffer is considered to be entirely inside a render pass. If this is a primary command buffer, then this bit is ignored.

		VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT:
			specifies that a command buffer can be resubmitted to a queue while it is in the pending state, and recorded into multiple primary command buffers.
	*********************/

	/// pInheritanceInfo is a pointer to a VkCommandBufferInheritanceInfo structure, used if commandBuffer is a secondary command buffer. If this is a primary command buffer, then this value is ignored.

	begin();
	vkCmdBeginRenderPass(Handle, &beginInfo, subpassContents);
}

void VulkanCommandBuffer::endRenderPass()
{
	assert(isValid() && m_State == eRecording);

	vkCmdEndRenderPass(Handle);
	GfxVerifyVk(vkEndCommandBuffer(Handle));

	setState(eExecutable);
}

void VulkanCommandBuffer::execute(const std::shared_ptr<VulkanCommandBuffer>& primaryCmdBuffer)
{
	assert(isValid() && m_Level == VK_COMMAND_BUFFER_LEVEL_SECONDARY);
	assert(primaryCmdBuffer->isValid() && primaryCmdBuffer->m_Level == VK_COMMAND_BUFFER_LEVEL_PRIMARY && primaryCmdBuffer->m_State == eRecording);
	assert(m_State == ePending || m_State == eExecutable);

	/**************************
		If any element of pCommandBuffers was not recorded with the VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT flag, 
			and it was recorded into any other primary command buffer, that primary command buffer must not be in the pending state

			it must not be in the pending state.

			it must not have already been recorded to commandBuffer.

			it must not appear more than once in pCommandBuffers.

		Each element of pCommandBuffers must have been allocated from a VkCommandPool that was created for the same queue family as the VkCommandPool from which commandBuffer was allocated

		If vkCmdExecuteCommands is being called within a render pass instance, 
			that render pass instance must have been begun with the contents parameter of vkCmdBeginRenderPass set to VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS

			each element of pCommandBuffers must have been recorded with the VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT

			each element of pCommandBuffers must have been recorded with VkCommandBufferInheritanceInfo::subpass set to the index of the subpass which the given command buffer will be executed in

			the render passes specified in the pBeginInfo->pInheritanceInfo->renderPass members of the vkBeginCommandBuffer commands used to begin recording each element of pCommandBuffers must be compatible with the current render pass.

			and any element of pCommandBuffers was recorded with VkCommandBufferInheritanceInfo::framebuffer not equal to VK_NULL_HANDLE, that VkFramebuffer must match the VkFramebuffer used in the current render pass instance

		If vkCmdExecuteCommands is not being called within a render pass instance, each element of pCommandBuffers must not have been recorded with the VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
	***************************/

	const VkCommandBuffer cmdBuffers[]
	{
		Handle
	};
	vkCmdExecuteCommands(primaryCmdBuffer->Handle, 1u, cmdBuffers);
}