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

	static uint32_t GetMemoryType(uint32_t memoryTypeBits, uint32_t memoryPropertyFlagBits);

	void Alloc(size_t size, uint32_t memoryType);

	void Update(const void *pMemory, size_t size, size_t offset);

	void Free();
protected:
private:
};

class VulkanBuffer : public VulkanObject<VkBuffer>
{
public:
	inline void CreateAsVertexBuffer(
		size_t byteWidth,
		uint32_t usage,
		const void *pData,
		uint32_t bindFlags = 0U)
	{
		Create(byteWidth, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

		Update(pData, byteWidth, 0U);
	}

	inline void CreateAsIndexBuffer(
		size_t byteWidth,
		uint32_t usage,
		const void *pData,
		uint32_t bindFlags = 0U)
	{
		Create(byteWidth, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

		Update(pData, byteWidth, 0U);
	}

	inline void CreateAsConstantBuffer(
		size_t byteWidth,
		uint32_t usage,
		const void *pData,
		uint32_t bindFlags = 0U)
	{
		Create(byteWidth, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

		Update(pData, byteWidth, 0U);
	}

	void Destory();

	inline void Update(const void *pMemory, size_t size, size_t offset = 0U)
	{
		m_Memory.Update(pMemory, size, offset);
	}
protected:
	void Create(size_t size, uint32_t usage);
private:
	VulkanDeviceMemory m_Memory;
};

class VulkanCommandBuffer : public VulkanObject<VkCommandBuffer>
{
public:
	void Create(const class VulkanCommandPool &pool, uint32_t level, uint32_t count);

	void Begin(uint32_t flags);

	void End();
protected:
private:
};

class VulkanUniformBuffer : public VulkanBuffer
{
public:
	inline void Create(size_t size)
	{
		VulkanBuffer::Create(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	}
protected:
private:
};

class VulkanFrameBuffer : public VulkanObject<VkFramebuffer>
{
public:
protected:
	void Create(uint32_t width, uint32_t height, const class VulkanRenderPass renderPass, const class VulkanTexture2D &tex);
private:
};

class VulkanVertexBuffer : public VulkanBuffer
{
public:
	inline void Create(size_t size)
	{
		VulkanBuffer::Create(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	}
protected:
private:
};

class VulkanIndexBuffer : public VulkanBuffer
{
public:
	inline void Create(size_t size)
	{
		VulkanBuffer::Create(size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	}
protected:
private:
};
