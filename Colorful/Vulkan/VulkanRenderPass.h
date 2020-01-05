#pragma once

#include "VulkanLoader.h"

class vkRenderPass : public vkDeviceObject<VkRenderPass>
{
public:
	void destroy(const class vkDevice &device) override final;
};