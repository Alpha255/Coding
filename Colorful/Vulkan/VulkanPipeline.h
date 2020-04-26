#pragma once

#include "Colorful/Vulkan/VulkanAsync.h"
#include "Colorful/Vulkan/VulkanDescriptor.h"

class vkPipelineLayout : public vkDeviceObject<VkPipelineLayout>
{
public:
	void create(const class vkDevice &device, const vkDescriptorSetLayout &descriptorSetLayout);
	void destroy(const class vkDevice &device) override final;
};

class vkPipelineCache : public vkDeviceObject<VkPipelineCache>
{
public:
	void create(const class vkDevice &device);
	void destroy(const class vkDevice &device) override final;
};

class vkPipeline : public vkDeviceObject<VkPipeline>, public rGpuResource
{
public:
	void destroy(const class vkDevice &device) override;
};

class vkGraphicsPipeline : public vkPipeline
{
public:
	void create(
		const class vkDevice &device,
		const class vkRenderPass &renderpass,
		const vkPipelineCache &cache,
		const rGraphicsPipelineState &state);

	void destroy(const class vkDevice &device) override final;

	void bind(const class vkCommandBuffer &cmdBuffer);
protected:
	VkPipelineRasterizationStateCreateInfo getRasterizationState(const rRasterizerStateDesc &stateDesc) const;
	VkPipelineDepthStencilStateCreateInfo getDepthStencilState(const rDepthStencilStateDesc &stateDesc) const;
	VkPipelineColorBlendStateCreateInfo getColorBlendState(
		VkPipelineColorBlendAttachmentState attachments[eMaxRenderTargets], 
		const rBlendStateDesc &stateDesc) const;
private:
	vkPipelineLayout m_PipelineLayout;
	vkDescriptorSetLayout m_DescriptorSetLayout;
	vkDescriptorSet m_DescriptorSet;
};

class vkComputePipeline : public vkPipeline, public rComputePipeline
{
};

class vkRayTracingPipeline : public vkPipeline
{
};
