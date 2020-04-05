#include "VulkanPipeline.h"
#include "VulkanEngine.h"

void vkPipelineLayout::destroy(const vkDevice &device)
{
	/// The pipeline layout represents a sequence of descriptor sets with each having a specific layout. 
	/// This sequence of layouts is used to determine the interface between shader stages and shader resources. 
	/// Each pipeline is created using a pipeline layout.
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyPipelineLayout(*device, **this, vkMemoryAllocator);
		reset();
	}
}

void vkGraphicsPipeline::setRasterizerState(
	eRPolygonMode polygonMode,
	eRCullMode cullMode,
	eRFrontFace frontFace,
	bool8_t enableDepthBias,
	float32_t depthBias,
	float32_t depthBiasClamp,
	float32_t depthBiasSlope)
{
	assert(polygonMode < eRPolygonMode_MaxEnum);
	assert(cullMode < eRCullMode_MaxEnum);
	assert(frontFace < eRFrontFace_MaxEnum);
	assert(m_RasterizationState.sType == 0);

	m_RasterizationState = VkPipelineRasterizationStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		nullptr,
		0u, /// flags is reserved for future use
		false,
		false,
		vkEngine::enumTranslator::toPolygonMode(polygonMode),
		vkEngine::enumTranslator::toCullMode(cullMode),
		vkEngine::enumTranslator::toFrontFace(frontFace),
		enableDepthBias,
		depthBias,
		depthBiasClamp,
		depthBiasSlope,
		0.0f
	};
}

void vkGraphicsPipeline::setBlendState(
	bool8_t enableLogicOp, 
	eLogicOp logicOp, 
	uint32_t attachmentCount, 
	const rColorBlendState * const clrBlendState)
{
	assert(m_ColorBlendState.sType == 0);
	assert(attachmentCount <= eMaxRenderTargets);

	for (uint32_t i = 0u; i < attachmentCount; ++i)
	{
		m_ColorBlendAttachmentState[i] = VkPipelineColorBlendAttachmentState
		{
			clrBlendState[i].Enable,
			vkEngine::enumTranslator::toBlendFactor(clrBlendState[i].SrcColor),
			vkEngine::enumTranslator::toBlendFactor(clrBlendState[i].DstColor),
			vkEngine::enumTranslator::toBlendOp(clrBlendState[i].ColorOp),
			vkEngine::enumTranslator::toBlendFactor(clrBlendState[i].SrcAlpha),
			vkEngine::enumTranslator::toBlendFactor(clrBlendState[i].DstAlpha),
			vkEngine::enumTranslator::toBlendOp(clrBlendState[i].AlphaOp),
			vkEngine::enumTranslator::toColorComponentFlags(clrBlendState[i].ColorMask)
		};
	}

	m_ColorBlendState = VkPipelineColorBlendStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		nullptr,
		0u,  /// flags is reserved for future use.
		enableLogicOp,
		vkEngine::enumTranslator::toLogicOp(logicOp),
		attachmentCount,
		m_ColorBlendAttachmentState
	};
}

void vkGraphicsPipeline::setDepthStencilState(
	bool8_t enableDepth, 
	bool8_t enableDepthWrite, 
	eRCompareOp depthCompareOp, 
	bool8_t enableStencil, 
	uint8_t stencilReadMask, 
	uint8_t stencilWriteMask, 
	const rStencilOp &front, 
	const rStencilOp &back)
{
	assert(m_DepthStencilState.sType == 0);

	m_DepthStencilState = VkPipelineDepthStencilStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		nullptr,
		0u,  /// flags is reserved for future use.
		enableDepth,
		enableDepthWrite,
		vkEngine::enumTranslator::toCompareOp(depthCompareOp),
		false,  /// depthBoundsTestEnable?
		enableStencil,
		{
			vkEngine::enumTranslator::toStencilOp(front.FailOp),
			vkEngine::enumTranslator::toStencilOp(front.PassOp),
			vkEngine::enumTranslator::toStencilOp(front.DepthFailOp),
			vkEngine::enumTranslator::toCompareOp(front.CompareOp),
			stencilReadMask,
			stencilWriteMask,
			0u
		},
		{
			vkEngine::enumTranslator::toStencilOp(back.FailOp),
			vkEngine::enumTranslator::toStencilOp(back.PassOp),
			vkEngine::enumTranslator::toStencilOp(back.DepthFailOp),
			vkEngine::enumTranslator::toCompareOp(back.CompareOp),
			stencilReadMask,
			stencilWriteMask,
			0u
		},
		0.0f,
		1.0f
	};
}

void vkPipelineCache::create(const vkDevice &device)
{
	assert(device.isValid() && !isValid());

	VkPipelineCacheCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
		nullptr,
		0u,   /// flags is reserved for future use
		0u,
		nullptr
	};

	VkPipelineCache handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreatePipelineCache(*device, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);
}

void vkPipelineCache::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyPipelineCache(*device, **this, vkMemoryAllocator);
		reset();
	}
}

void vkPipeline::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyPipeline(*device, **this, vkMemoryAllocator);
		reset();
	}
}
