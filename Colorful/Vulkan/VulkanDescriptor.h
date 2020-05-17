#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

class VulkanDescriptorSet : public VulkanObject<VkDescriptorSet>
{
};

class VulkanDescriptorPool : public VulkanDeviceObject<VkDescriptorPool>
{
public:
	void create(VkDevice device);
	void resetPool(VkDevice device);
	void destroy(VkDevice device) override final
	{
		if (isValid())
		{
			vkDestroyDescriptorPool(device, Handle, vkMemoryAllocator);
			Handle = VK_NULL_HANDLE;
		}
	}

	VulkanDescriptorSet alloc(VkDevice device, const class VulkanDescriptorSetLayout &layout) const;
};

class VulkanDescriptorSetLayout : public VulkanDeviceObject<VkDescriptorSetLayout>
{
public:
	void create(VkDevice device, const GfxDescriptorLayoutDesc &desc);
	void destroy(VkDevice device) override final
	{
		if (isValid())
		{
			vkDestroyDescriptorSetLayout(device, Handle, vkMemoryAllocator);
			Handle = VK_NULL_HANDLE;
		}
	}
};
