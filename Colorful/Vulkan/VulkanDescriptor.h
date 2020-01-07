#pragma once

#include "VulkanLoader.h"

class vkDescriptorPool : public vkDeviceObject<VkDescriptorPool>
{
public:
	void destroy(const class vkDevice &device) override final;
};

class vkDescriptorSetLayout : public vkDeviceObject<VkDescriptorSetLayout>
{
public:
	void destroy(const class vkDevice &device) override final;
};

class vkDescriptorUpdateTemplate : public vkDeviceObject<VkDescriptorUpdateTemplate>
{
public:
	void destroy(const class vkDevice &device) override final;
};
