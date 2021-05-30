#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

NAMESPACE_START(Gfx)

class VulkanBuffer final : public VkDeviceObject<VkBuffer_T>, public GpuBuffer
{
public:
	VulkanBuffer(uint32_t bindFlags, EBufferUsage usage, size_t size, const void* data);
	~VulkanBuffer();

	virtual void unmap() override final;

	void invalidateMappedRange(size_t size = VK_WHOLE_SIZE, size_t offset = 0u);
protected:
	std::pair<size_t, size_t> insureMappedRanges(size_t size, size_t offset);

	void* map_native(size_t size, size_t offset) override final;
	void update_native(const void* data, size_t size, size_t offset, bool8_t persistence) override final;
	void flushMappedRange_native(size_t size, size_t offset) override final;
private:
	VkDeviceMemory m_Memory = VK_NULL_HANDLE;
};
DECLARE_SHARED_PTR(VulkanBuffer)

NAMESPACE_END(Gfx)