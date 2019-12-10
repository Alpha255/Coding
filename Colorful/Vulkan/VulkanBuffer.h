#pragma once

#include "Colorful/Vulkan/VulkanContext.h"

class vkDeviceMemory : public vkDeviceObject<VkDeviceMemory>
{
public:
	void create(const class vkDevice &device, eRBufferUsage usage, const VkMemoryRequirements &memoryRequirements);
	void destroy(const class vkDevice &device) override final;

	void update(const class vkDevice &device, const void *pData, size_t size = VK_WHOLE_SIZE, size_t offset = 0u);
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
	void create(const class vkDevice &device, VkBufferUsageFlagBits usageFlagBits, size_t size, const void *pData);
};

class vkGpuBuffer : public vkBuffer, public rBuffer
{
public:
	vkGpuBuffer(const class vkDevice &device, eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void *pData);
};