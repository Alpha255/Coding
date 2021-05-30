#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

NAMESPACE_START(Gfx)

class VulkanMemoryAllocator : public Gear::NoneCopyable
{
public:
	static VulkanMemoryAllocator& instance();

	void initialize(VkPhysicalDevice device);
	void finalize();

	uint32_t getMemoryTypeIndex(EBufferUsage usage, uint32_t memTypeBits) const
	{
		/// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bit specifies that memory allocated with this type can be mapped for host access using vkMapMemory.

		/// VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bit specifies that the host cache management commands vkFlushMappedMemoryRanges and vkInvalidateMappedMemoryRanges 
		/// are not needed to flush host writes to the device or make device writes visible to the host, respectively.

		/// VK_MEMORY_PROPERTY_HOST_CACHED_BIT bit specifies that memory allocated with this type is cached on the host. 
		/// Host memory accesses to uncached memory are slower than to cached memory, however uncached memory is always host coherent.

		VkMemoryPropertyFlags memPropFlags = 0u;
		switch (usage)
		{
		case EBufferUsage::Default:
		case EBufferUsage::Immutable:
			memPropFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			break;
		case EBufferUsage::Dynamic:
			memPropFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			break;
		case EBufferUsage::Staging:
			memPropFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			break;
		}

		for (uint32_t i = 0u; i < m_MemoryProperties.memoryTypeCount; ++i)
		{
			if (((1 << i) & memTypeBits) &&
				((m_MemoryProperties.memoryTypes[i].propertyFlags & memPropFlags) == memPropFlags))
			{
				return i;
			}
		}

		assert(0);
		return ~0u;
	}
protected:
private:
	VkPhysicalDeviceMemoryProperties m_MemoryProperties;
};

NAMESPACE_END(Gfx)
