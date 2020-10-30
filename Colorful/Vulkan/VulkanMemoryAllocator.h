#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

NAMESPACE_START(Gfx)

class VulkanMemoryAllocator : public TLazySingleton<VulkanMemoryAllocator>
{
public:
	VulkanMemoryAllocator(VkDevice device, VkPhysicalDevice physicalDevice)
		: m_Device(device)
	{
		assert(device && physicalDevice);
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &m_MemoryProperties);
	}

	uint32_t getMemoryTypeIndex(EBufferUsage usage, uint32_t memTypeBits) const
	{
		VkMemoryPropertyFlags memPropFlags = 0u;
		switch (usage)
		{
		case EBufferUsage::Default:
		case EBufferUsage::Immutable:
			memPropFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			break;
		case EBufferUsage::Dynamic:
			memPropFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			break;
		case EBufferUsage::Staging:
			memPropFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
			break;
		}

		for (uint32_t i = 0u; i < m_MemoryProperties.memoryTypeCount; ++i)
		{
			if (((1 << i) & memTypeBits) && 
				((m_MemoryProperties.memoryTypes[i].propertyFlags & memTypeBits) == memTypeBits))
			{
				return i;
			}
		}

		assert(0);
		return std::numeric_limits<uint32_t>().max();
	}
protected:
private:
	const VkDevice m_Device;
	VkPhysicalDeviceMemoryProperties m_MemoryProperties{};
};

NAMESPACE_END(Gfx)
