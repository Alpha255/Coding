#pragma once

#include "VulkanObject.h"

class VulkanDeviceMemory : public VulkanObject<VkDeviceMemory>
{
public:
	void Create(size_t size, uint32_t memTypeBits, uint32_t memPropertyFlags);
	void Destory() override;

	void Update(const void *pMemory, size_t size = VK_WHOLE_SIZE, size_t offset = 0U);

	void Flush(size_t size = VK_WHOLE_SIZE, size_t offset = 0U);

	void Invalidate(size_t size = VK_WHOLE_SIZE, size_t offset = 0U);
protected:
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
		uint32_t usageFlag = 0U)
	{
		Create(byteWidth, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | usageFlag, GetMemoryProperty(usage));

		if (pData)
		{
			Update(pData, byteWidth, 0U);
		}
	}

	inline void CreateAsIndexBuffer(
		size_t byteWidth,
		uint32_t usage,
		const void *pData,
		uint32_t usageFlag = 0U)
	{
		Create(byteWidth, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | usageFlag, GetMemoryProperty(usage));

		if (pData)
		{
			Update(pData, byteWidth, 0U);
		}
	}

	inline void CreateAsConstantBuffer(
		size_t byteWidth,
		uint32_t usage,
		const void *pData,
		uint32_t usageFlag = 0U)
	{
		Create(byteWidth, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | usageFlag, GetMemoryProperty(usage));

		if (pData)
		{
			Update(pData, byteWidth, 0U);
		}
	}

	inline void CreateAsTransferBuffer(size_t byteWidth, uint32_t usage, bool bSource = true)
	{
		Create(
			byteWidth, 
			bSource ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			GetMemoryProperty(usage));
	}

	inline void Update(const void *pMemory, size_t size, size_t offset = 0U)
	{
		m_Memory.Update(pMemory, size, offset);
	}

	void Destory() override;
protected:
	void Create(size_t size, uint32_t usage, uint32_t memPropertyFlags);
	inline uint32_t GetMemoryProperty(uint32_t usage)
	{
		switch (usage)
		{
		case eGpuReadWrite:
		case eGpuReadOnly:
			return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;  /// Can't map
		case eGpuReadCpuWrite:
			return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		case eGpuCopyToCpu:
			return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		default:
			assert(0);
			return 0U;
		}
	}
private:
	VulkanDeviceMemory m_Memory;
};

class IVulkanCommandBuffer
{
public:
protected:
	friend class VulkanEngine;

	inline void SetIndex(uint32_t index)
	{
		assert(m_Index == UINT32_MAX);
		m_Index = index;
	}

	inline void SetPoolType(uint32_t type)
	{
		assert(m_PoolType == UINT32_MAX);
		m_PoolType = type;
	}

	inline uint32_t GetIndex() const
	{
		assert(m_Index != UINT32_MAX);
		return m_Index;
	}

	inline uint32_t GetPoolType() const
	{
		assert(m_PoolType != UINT32_MAX);
		return m_PoolType;
	}

	uint32_t m_Index = UINT32_MAX;
	uint32_t m_PoolType = UINT32_MAX;
};

class VulkanCommandBuffer : public VulkanObject<VkCommandBuffer>, public IVulkanCommandBuffer
{
public:
	inline VulkanCommandBuffer() = default;
	inline VulkanCommandBuffer(VkCommandBuffer cmdBuffer)
		: VulkanObject<VkCommandBuffer>(cmdBuffer)
	{
	}

	inline void Begin(uint32_t flags)
	{
		assert(IsValid());

		VkCommandBufferBeginInfo beginInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			(VkCommandBufferUsageFlags)flags,
			nullptr
		};

		Check(vkBeginCommandBuffer(m_Handle, &beginInfo));
	}

	inline void Execute(VulkanCommandBuffer &primaryCmdBuffer)
	{
		assert(IsValid() && primaryCmdBuffer.IsValid());

		vkCmdExecuteCommands(primaryCmdBuffer.Get(), 1U, &m_Handle);
	}

	inline void End()
	{
		assert(IsValid());

		Check(vkEndCommandBuffer(m_Handle));
	}

	inline void ResetCommand(uint32_t flags)
	{
		assert(IsValid());

		Check(vkResetCommandBuffer(m_Handle, (VkCommandBufferResetFlags)flags));
	}
protected:
private:
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
