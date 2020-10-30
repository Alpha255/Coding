#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

NAMESPACE_START(Gfx)

DECLARE_SHARED_PTR(VulkanBuffer)
class VulkanBuffer final : public VkObject<VkBuffer_T>
{
public:
	VulkanBuffer(VkDevice device, uint32_t bindFlags, EBufferUsage usage, size_t size, const void* data);

	void update(const void* data, size_t size, size_t offset);

	void flushRange(size_t size, size_t offset);

	void destroy(VkDevice device)
	{
		assert(device);

		vkFreeMemory(device, m_Memory, VK_MEMORY_ALLOCATOR);
		vkDestroyBuffer(device, get(), VK_MEMORY_ALLOCATOR);
	}

	VkDeviceMemory memory() const
	{
		return m_Memory;
	}

	//void update(VkDevice device, const void* data, size_t size, size_t offset)
	//{
	//	m_Memory.update(device, data, size, offset);
	//}

	///void update(const void* data, size_t size, size_t offset) override final;

	///void free() override final;
protected:
private:
	VkDeviceMemory m_Memory = VK_NULL_HANDLE;
};

NAMESPACE_END(Gfx)