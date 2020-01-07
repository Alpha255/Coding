#pragma once

#include "Colorful/Vulkan/VulkanAsync.h"

class vkPipelineLayout : public vkDeviceObject<VkPipelineLayout>
{
public:
	void destroy(const class vkDevice &device) override final;
};

class vkPipelineCache : public vkDeviceObject<VkPipelineCache>
{
public:
	void create(const class vkDevice &device);
	void destroy(const class vkDevice &device) override final;
};

class vkPipeline : public vkDeviceObject<VkPipeline>
{
public:
	void destroy(const class vkDevice &device) override final;
};

class vkGraphicsPipeline : public vkPipeline, public rGraphicsPipeline
{
};

class vkComputePipeline : public vkPipeline
{
};
