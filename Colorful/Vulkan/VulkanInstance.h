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

DECLARE_UNIQUE_PTR(VulkanDebugUtilsMessenger)
class VulkanDebugUtilsMessenger final : public VkObject<VkDebugUtilsMessengerEXT_T>
{
public:
	VulkanDebugUtilsMessenger(VkInstance instance, bool8_t verbose);

	void destroy(VkInstance instance)
	{
		assert(instance);
		PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
			vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
		assert(vkDestroyDebugUtilsMessenger);
		vkDestroyDebugUtilsMessenger(instance, get(), VK_MEMORY_ALLOCATOR);
	}
};

DECLARE_UNIQUE_PTR(VulkanInstance)
class VulkanInstance final : public VkObject<VkInstance_T>
{
public:
	VulkanInstance();

	~VulkanInstance()
	{
		vkDestroyInstance(get(), VK_MEMORY_ALLOCATOR);
	}
};

NAMESPACE_END(Gfx)
