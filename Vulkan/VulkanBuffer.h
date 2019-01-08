#pragma once

#include "VulkanObject.h"

class VulkanDeviceMemory : public VulkanObject<VkDeviceMemory>
{
public:
	VulkanDeviceMemory() = default;
	inline ~VulkanDeviceMemory()
	{
		Free();
	}

	static uint32_t GetMemoryTypeIndex(uint32_t memoryTypeBits, uint32_t memoryPropertyFlagBits);

	void Alloc(size_t size, uint32_t memoryTypeIndex);

	void Update(const void *pMemory, size_t size, size_t offset);

	void Free();
protected:
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
		uint32_t,
		const void *pData,
		uint32_t = 0U)
	{
		Create(byteWidth, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		Update(pData, byteWidth, 0U);
	}

	inline void CreateAsIndexBuffer(
		size_t byteWidth,
		uint32_t,
		const void *pData,
		uint32_t = 0U)
	{
		Create(byteWidth, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		Update(pData, byteWidth, 0U);
	}

	inline void CreateAsConstantBuffer(
		size_t byteWidth,
		uint32_t,
		const void *pData,
		uint32_t = 0U)
	{
		Create(byteWidth, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		if (pData)
		{
			Update(pData, byteWidth, 0U);
		}
	}

	inline void CreateAsSrcDynamicBuffer(size_t byteWidth)
	{
		Create(byteWidth, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	}

	void Destory();

	inline void Update(const void *pMemory, size_t size, size_t offset = 0U)
	{
		m_Memory.Update(pMemory, size, offset);
	}
protected:
	void Create(size_t size, uint32_t usage, uint32_t memoryPropertyFlags);
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

};
