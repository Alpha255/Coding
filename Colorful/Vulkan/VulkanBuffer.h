#pragma once

#include "VulkanLoader.h"

class vkDeviceMemory : public VulkanDeviceObject<VkDeviceMemory>
{
public:
	void create(const class VulkanDevice &device, eRBufferUsage usage, const VkMemoryRequirements &memoryRequirements);
	void destroy(const class VulkanDevice &device) override final;

	void update(const class VulkanDevice &device, const void *pData, size_t size = VK_WHOLE_SIZE, size_t offset = 0u);
};

class vkBuffer : public VulkanDeviceObject<VkBuffer>, public GfxGpuBuffer
{
public:
	void destroy(const class VulkanDevice &device) override final;
protected:
	vkDeviceMemory m_Memory;
private:
};

class vkStagingBuffer : public vkBuffer
{
public:
	vkStagingBuffer(const class VulkanDevice &device, VkBufferUsageFlags usageFlagBits, size_t size, const void *pData);
};

class vkGpuBuffer : public vkBuffer
{
public:
	vkGpuBuffer(const class VulkanDevice &device, eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void *pData);
	
	inline void update(const class VulkanDevice &device, const void *data, size_t size, size_t offset)
	{
		m_Memory.update(device, data, size, offset);
	}
};

class vkFrameBuffer : public VulkanDeviceObject<VkFramebuffer>
{
public:
	void create(const class VulkanDevice &device, const class vkRenderPass &renderPass, const GfxFrameBufferDesc &desc);
	void destroy(const class VulkanDevice &device) override final;
};