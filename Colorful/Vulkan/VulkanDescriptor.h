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
	void destroy(VkDevice device) override final;

	vkDescriptorSet alloc(VkDevice device, const class vkDescriptorSetLayout &layout) const;
};

class vkDescriptorSetLayout : public VulkanDeviceObject<VkDescriptorSetLayout>
{
public:
	void create(VkDevice device, const GfxDescriptorLayoutDesc &desc);
	void destroy(VkDevice device) override final;
};

class vkDescriptorUpdateTemplate : public VulkanDeviceObject<VkDescriptorUpdateTemplate>
{
public:
	void destroy(VkDevice device) override final;
};
