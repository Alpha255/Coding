#pragma once

#include "Colorful/Vulkan/VulkanAsync.h"

NAMESPACE_START(Gfx)
/// Command buffers are objects used to record commands which can be subsequently submitted to a device queue for execution. 
/// There are two levels of command buffers - primary command buffers, which can execute secondary command buffers, and which are submitted to queues, 
/// and secondary command buffers, which can be executed by primary command buffers, and which are not directly submitted to queues.

/// Each command buffer manages state independently of other command buffers. There is no inheritance of state across primary and secondary command buffers, or between secondary command buffers. 
/// When a command buffer begins recording, all state in that command buffer is undefined. When secondary command buffer(s) are recorded to execute on a primary command buffer, 
/// the secondary command buffer inherits no state from the primary command buffer, and all state of the primary command buffer is undefined after an execute secondary command buffer command is recorded. 
/// There is one exception to this rule - if the primary command buffer is inside a render pass instance, then the render pass and subpass state is not disturbed by executing secondary command buffers. 
/// For state dependent commands (such as draws and dispatches), any state consumed by those commands must not be undefined.

/// Unless otherwise specified, and without explicit synchronization, the various commands submitted to a queue via command buffers may execute in arbitrary order relative to each other, and/or concurrently. 
/// Also, the memory side-effects of those commands may not be directly visible to other commands without explicit memory dependencies. This is true within a command buffer, and across command buffers submitted to a given queue. 

DECLARE_SHARED_PTR(VulkanCommandBuffer)
class VulkanCommandBuffer final : public VkObject<VkCommandBuffer_T>
{
public:
	enum class EState
	{
		Initial,
		Recording,
		Executable,
		Pending,
		Invalid
	};

	inline EState state() const
	{
		return m_State;
	}

	inline bool8_t isInsideRenderPass() const
	{
		return m_State == EState::Recording;
	}

	void begin()
	{
		assert(isValid() && m_State != EState::Pending && m_State != EState::Recording);

		VkCommandBufferBeginInfo beginInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,   /// pNext must be NULL or a pointer to a valid instance of VkDeviceGroupCommandBufferBeginInfo
			0u,        /// VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
			nullptr
		};
		VERIFY_VK(vkBeginCommandBuffer(get(), &beginInfo));
		setState(EState::Recording);
	}

	void end()
	{
		assert(isValid() && m_State == EState::Recording);
		VERIFY_VK(vkEndCommandBuffer(get()));
		setState(EState::Executable);
	}

	inline void reset()
	{
		/// The command buffer can be in any state other than pending, and is moved into the initial state
		/// Any primary command buffer that is in the recording or executable state and has commandBuffer recorded into it, becomes invalid.
		/// VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT specifies that most or all memory resources currently owned by the command buffer should be returned to the parent command pool. 
		/// If this flag is not set, then the command buffer may hold onto memory resources and reuse them when recording commands.
		assert(m_State != EState::Pending);
		vkResetCommandBuffer(get(), VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
		setState(EState::Initial);
	}

	inline VkCommandBufferLevel level() const
	{
		return m_Level;
	}
protected:
	friend class VulkanCommandPool;

	inline void setState(EState state)
	{
		m_State = state;
	}
private:
	VkCommandBufferLevel m_Level = VK_COMMAND_BUFFER_LEVEL_MAX_ENUM;
	EState m_State = EState::Initial;
};

DECLARE_SHARED_PTR(VulkanCommandPool)
class VulkanCommandPool final : public VkObject<VkCommandPool_T>
{
public:
	VulkanCommandPool(VkDevice device, uint32_t queueIndex, VkCommandPoolCreateFlags flags);

	VulkanCommandBufferPtr alloc(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	void free(VulkanCommandBufferPtr& commandBuffer);

	void reset();

	void destroy()
	{
		/// When a pool is destroyed, all command buffers allocated from the pool are freed.

		/// Any primary command buffer allocated from another VkCommandPool that is in the recording or 
		/// executable state and has a secondary command buffer allocated from commandPool recorded into it, becomes invalid.

		/// All VkCommandBuffer objects allocated from commandPool must not be in the pending state.
		vkDestroyCommandPool(m_Device, get(), VK_MEMORY_ALLOCATOR);
	}
protected:
private:
	std::queue<VulkanCommandBufferPtr> m_CommandBuffers;
	const VkDevice m_Device;
};

NAMESPACE_END(Gfx)