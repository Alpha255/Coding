#include "VulkanPipeline.h"
#include "VulkanDescriptor.h"
#include "VulkanShader.h"
#include "VulkanRenderPass.h"
#include "VulkanEngine.h"

void vkPipelineLayout::create(const vkDevice &device, const vkDescriptorSetLayout &descriptorSetLayout)
{
	assert(device.isValid() && !isValid() && descriptorSetLayout.isValid());

	VkDescriptorSetLayout descriptorSetLayoutHandle = *descriptorSetLayout;
	VkPipelineLayoutCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0u,
		1u,
		&descriptorSetLayoutHandle,
		0u,
		nullptr /// PushRange ?????
	};

	VkPipelineLayout handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreatePipelineLayout(*device, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);
}

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
	eRLogicOp logicOp, 
	uint32_t attachmentCount, 
	const rColorBlendState * const clrBlendState)
{
	assert(m_ColorBlendState.sType == 0);
	assert(attachmentCount <= eMaxRenderTargets);

	for (uint32_t i = 0u; i < attachmentCount; ++i)
	{
		assert(clrBlendState);
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

void vkGraphicsPipeline::create(
	const vkDevice &device, 
	const vkRenderPass &renderpass, 
	const vkPipelineCache &cache)
{
	assert(device.isValid() && !isValid());
	assert(m_Shaders[eVertexShader]);

	rDescriptorLayoutDesc descriptorLayoutDesc{};
	std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
	for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
	{
		if (m_Shaders[i])
		{
			auto shader = static_cast<const vkShader *>(m_Shaders[i]);

			VkPipelineShaderStageCreateInfo shaderStageCreateInfo
			{
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				nullptr,
				0u,
				vkEngine::enumTranslator::toShaderStage(shader->getUsage()),
				**shader,
				"main",
				nullptr
			};
			shaderStageCreateInfos.emplace_back(std::move(shaderStageCreateInfo));

			descriptorLayoutDesc[i] = shader->getReflections();
		}
	}

	m_DescriptorSetLayout.create(device, descriptorLayoutDesc);
	m_PipelineLayout.create(device, m_DescriptorSetLayout);

	auto vertexShader = static_cast<const vkShader *>(m_Shaders[eVertexShader]);

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		nullptr,
		0u,
		vkEngine::enumTranslator::toPrimitiveTopology(m_PrimitiveTopology),
		false
	};

	//VkPipelineTessellationStateCreateInfo tessellationStateCreateInfo
	//{
	//};

	/// Set viewport dynamic
	VkPipelineViewportStateCreateInfo viewportStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		nullptr,
		0u,
		1u,
		nullptr,
		1u,
		nullptr
	};

	if (m_RasterizationState.sType == 0)
	{
		setRasterizerState(eRPolygonMode::eSolid, eRCullMode::eCullNone, eRFrontFace::eClockwise);
	}

	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		nullptr,
		0u,
		VK_SAMPLE_COUNT_1_BIT,
		false,
		0.0f,
		nullptr,
		false,
		false
	};

	if (m_DepthStencilState.sType == 0)
	{
		rStencilOp stencilOp
		{
			eRStencilOp::eKeep,
			eRStencilOp::eKeep,
			eRStencilOp::eKeep,
			eRCompareOp::eAlways
		};
		setDepthStencilState(
			false,
			false,
			eRCompareOp::eAlways,
			false,
			0u,
			0u,
			stencilOp,
			stencilOp
		);
	}

	if (m_ColorBlendState.sType == 0)
	{
		setBlendState(false, eRLogicOp::eAnd, 0u, nullptr);
	}

	std::vector<VkDynamicState> dynamicStates
	{ 
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR 
	};
	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		nullptr,
		0u,
		(uint32_t)dynamicStates.size(),
		dynamicStates.data()
	};

	VkGraphicsPipelineCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		nullptr,
		0u,
		(uint32_t)shaderStageCreateInfos.size(),
		shaderStageCreateInfos.data(),
		&vertexShader->getInputLayout(),
		&inputAssemblyStateCreateInfo,
		nullptr,
		&viewportStateCreateInfo,
		&m_RasterizationState,
		&multisampleStateCreateInfo,
		&m_DepthStencilState,
		&m_ColorBlendState,
		&dynamicStateCreateInfo,
		*m_PipelineLayout,
		*renderpass,
		0u,
		VK_NULL_HANDLE,
		0u
	};

	/// Pending creations ???
	VkPipeline handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateGraphicsPipelines(*device, *cache, 1u, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);
}

void vkGraphicsPipeline::destroy(const vkDevice &device)
{
	m_PipelineLayout.destroy(device);
	m_DescriptorSetLayout.destroy(device);
	vkPipeline::destroy(device);
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
