#pragma once

#include "VulkanObject.h"

class VulkanDeviceMemory : public VulkanObject<VkDeviceMemory>
{
public:
	void Create(size_t size, uint32_t memTypeBits, uint32_t usage);
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

	inline void CreateAsConstantBuffer(
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

	//inline void CreateAsTransferBuffer(size_t byteWidth, uint32_t usage, bool bSource = true)
	//{
	//	Create(
	//		byteWidth, 
	//		bSource ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : VK_BUFFER_USAGE_TRANSFER_DST_BIT,
	//		GetMemoryProperty(usage));
	//}

	inline void Update(const void *pMemory, size_t size = VK_WHOLE_SIZE, size_t offset = 0U)
	{
		m_Memory.Update(pMemory, size, offset);
	}

	void Destory() override;
protected:
	void Create(size_t size, uint32_t bufferUsageFlags, uint32_t usage);
private:
	VulkanDeviceMemory m_Memory;
};

class VulkanCommandBuffer
{
public:
	enum eCommandBufferUsage
	{
		eOneTimeSubmit = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		eRenderpassContinue = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
		eSimultaneous = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
	};

	enum eCommandBufferResetFlags
	{
		eResetReleaseResourceBit = VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT
	};

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
};

#if 0
class VulkanFrameBuffer : public VulkanObject<VkFramebuffer>
{
public:
	void Create(const std::vector<class VulkanImageView> &imageViews, const class VulkanRenderPass &renderPass, uint32_t width, uint32_t height, uint32_t layers = 1U);
	void Destory();
protected:
private:
};
#endif
