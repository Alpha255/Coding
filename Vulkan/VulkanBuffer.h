#pragma once

#include "VulkanObject.h"

class VulkanDeviceMemory : public VulkanObject<VkDeviceMemory>
{
public:
	VulkanDeviceMemory() = default;
	inline ~VulkanDeviceMemory()
	{
		Destory();
	}

	static uint32_t GetMemoryTypeIndex(uint32_t memoryTypeBits, uint32_t memoryPropertyFlagBits);

	void Create(size_t size, uint32_t memoryTypeIndex);

	void Update(const void *pMemory, size_t size, size_t offset);
protected:
	void Destory();
private:
};

class VulkanBuffer : public VulkanObject<VkBuffer>
{
public:
	inline VulkanBuffer() = default;
	inline ~VulkanBuffer()
	{
		Destory();
	}

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
protected:
	void Create(size_t size, uint32_t usage, uint32_t memoryPropertyFlags);
	void Destory();
	inline uint32_t GetMemoryProperty(uint32_t usage)
	{
		switch (usage)
		{
		case eGpuReadWrite:
			return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		case eGpuReadOnly:
		case eGpuReadCpuWrite:
			return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		case eGpuCopyToCpu:
			return 0U;
		default:
			System::Log("Not supported yet!");
			assert(0);
			return 0U;
		}
	}
private:
	VulkanDeviceMemory m_Memory;
};

class VulkanCommandBuffer : public VulkanObject<VkCommandBuffer>
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

		VKCheck(vkBeginCommandBuffer(m_Handle, &beginInfo));
	}

	inline void Execute(VulkanCommandBuffer &primaryCmdBuffer)
	{
		assert(IsValid() && primaryCmdBuffer.IsValid());

		vkCmdExecuteCommands(primaryCmdBuffer.Get(), 1U, &m_Handle);
	}

	inline void End()
	{
		assert(IsValid());

		VKCheck(vkEndCommandBuffer(m_Handle));
	}

	inline void ResetCommand(uint32_t flags)
	{
		assert(IsValid());

		VKCheck(vkResetCommandBuffer(m_Handle, (VkCommandBufferResetFlags)flags));
	}
protected:
private:
};

class VulkanCommandBufferList
{
public:
	inline VulkanCommandBufferList(const std::vector<VkCommandBuffer> &other)
		: m_BufferList(other)
	{
	}

	inline void Execute(VulkanCommandBuffer &primaryCmdBuffer)
	{
		assert(m_BufferList.size() > 0U);

		vkCmdExecuteCommands(primaryCmdBuffer.Get(), (uint32_t)m_BufferList.size(), m_BufferList.data());
	}

	inline uint32_t GetCount() const
	{
		return (uint32_t)m_BufferList.size();
	}

	inline VkCommandBuffer *operator&()
	{
		return m_BufferList.data();
	}

	inline VulkanCommandBuffer operator[](uint32_t index)
	{
		assert(index < m_BufferList.size() && m_BufferList[index] != VK_NULL_HANDLE);
		return VulkanCommandBuffer(m_BufferList[index]);
	}

	inline void Reset()
	{
		m_BufferList.clear();
	}
protected:
private:
	std::vector<VkCommandBuffer> m_BufferList;
};

class VulkanFrameBuffer : public VulkanObject<VkFramebuffer>
{
public:
	inline VulkanFrameBuffer() = default;
	inline ~VulkanFrameBuffer()
	{
		Destory();
	}

	void Create(const std::vector<class VulkanImageView> &imageViews, const class VulkanRenderPass &renderPass, uint32_t width, uint32_t height, uint32_t layers = 1U);
protected:
	void Destory();
private:
};
