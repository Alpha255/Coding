#pragma once

#include "VulkanLoader.h"

class vkRenderPass : public vkDeviceObject<VkRenderPass>, public rRenderpass
{
public:
	void destroy(const class vkDevice &device) override final;
};