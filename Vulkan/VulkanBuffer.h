#pragma once

#include "VulkanObject.h"

class VulkanDeviceMemory : public VulkanObject<VkDeviceMemory>
{
public:
	static uint32_t GetMemoryType(uint32_t memoryTypeBits, uint32_t memoryPropertyFlagBits);

	void Create(size_t size, uint32_t memoryType, uint32_t flags = 0U);

	void Update(const void *pMemory, size_t size, size_t offset);

	void Destory();
protected:
private:
};

class VulkanBuffer : public VulkanObject<VkBuffer>
{
public:
	void Create(size_t size, uint32_t usage);

	void Destory();

	inline void Update(const void *pMemory, size_t size, size_t offset = 0U)
	{
		m_Memory.Update(pMemory, size, offset);
	}
protected:
private:
	VulkanDeviceMemory m_Memory;
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
	void Create(uint32_t width, uint32_t height, const class VulkanRenderPass renderPass, const class VulkanTexture2D &tex);
protected:
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
