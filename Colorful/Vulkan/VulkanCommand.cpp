#include "Colorful/Vulkan/VulkanCommand.h"
#include "Colorful/Vulkan/VulkanDevice.h"
#include "Colorful/Vulkan/VulkanEnumTranslator.h"

NAMESPACE_START(Gfx)

void VulkanCommandBuffer::beginDebugMarker(const char8_t* name, const Vec4& color)
{
	assert(m_State == EState::Recording && name);

	if (s_Device->enableProfiler())
	{
		VkDebugMarkerMarkerInfoEXT markerInfo
		{
			VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT,
			nullptr,
			name,
			{ color.x, color.y, color.z, color.w }
		};
		vkCmdDebugMarkerBeginEXT(m_Handle, &markerInfo);
	}
}

void VulkanCommandBuffer::insertDebugMarker(const char8_t* name, const Vec4& color)
{
	assert(m_State == EState::Recording && name);

	if (s_Device->enableProfiler())
	{
		VkDebugMarkerMarkerInfoEXT markerInfo
		{
			VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT,
			nullptr,
			name,
			{ color.x, color.y, color.z, color.w }
		};
		vkCmdDebugMarkerInsertEXT(m_Handle, &markerInfo);
	}
}

void VulkanCommandBuffer::endDebugMarker()
{
	assert(m_State == EState::Recording);

	if (s_Device->enableProfiler())
	{
		vkCmdDebugMarkerEndEXT(m_Handle);
	}
}

void VulkanCommandBuffer::setObjectDebugName(uint64_t objectHandle, EDescriptorType type, const char8_t* name)
{
	assert(objectHandle != VK_NULL_HANDLE && m_State == EState::Recording && name);

	if (s_Device->enableProfiler())
	{
		VkDebugMarkerObjectNameInfoEXT objectNameInfo
		{
			VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT,
			nullptr,
			VkEnumTranslator::debugReportObjectType(type),
			objectHandle,
			name
		};
		vkDebugMarkerSetObjectNameEXT(s_Device->get(), &objectNameInfo);
	}
}

void VulkanCommandBuffer::drawIndexed(uint32_t indexCount, uint32_t firstIndex, int32_t vertexOffset)
{
	//assert(0);
	(void)indexCount;
	(void)firstIndex;
	(void)vertexOffset;
}

void VulkanCommandBuffer::beginRenderPass(const RenderPassDesc& renderPassDesc, GraphicsPipelineState& pipelineState)
{
	assert(m_State == EState::Initial || m_State == EState::Recording);

	if (m_State == EState::Initial)
	{
		begin();
	}


}

void VulkanCommandBuffer::endRenderPass()
{
	//assert(0);
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

NAMESPACE_END(Gfx)

#if 0
#include "Colorful/Vulkan/VulkanEngine.h"

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

	end();
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
#endif