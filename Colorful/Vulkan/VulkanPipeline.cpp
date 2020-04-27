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

void vkGraphicsPipeline::create(
	const vkDevice &device, 
	const vkRenderPass &renderpass, 
	const vkPipelineCache &cache, 
	const rGraphicsPipelineState &state)
{
	assert(device.isValid() && !isValid());
	assert(state.Shaders[eVertexShader]);
	assert(renderpass.isValid() && cache.isValid());

	rDescriptorLayoutDesc descriptorLayoutDesc{};
	std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
	for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
	{
		if (state.Shaders[i])
		{
			auto shader = static_cast<const vkShader *>(state.Shaders[i]);

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
	setupDescriptorSet(device, state);

	auto vertexShader = static_cast<const vkShader *>(state.Shaders[eVertexShader]);

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		nullptr,
		0u,
		vkEngine::enumTranslator::toPrimitiveTopology(state.PrimitiveTopology),
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

	std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
	auto depthStencilState = getDepthStencilState(state.DepthStencilStateDesc);
	auto rasterizationState = getRasterizationState(state.RasterizerStateDesc);
	auto blendState = getColorBlendState(colorBlendAttachments, state.BlendStateDesc);

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
		&rasterizationState,
		&multisampleStateCreateInfo,
		&depthStencilState,
		&blendState,
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

void vkGraphicsPipeline::bind(const vkCommandBuffer &cmdBuffer)
{
	assert(isValid() && cmdBuffer.isValid() && m_DescriptorSet.isValid());

	VkDescriptorSet descriptorSet = *m_DescriptorSet;
	vkCmdBindDescriptorSets(*cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_PipelineLayout, 0u, 1u, &descriptorSet, 0u, nullptr);

	vkCmdBindPipeline(*cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, **this);
}

VkPipelineRasterizationStateCreateInfo vkGraphicsPipeline::getRasterizationState(const rRasterizerStateDesc &stateDesc) const
{
	assert(stateDesc.PolygonMode < eRPolygonMode_MaxEnum);
	assert(stateDesc.CullMode < eRCullMode_MaxEnum);
	assert(stateDesc.FrontFace < eRFrontFace_MaxEnum);

	VkPipelineRasterizationStateCreateInfo rasterizationState
	{
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		nullptr,
		0u, /// flags is reserved for future use
		false,
		false,
		vkEngine::enumTranslator::toPolygonMode(stateDesc.PolygonMode),
		vkEngine::enumTranslator::toCullMode(stateDesc.CullMode),
		vkEngine::enumTranslator::toFrontFace(stateDesc.FrontFace),
		stateDesc.EnableDepthBias,
		stateDesc.DepthBias,
		stateDesc.DepthBiasClamp,
		stateDesc.DepthBiasSlope,
		0.0f
	};

	return rasterizationState;
}

VkPipelineDepthStencilStateCreateInfo vkGraphicsPipeline::getDepthStencilState(const rDepthStencilStateDesc &stateDesc) const
{
	VkPipelineDepthStencilStateCreateInfo depthStencilState
	{
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		nullptr,
		0u,  /// flags is reserved for future use.
		stateDesc.EnableDepth,
		stateDesc.EnableDepthWrite,
		vkEngine::enumTranslator::toCompareOp(stateDesc.DepthCompareOp),
		false,  /// depthBoundsTestEnable?
		stateDesc.EnableStencil,
		{
			vkEngine::enumTranslator::toStencilOp(stateDesc.FrontFace.FailOp),
			vkEngine::enumTranslator::toStencilOp(stateDesc.FrontFace.PassOp),
			vkEngine::enumTranslator::toStencilOp(stateDesc.FrontFace.DepthFailOp),
			vkEngine::enumTranslator::toCompareOp(stateDesc.FrontFace.CompareOp),
			stateDesc.StencilReadMask,
			stateDesc.StencilWriteMask,
			0u
		},
		{
			vkEngine::enumTranslator::toStencilOp(stateDesc.BackFace.FailOp),
			vkEngine::enumTranslator::toStencilOp(stateDesc.BackFace.PassOp),
			vkEngine::enumTranslator::toStencilOp(stateDesc.BackFace.DepthFailOp),
			vkEngine::enumTranslator::toCompareOp(stateDesc.BackFace.CompareOp),
			stateDesc.StencilReadMask,
			stateDesc.StencilWriteMask,
			0u
		},
		0.0f,
		1.0f
	};

	return depthStencilState;
}

VkPipelineColorBlendStateCreateInfo vkGraphicsPipeline::getColorBlendState(
	std::vector<VkPipelineColorBlendAttachmentState> &attachments,
	const rBlendStateDesc &stateDesc) const
{
	for (uint32_t i = 0u; i < eMaxRenderTargets; ++i)
	{
		if (stateDesc.ColorBlendStateDesc[i].Enable)
		{
			VkPipelineColorBlendAttachmentState attachment
			{
				stateDesc.ColorBlendStateDesc[i].Enable,
				vkEngine::enumTranslator::toBlendFactor(stateDesc.ColorBlendStateDesc[i].SrcColor),
				vkEngine::enumTranslator::toBlendFactor(stateDesc.ColorBlendStateDesc[i].DstColor),
				vkEngine::enumTranslator::toBlendOp(stateDesc.ColorBlendStateDesc[i].ColorOp),
				vkEngine::enumTranslator::toBlendFactor(stateDesc.ColorBlendStateDesc[i].SrcAlpha),
				vkEngine::enumTranslator::toBlendFactor(stateDesc.ColorBlendStateDesc[i].DstAlpha),
				vkEngine::enumTranslator::toBlendOp(stateDesc.ColorBlendStateDesc[i].AlphaOp),
				vkEngine::enumTranslator::toColorComponentFlags(stateDesc.ColorBlendStateDesc[i].ColorMask)
			};
			attachments.emplace_back(std::move(attachment));
		}
	}

	if (attachments.size() == 0u)
	{
		VkPipelineColorBlendAttachmentState attachment
		{
			false,
			VK_BLEND_FACTOR_ONE,
			VK_BLEND_FACTOR_ZERO,
			VK_BLEND_OP_ADD,
			VK_BLEND_FACTOR_ONE,
			VK_BLEND_FACTOR_ZERO,
			VK_BLEND_OP_ADD,
			0xF
		};
		attachments.emplace_back(std::move(attachment));
	}
	VkPipelineColorBlendStateCreateInfo blendState
	{
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		nullptr,
		0u,  /// flags is reserved for future use.
		stateDesc.EnableLogicOp,
		vkEngine::enumTranslator::toLogicOp(stateDesc.LogicOp),
		(uint32_t)attachments.size(),
		attachments.data()
	};

	return blendState;
}

void vkGraphicsPipeline::setupDescriptorSet(const vkDevice &device, const rGraphicsPipelineState &state)
{
	m_DescriptorSet = device.allocDescriptorSet(m_DescriptorSetLayout);

	for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
	{
		if (state.Shaders[i])
		{
			auto uniformBuffer = state.Shaders[i]->getUniformBuffer();
			if (uniformBuffer)
			{
				auto buffer = static_cast<vkBuffer *>(uniformBuffer);

				VkDescriptorBufferInfo bufferInfo
				{
					**buffer,
					buffer->getOffset(),
					buffer->getSize()
				};

				VkWriteDescriptorSet writeDescriptorSet
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					nullptr,
					*m_DescriptorSet,
					0u,
					0u,
					1u,
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					nullptr,
					&bufferInfo,
					nullptr
				};
				vkUpdateDescriptorSets(*device, 1u, &writeDescriptorSet, 0u, nullptr);
			}
		}
	}
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
