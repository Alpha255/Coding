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
		const rColorBlendState * const clrBlendState) override final;

	void setDepthStencilState(
		bool8_t enableDepth,
		bool8_t enableDepthWrite,
		eRCompareOp depthCompareOp,
		bool8_t enableStencil,
		uint8_t stencilReadMask,
		uint8_t stencilWriteMask,
		const rStencilOp &front,
		const rStencilOp &back) override final;

	void create(
		const class vkDevice &device, 
		const class vkRenderPass &renderpass, 
		const vkPipelineCache &cache, 
		const vkPipelineLayout &layout);
protected:
private:
	VkPipelineRasterizationStateCreateInfo m_RasterizationState{};
	VkPipelineDepthStencilStateCreateInfo m_DepthStencilState{};

	VkPipelineColorBlendStateCreateInfo m_ColorBlendState{};
	VkPipelineColorBlendAttachmentState m_ColorBlendAttachmentState[eMaxRenderTargets]{};
};

class vkComputePipeline : public vkPipeline, public rComputePipeline
{
};

class vkRayTracingPipeline : public vkPipeline
{
};
