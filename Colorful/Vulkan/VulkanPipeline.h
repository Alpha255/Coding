#pragma once

#include "Colorful/Vulkan/VulkanAsync.h"
#include "Colorful/Vulkan/VulkanDescriptor.h"

class vkPipelineLayout : public VulkanDeviceObject<VkPipelineLayout>
{
public:
	void create(const class VulkanDevice &device, const vkDescriptorSetLayout &descriptorSetLayout);
	void destroy(const class VulkanDevice &device) override final;
};

class vkPipelineCache : public VulkanDeviceObject<VkPipelineCache>
{
public:
	void create(const class VulkanDevice &device);
	void destroy(const class VulkanDevice &device) override final;
};

class vkPipeline : public VulkanDeviceObject<VkPipeline>, public rGpuResource
{
public:
	void destroy(const class VulkanDevice &device) override;
};

class vkGraphicsPipeline : public vkPipeline
{
public:
	void create(
		const class VulkanDevice &device,
		const class vkRenderPass &renderpass,
		const vkPipelineCache &cache,
		const rGraphicsPipelineState &state);

	void destroy(const class VulkanDevice &device) override final;

	void bind(const class vkCommandBuffer &cmdBuffer);
protected:
	VkPipelineRasterizationStateCreateInfo getRasterizationState(const rRasterizerStateDesc &stateDesc) const;
	VkPipelineDepthStencilStateCreateInfo getDepthStencilState(const rDepthStencilStateDesc &stateDesc) const;
	VkPipelineColorBlendStateCreateInfo getColorBlendState(
		std::vector<VkPipelineColorBlendAttachmentState> &attachments, 
		const rBlendStateDesc &stateDesc) const;

	void setupDescriptorSet(const class VulkanDevice &device, const rGraphicsPipelineState &state);
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
