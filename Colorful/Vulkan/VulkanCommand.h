#pragma once

#include "VulkanLoader.h"

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


class vkCommandBuffer : public VulkanObject<VkCommandBuffer>
{
public:
	vkCommandBuffer() = default;
	vkCommandBuffer(VkCommandBufferLevel level, VkCommandBuffer handle, class VulkanFence *fence, class vkSemaphore *semaphore)
		: m_Level(level)
		, m_Fence(fence)
		, m_Semaphore(semaphore)
	{
		assert(handle != VK_NULL_HANDLE);
		setState(eInitial);
		Handle = handle;
	}

	void beginRenderPass(const VkRenderPassBeginInfo &renderPassBeginInfo, VkSubpassContents subpassContents);
	void endRenderPass();

	void begin();
	void end();

	void execute(const vkCommandBuffer &primaryCommandBuffer);

	inline bool8_t isInsideRenderPass() const
	{
		return m_State == eRecording;
	}

	inline class VulkanFence *getFence() const
	{
		assert(m_Fence);
		return m_Fence;
	}

	inline class vkSemaphore *getWaitSemaphore() const
	{
		assert(m_Semaphore);
		return m_Semaphore;
	}

	void waitFence(const class VulkanDevice &device);

	inline void resetCommand()
	{
		/// The command buffer can be in any state other than pending, and is moved into the initial state

		/// Any primary command buffer that is in the recording or executable state and has commandBuffer recorded into it, becomes invalid.

		/// VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT specifies that most or all memory resources currently owned by the command buffer should be returned to the parent command pool. 
		/// If this flag is not set, then the command buffer may hold onto memory resources and reuse them when recording commands.
		//assert(m_State != ePending && isValid());
		vkResetCommandBuffer(Handle, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
		setState(eInitial);
	}
protected:
	friend class vkCommandPool;
	friend class vkDeviceQueue;
	friend class vkEngine;
	enum eState
	{
		eInitial,
		eRecording,
		eExecutable,
		ePending,
		eInvalid,
		eState_MaxEnum
	};

	inline void setState(eState state)
	{
		m_State = state;
	}
private:
	VkCommandBufferLevel m_Level = VK_COMMAND_BUFFER_LEVEL_MAX_ENUM;
	eState m_State = eState_MaxEnum;
	class VulkanFence *m_Fence;
	class vkSemaphore *m_Semaphore = nullptr;
};

class vkCommandPool : public VulkanDeviceObject<VkCommandPool>
{
public:
	void create(const class VulkanDevice &device, uint32_t queueIndex);
	void destroy(const class VulkanDevice &device) override final;
	void reset(const class VulkanDevice &device);
	void trim(const class VulkanDevice &device);

	vkCommandBuffer alloc(const class VulkanDevice &device, VkCommandBufferLevel level, bool8_t signaleFence = true) const;
	void free(const class VulkanDevice &device, vkCommandBuffer &commandBuffer) const;

	vkCommandBuffer *getActiveCommandBuffer(const class VulkanDevice &device);
protected:
private:
	vkCommandBuffer *m_ActiveCmdBuffer = nullptr;
};
