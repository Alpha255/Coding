#pragma once

#include "VulkanObject.h"

class VulkanBuffer : public VulkanObject<VkBuffer>
{
public:
	VulkanBuffer() = default;
	~VulkanBuffer() = default;

	void Create(size_t size, VkBufferUsageFlagBits usage);
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
