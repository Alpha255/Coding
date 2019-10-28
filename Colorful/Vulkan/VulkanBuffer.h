#pragma once

#include "VulkanObject.h"

class VulkanDeviceMemory : public VulkanObject<VkDeviceMemory>
{
public:
	void Alloc(size_t size, uint32_t memTypeBits, uint32_t usage);
	void Destory() override;

	void Update(const void *pMemory, size_t size = VK_WHOLE_SIZE, size_t offset = 0U);

	void Flush(size_t size = VK_WHOLE_SIZE, size_t offset = 0U);

	void Invalidate(size_t size = VK_WHOLE_SIZE, size_t offset = 0U);
protected:
	uint32_t GetMemoryTypeIndex(uint32_t memTypeBits, uint32_t usage);
private:
	uint32_t m_MemPropertyFlags = UINT32_MAX;
};

class VulkanBuffer : public VulkanObject<VkBuffer>
{
public:
	inline void CreateAsVertexBuffer(
		size_t byteWidth,
		uint32_t usage,
		const void *pData,
		uint32_t usageFlags = 0U)
	{
		Create(byteWidth, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | usageFlags, usage);

		if (pData)
		{
			Update(pData, byteWidth, 0U);
		}
	}

	inline void CreateAsIndexBuffer(
		size_t byteWidth,
		uint32_t usage,
		const void *pData,
		uint32_t usageFlags = 0U)
	{
		Create(byteWidth, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | usageFlags, usage);

		if (pData)
		{
			Update(pData, byteWidth, 0U);
		}
	}

	inline void CreateAsUniformBuffer(
		size_t byteWidth,
		uint32_t usage,
		const void *pData,
		uint32_t usageFlags = 0U)
	{
		Create(byteWidth, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | usageFlags, usage);

		if (pData)
		{
			Update(pData, byteWidth, 0U);
		}
	}

	inline void CreateAsTransferBuffer(size_t byteWidth, uint32_t usage, const void *pData)
	{
		Create(byteWidth, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, usage);

		if (pData)
		{
			Update(pData, byteWidth, 0U);
		}
	}

	inline void Update(const void *pMemory, size_t size = VK_WHOLE_SIZE, size_t offset = 0U)
	{
		m_Memory.Update(pMemory, size, offset);
	}

	inline VkDescriptorBufferInfo GetDescriptorInfo() const
	{
		return m_DescriptorInfo;
	}

	void Destory() override;
protected:
	void Create(size_t size, uint32_t bufferUsageFlags, uint32_t usage);
private:
	VulkanDeviceMemory m_Memory;
	VkDescriptorBufferInfo m_DescriptorInfo = {};
};

class VulkanCommandBuffer
{
public:
	/// VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: The command buffer will be rerecorded right after executing it once
	/// VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT: This is a secondary command buffer that will be entirely within a single render pass
	/// VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT: The command buffer can be resubmitted while it is also already pending execution
	enum eCommandBufferUsage
	{
		eDefault = 0U,
		eOneTimeSubmit = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		eRenderpassContinue = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
		eSimultaneous = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
	};

	enum eCommandBufferResetFlags
	{
		eResetReleaseResourceBit = VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT
	};

	VulkanCommandBuffer() = default;
	VulkanCommandBuffer(uint32_t count)
	{
		m_CommandBuffers.resize(count);
	}
	~VulkanCommandBuffer() = default;

	bool IsValid(uint32_t index = UINT32_MAX)
	{
		bool bValid = true;
		if (index == UINT32_MAX)
		{
			for each (auto commandBuffer in m_CommandBuffers)
			{
				bValid &= (commandBuffer != VK_NULL_HANDLE);
			}
		}
		else
		{
			if (index > m_CommandBuffers.size())
			{
				bValid &= false;
			}
			else
			{
				bValid &= (m_CommandBuffers[index] != VK_NULL_HANDLE);
			}
		}

		return bValid;
	}

	inline VkCommandBuffer *operator &()
	{
		return m_CommandBuffers.data();
	}

	inline const VkCommandBuffer *operator &() const
	{
		return m_CommandBuffers.data();
	}

	inline uint32_t GetBufferCount() const
	{
		return (uint32_t)m_CommandBuffers.size();
	}

	inline VkCommandBuffer Get(uint32_t index) const
	{
		///assert(IsValid(index));
		return m_CommandBuffers[index];
	}

	inline uint32_t GetPoolType() const
	{
		assert(m_PoolType != UINT32_MAX);
		return m_PoolType;
	}

	inline void SetPoolType(uint32_t type)
	{
		m_PoolType = type;
	}

	inline void Begin(eCommandBufferUsage usage, uint32_t index)
	{
		assert(IsValid(index));

		VkCommandBufferBeginInfo beginInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			(VkCommandBufferUsageFlags)usage,
			nullptr
		};

		/// If the command buffer was already recorded once, then a call to vkBeginCommandBuffer
		/// will implicitly reset it.It¡¯s not possible to append commands to a buffer at a later time
		Check(vkBeginCommandBuffer(m_CommandBuffers[index], &beginInfo));
	}

	inline void Execute(VkCommandBuffer primaryBuffer)
	{
		assert(IsValid() && primaryBuffer != VK_NULL_HANDLE);

		vkCmdExecuteCommands(primaryBuffer, (uint32_t)m_CommandBuffers.size(), m_CommandBuffers.data());
	}

	inline void End(uint32_t index)
	{
		assert(IsValid(index));

		Check(vkEndCommandBuffer(m_CommandBuffers[index]));
	}

	inline void Reset(uint32_t index, uint32_t flags)
	{
		assert(IsValid(index));

		Check(vkResetCommandBuffer(m_CommandBuffers[index], (VkCommandBufferResetFlags)flags));
	}

	inline void Clear()
	{
		m_CommandBuffers.clear();
	}
protected:
private:
	std::vector<VkCommandBuffer> m_CommandBuffers;
	uint32_t m_PoolType = UINT32_MAX;
};

class VulkanFrameBuffer : public VulkanObject<VkFramebuffer>
{
public:
	void Create(uint32_t width, uint32_t height, const std::vector<VkImageView> &views);
	void Destory() override;
protected:
private:
};
