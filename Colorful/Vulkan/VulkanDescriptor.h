#pragma once

#include "VulkanLoader.h"

class vkDescriptorSet : public VulkanObject<VkDescriptorSet>
{
};

class vkDescriptorPool : public VulkanDeviceObject<VkDescriptorPool>
{
public:
	void create(const class VulkanDevice &device);
	void resetPool(const class VulkanDevice &device);
	void destroy(const class VulkanDevice &device) override final;

	vkDescriptorSet alloc(const class VulkanDevice &device, const class vkDescriptorSetLayout &layout) const;
};

class vkDescriptorSetLayout : public VulkanDeviceObject<VkDescriptorSetLayout>
{
public:
	void create(const class VulkanDevice &device, const rDescriptorLayoutDesc &desc);
	void destroy(const class VulkanDevice &device) override final;
};

class vkDescriptorUpdateTemplate : public VulkanDeviceObject<VkDescriptorUpdateTemplate>
{
public:
	void destroy(const class VulkanDevice &device) override final;
};
