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


class vkCommandBuffer : public vkObject<VkCommandBuffer>
{
public:
	vkCommandBuffer(VkCommandBufferLevel level, VkCommandBuffer handle)
		: m_Level(level)
	{
		assert(handle != VK_NULL_HANDLE);
		reset(handle);
		setState(eInitial);
	}

	void begin();

	void execute(const vkCommandBuffer &primaryCommandBuffer);

	void queueSubmit();

	void end();

	inline void resetBuffer()
	{
		/// The command buffer can be in any state other than pending, and is moved into the initial state

		/// Any primary command buffer that is in the recording or executable state and has commandBuffer recorded into it, becomes invalid.

		/// VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT specifies that most or all memory resources currently owned by the command buffer should be returned to the parent command pool. 
		/// If this flag is not set, then the command buffer may hold onto memory resources and reuse them when recording commands.
		assert(m_State != ePending && isValid());
		vkResetCommandBuffer(**this, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
		setState(eInitial);
	}
protected:
	friend class vkCommandBufferArray;

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
};

class vkCommandBufferArray
{
public:
	inline vkCommandBuffer get(uint32_t index = 0u) const
	{
		assert(m_CommandBuffers.size() > 0u && index < m_CommandBuffers.size());
		return m_CommandBuffers[index];
	}

	inline void begin(uint32_t index)
	{
		assert(index < m_CommandBuffers.size());
		m_CommandBuffers[index].begin();
	}

	inline void end(uint32_t index)
	{
		assert(index < m_CommandBuffers.size());
		m_CommandBuffers[index].end();
	}

	inline void reset(uint32_t index)
	{
		assert(index < m_CommandBuffers.size());
		m_CommandBuffers[index].resetBuffer();
	}

	inline void execute(const vkCommandBuffer &primaryCommandBuffer, uint32_t index)
	{
		assert(index < m_CommandBuffers.size());
		m_CommandBuffers[index].execute(primaryCommandBuffer);
	}

	void execute(const vkCommandBuffer &primaryCommandBuffer);
protected:
	friend class vkCommandPool;

	inline void destory()
	{
		m_CommandBuffers.clear();
	}
private:
	std::vector<vkCommandBuffer> m_CommandBuffers;
};

class vkCommandPool : public vkDeviceObject<VkCommandPool>
{
public:
	void create(const class vkDevice &device, uint32_t queueIndex);
	void destroy(const class vkDevice &device) override final;
	void resetPool(const class vkDevice &device);
	void trimPool(const class vkDevice &device);

	vkCommandBufferArray allocCommandBuffers(const class vkDevice &device, VkCommandBufferLevel level, uint32_t count) const;
	void freeCommandBuffers(const class vkDevice &device, vkCommandBufferArray &commandBuffers) const;

	vkCommandBuffer allocCommandBuffer(const class vkDevice &device, VkCommandBufferLevel level) const;
	void freeCommandBuffer(const class vkDevice &device, vkCommandBuffer &commandBuffer) const;

	inline vkCommandBuffer *getActiveCommandBuffer() const
	{
		return nullptr;
	}
protected:
private:
};
