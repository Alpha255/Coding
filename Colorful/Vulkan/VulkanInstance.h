#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

NAMESPACE_START(Gfx)

template <typename T> static std::vector<const char8_t*> getSupportedProperties(
	const std::vector<T>& supportedProperties, 
	const std::vector<const char8_t*>& targetProperties)
{
	std::vector<const char8_t*> result;

	for each (auto propertyName in targetProperties)
	{
		for (auto it = supportedProperties.begin(); it != supportedProperties.end(); ++it)
		{
			if (_stricmp((const char8_t* const)(&(*it)), propertyName) == 0)
			{
				result.push_back(propertyName);
				break;
			}
		}
	}

	return result;
}

class VulkanDebugUtilsMessenger final : public VkDeviceObject<VkDebugUtilsMessengerEXT_T>
{
public:
	VulkanDebugUtilsMessenger(bool8_t verbose);
	~VulkanDebugUtilsMessenger();
};
DECLARE_UNIQUE_PTR(VulkanDebugUtilsMessenger)

class VulkanInstance final : public VkObject<VkInstance_T>
{
public:
	VulkanInstance();

	~VulkanInstance()
	{
		vkDestroyInstance(get(), VK_MEMORY_ALLOCATOR);
	}
};
DECLARE_UNIQUE_PTR(VulkanInstance)

NAMESPACE_END(Gfx)
