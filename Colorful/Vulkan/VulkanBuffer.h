#pragma once

#include "VulkanLoader.h"

class vkDeviceMemory : public vkDeviceObject<VkDeviceMemory>
{
public:
	void create(const class vkDevice &device, eRBufferUsage usage, const VkMemoryRequirements &memoryRequirements);
	void destroy(const class vkDevice &device) override final;

	void update(const class vkDevice &device, const void *pData, size_t size = VK_WHOLE_SIZE, size_t offset = 0u);
};

class vkBuffer : public vkDeviceObject<VkBuffer>, public rBuffer
{
public:
	void destroy(const class vkDevice &device) override final;
protected:
	vkDeviceMemory m_Memory;
private:
};

class vkStagingBuffer : public vkBuffer
{
public:
	vkStagingBuffer(const class vkDevice &device, VkBufferUsageFlags usageFlagBits, size_t size, const void *pData);
};

class vkGpuBuffer : public vkBuffer
{
public:
	vkGpuBuffer(const class vkDevice &device, eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void *pData);
	
	inline void update(const class vkDevice &device, const void *data, size_t size, size_t offset)
	{
		m_Memory.update(device, data, size, offset);
	}
};

class vkFrameBuffer : public vkDeviceObject<VkFramebuffer>
{
public:
	void create(const class vkDevice &device, const class vkRenderPass &renderPass, const rFrameBufferDesc &desc);
	void destroy(const class vkDevice &device) override final;
};