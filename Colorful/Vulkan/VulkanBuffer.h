#pragma once

#include "VulkanLoader.h"

class VulkanDeviceMemory : public VulkanDeviceObject<VkDeviceMemory>
{
public:
	void create(VkDevice device, eRBufferUsage usage, const VkMemoryRequirements& memoryRequirements);
	void update(VkDevice device, const void* data, size_t size, size_t offset = 0u);
	
	void destroy(VkDevice device) override final
	{
		if (isValid())
		{
			vkFreeMemory(device, Handle, vkMemoryAllocator);
			Handle = VK_NULL_HANDLE;
		}
	}
};

class VulkanBuffer : public VulkanDeviceObject<VkBuffer>, public GfxGpuBuffer
{
public:
	VulkanBuffer(VkDevice device, eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void* data);
	VulkanBuffer(VkDevice device, VkBufferUsageFlags usageFlagBits, size_t size, const void* data);

	void destroy(VkDevice device) override final;

	inline void update(VkDevice device, const void* data, size_t size, size_t offset)
	{
		m_Memory.update(device, data, size, offset);
	}

	void update(const void* data, size_t size, size_t offset) override final;
	void free() override final;
protected:
private:
	VulkanDeviceMemory m_Memory;
};
using VulkanBufferPtr = VulkanBuffer*;

class VulkanPushConstants : public GfxGpuBuffer
{
public:
	void update(const void* data, size_t size, size_t offset) override final
	{

	}

	void free() override final
	{

	}
};

class VulkanFrameBuffer : public VulkanDeviceObject<VkFramebuffer>
{
public:
	void create(VkDevice device, const class VulkanRenderPass &renderPass, const GfxFrameBufferDesc &desc);
	void destroy(VkDevice device) override final;
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
	
	inline void free(VulkanBufferPtr buffer)
	{
		auto it = m_Buffers.find(buffer->Handle);
		if (it != m_Buffers.end())
		{
			m_Buffers.erase(it);
			m_DelayFreeList[m_ListIndex++] = it->second;
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