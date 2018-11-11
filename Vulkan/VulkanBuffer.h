#pragma once

#include "VulkanObject.h"

class VulkanDeviceMemory : public VulkanObject<VkDeviceMemory>
{
public:
	void Create(size_t size, uint32_t memoryType, uint32_t flags = 0U);

	void Update(const void *pMemory, size_t size, size_t offset);

	void Destory();
protected:
private:
};

class VulkanBuffer : public VulkanObject<VkBuffer>
{
public:
	void Create(size_t size, VkBufferUsageFlagBits usage);

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
