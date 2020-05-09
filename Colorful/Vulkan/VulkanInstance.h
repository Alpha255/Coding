#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

class VulkanInstance : public VulkanObject<VkInstance>
{
public:
	void create(const Gear::Configurations& config);
	void destroy();
protected:
	class VulkanDebugUtilsMessenger : public VulkanObject<VkDebugUtilsMessengerEXT>
	{
	public:
		void create(VkInstance instance, bool8_t verbose);
		void destroy(VkInstance instance);
	};
private:
	VulkanDebugUtilsMessenger m_DebugUtilsMessenger;
};
