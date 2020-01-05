#pragma once

#include "VulkanLoader.h"

class vkDescriptorSetLayout : public vkDeviceObject<VkDescriptorSetLayout>
{
public:
	void destroy(const class vkDevice &device) override final;
};
