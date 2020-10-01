#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

NAMESPACE_START(Gfx)

//class VulkanDeviceMemory final : public VkObject<VkDeviceMemory_T>
//{
//public:
//	VulkanDeviceMemory(VkDevice device, eRBufferUsage usage, const VkMemoryRequirements& requirements);
//
//	void update(VkDevice device, const void* data, size_t size, size_t offset = 0u);
//	
//	//void destroy(VkDevice device) override final
//	//{
//	//	if (isValid())
//	//	{
//	//		vkFreeMemory(device, Handle, vkMemoryAllocator);
//	//		Handle = VK_NULL_HANDLE;
//	//	}
//	//}
//};

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

#if 0
class VulkanPushConstants : public GfxGpuBuffer
{
public:
	void update(const void*, size_t, size_t) override final
	{

	}

	void free() override final
	{

	}
};

class VulkanBufferPool : public LazySingleton<VulkanBufferPool>
{
	lazySingletonDeclare(VulkanBufferPool);
public:
	uint32_t memoryTypeIndex(eRBufferUsage usage, uint32_t memoryTypeBits) const;

	inline VulkanBufferPtr alloc(eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void* data)
	{
		auto buffer = new VulkanBuffer(m_Device, bindFlags, usage, size, data);
		m_Buffers.insert(std::make_pair(buffer->Handle, buffer));
		return buffer;
	}

	inline VulkanBufferPtr allocIndexBuffer(eRBufferUsage usage, size_t size, const void* data)
	{
		return alloc(eIndexBuffer, usage, size, data);
	}

	inline VulkanBufferPtr allocVertexBuffer(eRBufferUsage usage, size_t size, const void* data)
	{
		return alloc(eVertexBuffer, usage, size, data);
	}

	inline VulkanBufferPtr allocUniformBuffer(size_t size, const void* data)
	{
		return alloc(eUniformBuffer, eGpuReadCpuWrite, size, data);
	}

	inline VulkanBufferPtr allocStagingBuffer(VkBufferUsageFlags usageFlagBits, size_t size, const void* data)
	{
		auto buffer = new VulkanBuffer(m_Device, usageFlagBits, size, data);
		m_Buffers.insert(std::make_pair(buffer->Handle, buffer));
		return buffer;
	}
	
	inline void free(VulkanBufferPtr buffer)
	{
		auto it = m_Buffers.find(buffer->Handle);
		if (it != m_Buffers.end())
		{
			m_DelayFreeList[m_ListIndex++] = it->second;
			m_Buffers.erase(it);
		}
		else
		{
			m_DelayFreeList[m_ListIndex++] = buffer;
		}
	}

	inline void updateBuffer(VulkanBufferPtr buffer, const void* data, size_t size, size_t offset)
	{
		buffer->update(m_Device, data, size, offset);
	}

	void delayFree(bool8_t force = false);

	void cleanup() override final;
protected:
	VulkanBufferPool(const VkDevice device, VkPhysicalDevice physicalDevice)
		: m_Device(device)
	{
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &m_DeviceMemoryProperties);
		m_DelayFreeList.resize(eMaxDelayList);
	}

	/// May need linear allocator here for ImGui?
	enum ePoolCapacity
	{
		eMaxDelayList = 128
	};
private:
	const VkDevice m_Device;
	VkPhysicalDeviceMemoryProperties m_DeviceMemoryProperties{};
	std::unordered_map<VkBuffer, VulkanBufferPtr> m_Buffers;
	std::vector<VulkanBufferPtr> m_DelayFreeList;
	uint32_t m_ListIndex = 0u;
};
#endif

NAMESPACE_END(Gfx)