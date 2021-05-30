#include "Colorful/Vulkan/VulkanMemoryAllocator.h"
#include "Colorful/Vulkan/VulkanDevice.h"

NAMESPACE_START(Gfx)

VulkanMemoryAllocator& VulkanMemoryAllocator::instance()
{
	static VulkanMemoryAllocator s_Instance;
	return s_Instance;
}

void VulkanMemoryAllocator::initialize(VkPhysicalDevice device)
{
	assert(device);
	vkGetPhysicalDeviceMemoryProperties(device, &m_MemoryProperties);
}

void VulkanMemoryAllocator::finalize()
{
}

NAMESPACE_END(Gfx)