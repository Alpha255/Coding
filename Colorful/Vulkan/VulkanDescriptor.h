#pragma once

#include "VulkanLoader.h"

class vkDescriptorSet : public VulkanObject<VkDescriptorSet>
{
};

class vkDescriptorPool : public VulkanDeviceObject<VkDescriptorPool>
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

	vkDescriptorSet alloc(VkDevice device, const class vkDescriptorSetLayout &layout) const;
};

class vkDescriptorSetLayout : public VulkanDeviceObject<VkDescriptorSetLayout>
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

class vkDescriptorUpdateTemplate : public VulkanDeviceObject<VkDescriptorUpdateTemplate>
{
public:
	void destroy(VkDevice device) override final
	{	
		/// A descriptor update template specifies a mapping from descriptor update information in host memory to descriptors in a descriptor set.
		/// It is designed to avoid passing redundant information to the driver when frequently updating the same set of descriptors in descriptor sets.
		/// For cases when an application wishes to update the same set of descriptors in multiple descriptor sets allocated using the same VkDescriptorSetLayout, 
		/// vkUpdateDescriptorSetWithTemplate can be used as a replacement for vkUpdateDescriptorSets.

		if (isValid())
		{
			///vkDestroyDescriptorUpdateTemplate(device, Handle, vkMemoryAllocator);
			Handle = VK_NULL_HANDLE;
		}
	}
};
