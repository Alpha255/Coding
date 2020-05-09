#pragma once

#include "Colorful/Vulkan/VulkanAsync.h"
#include "Colorful/Vulkan/VulkanDescriptor.h"

class vkPipelineLayout : public VulkanDeviceObject<VkPipelineLayout>
{
public:
	void create(VkDevice device, const vkDescriptorSetLayout &descriptorSetLayout);
	void destroy(VkDevice device) override final;
};

class vkPipelineCache : public VulkanDeviceObject<VkPipelineCache>
{
public:
	void create(VkDevice device);
	void destroy(VkDevice device) override final;
};

class vkPipeline : public VulkanDeviceObject<VkPipeline>, public rGpuResource
{
public:
	void destroy(VkDevice device) override;
};

class vkGraphicsPipeline : public vkPipeline
{
public:
	void create(
		VkDevice device,
		const class vkRenderPass &renderpass,
		const vkPipelineCache &cache,
		const GfxPipelineState &state);

	void destroy(VkDevice device) override final;

	void bind(const class vkCommandBuffer &cmdBuffer);
protected:
	VkPipelineRasterizationStateCreateInfo getRasterizationState(const GfxRasterizerStateDesc &stateDesc) const;
	VkPipelineDepthStencilStateCreateInfo getDepthStencilState(const GfxDepthStencilStateDesc &stateDesc) const;
	VkPipelineColorBlendStateCreateInfo getColorBlendState(
		std::vector<VkPipelineColorBlendAttachmentState> &attachments, 
		const GfxBlendStateDesc &stateDesc) const;

	void setupDescriptorSet(VkDevice device, const GfxPipelineState &state);
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
