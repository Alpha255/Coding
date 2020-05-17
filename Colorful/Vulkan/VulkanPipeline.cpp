#include "Colorful/Vulkan/VulkanEngine.h"

void VulkanPipelineLayout::create(VkDevice device, const VulkanDescriptorSetLayout &descriptorSetLayout)
{
	assert(!isValid() && descriptorSetLayout.isValid());

	VkPipelineLayoutCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0u,
		1u,
		&descriptorSetLayout.Handle,
		0u,
		nullptr /// PushRange ?????
	};

	GfxVerifyVk(vkCreatePipelineLayout(device, &createInfo, vkMemoryAllocator, &Handle));
}

VulkanGraphicsPipeline::VulkanGraphicsPipeline(
	VkDevice device, 
	VkRenderPass renderPass,
	VkPipelineCache pipelineCache,
	const GfxPipelineState& state)
{
	assert(!isValid());
	assert(state.Shaders[eVertexShader]);
	assert(renderPass != VK_NULL_HANDLE && pipelineCache != VK_NULL_HANDLE);

	GfxDescriptorLayoutDesc descriptorLayoutDesc{};
	std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
	for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
	{
		if (state.Shaders[i])
		{
			auto shader = std::static_pointer_cast<VulkanShader>(state.Shaders[i]);

			VkPipelineShaderStageCreateInfo shaderStageCreateInfo
			{
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				nullptr,
				0u,
				VulkanEnum::toShaderStage(shader->usage()),
				shader->Handle,
				"main",
				nullptr
			};
			shaderStageCreateInfos.emplace_back(std::move(shaderStageCreateInfo));

			descriptorLayoutDesc[i] = shader->reflections();
		}
	}

	m_DescriptorSetLayout.create(device, descriptorLayoutDesc);
	m_PipelineLayout.create(device, m_DescriptorSetLayout);
	setupDescriptorSet(device, state);

	auto vertexShader = std::static_pointer_cast<VulkanShader>(state.Shaders[eVertexShader]);

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		nullptr,
		0u,
		VulkanEnum::toPrimitiveTopology(state.PrimitiveTopology),
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
	auto depthStencilState = makeDepthStencilState(state.DepthStencilStateDesc);
	auto rasterizationState = makeRasterizationState(state.RasterizerStateDesc);
	auto blendState = makeColorBlendState(colorBlendAttachments, state.BlendStateDesc);

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
		&vertexShader->inputLayout(),
		&inputAssemblyStateCreateInfo,
		nullptr,
		&viewportStateCreateInfo,
		&rasterizationState,
		&multisampleStateCreateInfo,
		&depthStencilState,
		&blendState,
		&dynamicStateCreateInfo,
		m_PipelineLayout.Handle,
		renderPass,
		0u,
		VK_NULL_HANDLE,
		0u
	};

	/// Pending creations ???
	GfxVerifyVk(vkCreateGraphicsPipelines(device, pipelineCache, 1u, &createInfo, vkMemoryAllocator, &Handle));
}

void VulkanGraphicsPipeline::bind(const VulkanCommandBuffer &cmdBuffer)
{
	assert(isValid() && cmdBuffer.isValid() && m_DescriptorSet.isValid());

	vkCmdBindDescriptorSets(cmdBuffer.Handle, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout.Handle, 0u, 1u, &m_DescriptorSet.Handle, 0u, nullptr);

	vkCmdBindPipeline(cmdBuffer.Handle, VK_PIPELINE_BIND_POINT_GRAPHICS, Handle);
}

VkPipelineRasterizationStateCreateInfo VulkanGraphicsPipeline::makeRasterizationState(const GfxRasterizerStateDesc& stateDesc) const
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
		VulkanEnum::toPolygonMode(stateDesc.PolygonMode),
		VulkanEnum::toCullMode(stateDesc.CullMode),
		VulkanEnum::toFrontFace(stateDesc.FrontFace),
		stateDesc.EnableDepthBias,
		stateDesc.DepthBias,
		stateDesc.DepthBiasClamp,
		stateDesc.DepthBiasSlope,
		0.0f
	};

	return rasterizationState;
}

VkPipelineDepthStencilStateCreateInfo VulkanGraphicsPipeline::makeDepthStencilState(const GfxDepthStencilStateDesc& stateDesc) const
{
	VkPipelineDepthStencilStateCreateInfo depthStencilState
	{
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		nullptr,
		0u,  /// flags is reserved for future use.
		stateDesc.EnableDepth,
		stateDesc.EnableDepthWrite,
		VulkanEnum::toCompareOp(stateDesc.DepthCompareOp),
		false,  /// depthBoundsTestEnable?
		stateDesc.EnableStencil,
		{
			VulkanEnum::toStencilOp(stateDesc.FrontFace.FailOp),
			VulkanEnum::toStencilOp(stateDesc.FrontFace.PassOp),
			VulkanEnum::toStencilOp(stateDesc.FrontFace.DepthFailOp),
			VulkanEnum::toCompareOp(stateDesc.FrontFace.CompareOp),
			stateDesc.StencilReadMask,
			stateDesc.StencilWriteMask,
			0u
		},
		{
			VulkanEnum::toStencilOp(stateDesc.BackFace.FailOp),
			VulkanEnum::toStencilOp(stateDesc.BackFace.PassOp),
			VulkanEnum::toStencilOp(stateDesc.BackFace.DepthFailOp),
			VulkanEnum::toCompareOp(stateDesc.BackFace.CompareOp),
			stateDesc.StencilReadMask,
			stateDesc.StencilWriteMask,
			0u
		},
		0.0f,
		1.0f
	};

	return depthStencilState;
}

VkPipelineColorBlendStateCreateInfo VulkanGraphicsPipeline::makeColorBlendState(
	std::vector<VkPipelineColorBlendAttachmentState>& attachments,
	const GfxBlendStateDesc& stateDesc) const
{
	for (uint32_t i = 0u; i < eMaxRenderTargets; ++i)
	{
		if (stateDesc.ColorBlendStates[i].Enable)
		{
			VkPipelineColorBlendAttachmentState attachment
			{
				stateDesc.ColorBlendStates[i].Enable,
				VulkanEnum::toBlendFactor(stateDesc.ColorBlendStates[i].SrcColor),
				VulkanEnum::toBlendFactor(stateDesc.ColorBlendStates[i].DstColor),
				VulkanEnum::toBlendOp(stateDesc.ColorBlendStates[i].ColorOp),
				VulkanEnum::toBlendFactor(stateDesc.ColorBlendStates[i].SrcAlpha),
				VulkanEnum::toBlendFactor(stateDesc.ColorBlendStates[i].DstAlpha),
				VulkanEnum::toBlendOp(stateDesc.ColorBlendStates[i].AlphaOp),
				VulkanEnum::toColorComponentFlags(stateDesc.ColorBlendStates[i].ColorMask)
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
		VulkanEnum::toLogicOp(stateDesc.LogicOp),
		(uint32_t)attachments.size(),
		attachments.data()
	};

	return blendState;
}

void VulkanGraphicsPipeline::setupDescriptorSet(VkDevice device, const GfxPipelineState& state)
{
	//m_DescriptorSet = device.allocDescriptorSet(m_DescriptorSetLayout);

	//std::vector<VkWriteDescriptorSet> writeDescriptorSets;
	//std::vector<VkDescriptorImageInfo> imageInfos;
	//uint32_t bindingIndex = 0u;
	//for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
	//{
	//	if (state.Shaders[i])
	//	{
	//		auto uniformBuffer = state.Shaders[i]->uniformBuffer();
	//		if (uniformBuffer)
	//		{
	//			auto buffer = static_cast<VulkanBuffer *>(uniformBuffer);

	//			VkDescriptorBufferInfo bufferInfo
	//			{
	//				buffer->Handle,
	//				0u,
	//				VK_WHOLE_SIZE   /// Use whole size for now
	//			};

	//			VkWriteDescriptorSet writeDescriptorSetUniforBuffer
	//			{
	//				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
	//				nullptr,
	//				m_DescriptorSet.Handle,
	//				bindingIndex++,  /// Need get binding from shader reflection
	//				0u,
	//				1u,   
	//				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	//				nullptr,
	//				&bufferInfo,
	//				nullptr
	//			};
	//			writeDescriptorSets.emplace_back(std::move(writeDescriptorSetUniforBuffer));
	//		}

	//		auto &textures = state.Shaders[i]->getTextures();
	//		for (uint32_t j = 0u; j < textures.size(); ++j)
	//		{
	//			auto imageView = static_cast<const VulkanImageView *>(textures[j]);
	//			const VulkanSampler *sampler = nullptr;
	//			auto combinedSampler = imageView->getSampler();
	//			if (combinedSampler)
	//			{
	//				sampler = static_cast<const VulkanSampler *>(combinedSampler);
	//			}

	//			VkDescriptorImageInfo imageInfo
	//			{
	//				sampler ? sampler->Handle : VK_NULL_HANDLE,
	//				imageView->Handle,
	//				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	//			};
	//			imageInfos.emplace_back(std::move(imageInfo));

	//			VkWriteDescriptorSet writeDescriptorSetImageView
	//			{
	//				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
	//				nullptr,
	//				m_DescriptorSet.Handle,
	//				bindingIndex++,
	//				0u,
	//				1u,
	//				sampler ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER : VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
	//				&imageInfos[imageInfos.size() - 1u],
	//				nullptr,
	//				nullptr,
	//			};
	//			writeDescriptorSets.emplace_back(std::move(writeDescriptorSetImageView));
	//		}
	//	}
	//}

	//assert(writeDescriptorSets.size() > 0u);
	//vkUpdateDescriptorSets(device, (uint32_t)writeDescriptorSets.size(), writeDescriptorSets.data(), 0u, nullptr);
}

void VulkanPipelineCache::create(VkDevice device)
{
	assert(!isValid());

	VkPipelineCacheCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
		nullptr,
		0u,   /// flags is reserved for future use
		0u,
		nullptr
	};

	GfxVerifyVk(vkCreatePipelineCache(device, &createInfo, vkMemoryAllocator, &Handle));
}

VulkanGraphicsPipelinePtr VulkanPipelinePool::getOrCreateGfxPipeline(VkRenderPass renderPass, const GfxPipelineState& state)
{
	for each (auto &it in m_GfxPipelines)
	{
		if (it.first == state)
		{
			return it.second;
		}
	}

	auto gfxPipeline = std::make_shared<VulkanGraphicsPipeline>(m_Device, renderPass, m_PipelineCache.Handle, state);
	m_GfxPipelines.emplace_back(std::make_pair(state, gfxPipeline));
	return gfxPipeline;
}

void VulkanPipelinePool::cleanup()
{
	for each (auto &it in m_GfxPipelines)
	{
		it.second->destroy(m_Device);
	}
	m_GfxPipelines.clear();

	m_PipelineCache.destroy(m_Device);
}
