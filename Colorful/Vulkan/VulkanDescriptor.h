#pragma once

#include "VulkanLoader.h"

class vkDescriptorPool : public vkDeviceObject<VkDescriptorPool>
{
public:
	void create(const class vkDevice &device);
	void resetPool(const class vkDevice &device);
	void destroy(const class vkDevice &device) override final;

	void allocDescriptorSets();
	void freeDescriptorSets();

	void allocDescriptorSet();
	void freeDescriptorSet();
};

class vkDescriptorSetLayout : public vkDeviceObject<VkDescriptorSetLayout>
{
public:
	void create(const class vkDevice &device, const rDescriptorLayoutDesc &desc);
	void destroy(const class vkDevice &device) override final;
};

class vkDescriptorUpdateTemplate : public vkDeviceObject<VkDescriptorUpdateTemplate>
{
public:
	void destroy(const class vkDevice &device) override final;
};
