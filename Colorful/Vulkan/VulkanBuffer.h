#pragma once

#include "Colorful/Vulkan/VulkanContext.h"

class vkDeviceMemory : public vkDeviceObject<VkDeviceMemory>
{
public:
	void create(const class vkDevice &device, eRBufferUsage usage, uint32_t memoryTypeBits, size_t size);
	void destroy(const class vkDevice &device) override final;
};

class vkBuffer : public vkDeviceObject<VkBuffer>
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
	void create(const class vkDevice &device, VkBufferUsageFlagBits usageFlagBits, size_t size);
};

class vkGpuBuffer : public vkBuffer, public rBuffer
{
public:
	vkGpuBuffer(const class vkDevice &device, eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void *pData);
};