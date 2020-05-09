#pragma once

#include "VulkanLoader.h"

class VulkanBuffer : public VulkanDeviceObject<VkBuffer>, public GfxGpuBuffer
{
public:
	void destroy(VkDevice device) override final;
protected:
	class VulkanDeviceMemory : public VulkanObject<VkDeviceMemory>
	{
	public:
		void create(VkDevice device, eRBufferUsage usage, const VkMemoryRequirements& memoryRequirements);
		void update(VkDevice device, const void* data, size_t size, size_t offset = 0u);
	};

	VulkanDeviceMemory m_Memory;
private:
};

class VulkanStagingBuffer : public VulkanBuffer
{
public:
	VulkanStagingBuffer(VkDevice device, VkBufferUsageFlags usageFlagBits, size_t size, const void *pData);
};

class VulkanGpuBuffer : public VulkanBuffer
{
public:
	VulkanGpuBuffer(VkDevice device, eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void* data);
	
	inline void update(VkDevice device, const void* data, size_t size, size_t offset)
	{
		m_Memory.update(device, data, size, offset);
	}
};
using VulkanGpuBufferPtr = std::shared_ptr<VulkanGpuBuffer>;

class VulkanFrameBuffer : public VulkanDeviceObject<VkFramebuffer>
{
public:
	void create(VkDevice device, const class vkRenderPass &renderPass, const GfxFrameBufferDesc &desc);
	void destroy(VkDevice device) override final;
};

class VulkanBufferPool : public LazySingleton<VulkanBufferPool>
{
	lazySingletonDeclare(VulkanBufferPool);
public:
	uint32_t memoryTypeIndex(eRBufferUsage usage, uint32_t memoryTypeBits) const;

	inline VulkanGpuBufferPtr alloc(eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void* data)
	{

	}

	inline VulkanGpuBufferPtr allocIndexBuffer(eRBufferUsage usage, size_t size, const void* data)
	{
		return alloc(eIndexBuffer, usage, size, data);
	}

	inline VulkanGpuBufferPtr allocVertexBuffer(eRBufferUsage usage, size_t size, const void* data)
	{
		return alloc(eVertexBuffer, usage, size, data);
	}

	inline VulkanGpuBufferPtr allocUniformBuffer(size_t size, const void* data)
	{
		return alloc(eUniformBuffer, eGpuReadCpuWrite, size, data);
	}
	
	inline void free(VulkanGpuBufferPtr& buffer)
	{
		m_DelayFreeList.emplace_back(buffer);
	}

	void cleanup() override final;
protected:
	VulkanBufferPool(const VkDevice device, VkPhysicalDevice physicalDevice)
		: m_Device(device)
	{
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &m_DeviceMemoryProperties);
	}
private:
	const VkDevice m_Device;
	VkPhysicalDeviceMemoryProperties m_DeviceMemoryProperties{};
	std::unordered_map<VkBuffer, VulkanGpuBufferPtr> m_Buffers;
	std::vector<VulkanGpuBufferPtr> m_DelayFreeList;
};