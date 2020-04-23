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

class vkPipeline : public vkDeviceObject<VkPipeline>
{
public:
	void destroy(const class vkDevice &device) override;
};

class vkGraphicsPipeline : public vkPipeline, public rGraphicsPipeline
{
public:
	void setRasterizerState(
		eRPolygonMode polygonMode,
		eRCullMode cullMode,
		eRFrontFace frontFace,
		bool8_t enableDepthBias = false,
		float32_t depthBias = 0.0f,
		float32_t depthBiasClamp = 0.0f,
		float32_t depthBiasSlope = 0.0f) override final;

	void setBlendState(
		bool8_t enableLogicOp,
		eRLogicOp logicOp,
		uint32_t attachmentCount,
		const rColorBlendStateDesc * const clrBlendState) override final;

	void setDepthStencilState(
		bool8_t enableDepth,
		bool8_t enableDepthWrite,
		eRCompareOp depthCompareOp,
		bool8_t enableStencil,
		uint8_t stencilReadMask,
		uint8_t stencilWriteMask,
		const rStencilOpDesc &front,
		const rStencilOpDesc &back) override final;

	void create(
		const class vkDevice &device, 
		const class vkRenderPass &renderpass, 
		const vkPipelineCache &cache);

	void destroy(const class vkDevice &device) override final;
protected:
private:
	VkPipelineRasterizationStateCreateInfo m_RasterizationState{};
	VkPipelineDepthStencilStateCreateInfo m_DepthStencilState{};

	VkPipelineColorBlendStateCreateInfo m_ColorBlendState{};
	VkPipelineColorBlendAttachmentState m_ColorBlendAttachmentState[eMaxRenderTargets]{};

	vkPipelineLayout m_PipelineLayout;
	vkDescriptorSetLayout m_DescriptorSetLayout;
};

class vkComputePipeline : public vkPipeline, public rComputePipeline
{
};

class vkRayTracingPipeline : public vkPipeline
{
};
