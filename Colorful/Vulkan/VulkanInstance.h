#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

NAMESPACE_START(Gfx)

class VulkanDebugUtilsMessenger : public SharedObject<VkDebugUtilsMessengerEXT_T>
{
public:
	void create(VkInstance instance, bool8_t verbose);
	void destroy(VkInstance instance);
};

class VulkanInstance : public SharedObject<VkInstance_T>
{
public:
	void create(bool8_t verbose);
	void destroy();
protected:
private:
	VulkanDebugUtilsMessenger m_DebugUtilsMessenger;
};

NAMESPACE_END(Gfx)
