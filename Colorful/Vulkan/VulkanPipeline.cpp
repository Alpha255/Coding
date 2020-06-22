#include "Colorful/Vulkan/VulkanEngine.h"

void VulkanPipelineLayout::create(VkDevice device, VkDescriptorSetLayout descriptorSetLayout)
{
	assert(!isValid() && descriptorSetLayout != VK_NULL_HANDLE);

	VkPipelineLayoutCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0u,
		1u,
		&descriptorSetLayout,
		0u,
		nullptr /// PushRange ?????
	};

	GfxVerifyVk(vkCreatePipelineLayout(device, &createInfo, vkMemoryAllocator, &Handle));
}

VulkanGraphicsPipeline::VulkanGraphicsPipeline(
	VkDevice device, 
	VkRenderPass renderPass,
	VkPipelineCache pipelineCache,
	const GfxPipelineState* state)
{
	assert(!isValid());
	assert(state && state->Shaders[eVertexShader]);
	assert(renderPass != VK_NULL_HANDLE && pipelineCache != VK_NULL_HANDLE);

	GfxDescriptorLayoutDesc descriptorLayoutDesc{};
	std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
	for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
	{
		if (state->Shaders[i])
		{
			auto shader = std::static_pointer_cast<VulkanShader>(state->Shaders[i]);

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

	initShaderResourceMap(descriptorLayoutDesc);
	m_DescriptorSetLayout.create(device, descriptorLayoutDesc);
	m_PipelineLayout.create(device, m_DescriptorSetLayout.Handle);

	auto vertexShader = std::static_pointer_cast<VulkanShader>(state->Shaders[eVertexShader]);

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		nullptr,
		0u,
		VulkanEnum::toPrimitiveTopology(state->PrimitiveTopology),
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
	auto depthStencilState = makeDepthStencilState(state->DepthStencilStateDesc);
	auto rasterizationState = makeRasterizationState(state->RasterizerStateDesc);
	auto blendState = makeColorBlendState(colorBlendAttachments, state->BlendStateDesc);

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

	if (rasterizationState.polygonMode == VK_POLYGON_MODE_FILL)
	{
		rasterizationState.polygonMode = VK_POLYGON_MODE_LINE;
		GfxVerifyVk(vkCreateGraphicsPipelines(device, pipelineCache, 1u, &createInfo, vkMemoryAllocator, &m_WireframePipeline.Handle));
	}

	for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
	{
		if (state->Shaders[i])
		{
			auto shader = std::static_pointer_cast<VulkanShader>(state->Shaders[i]);
			shader->destroy(device);
		}
	}
}

void VulkanGraphicsPipeline::updateDescriptorSet(const GfxPipelineState* state)
{
	assert(state && isValid());

	size_t resourceHash = 0u;
#if defined(UsingUnorderedMap)
	for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
	{
		auto resources = state->ResourceMap[i];
		for (auto &res : resources)
		{
			assert(m_ResourceMap.find(res.first) != m_ResourceMap.end());
			switch (res.second.Type)
			{
			case GfxPipelineState::eTexture:
			{
				auto imageView = std::static_pointer_cast<VulkanImageView>(res.second.Texture);
				assert(imageView); /// Using default???
				m_ResourceMap[res.first].ImageView = imageView->Handle;
				hash_combine(resourceHash, (size_t)imageView->Handle);
			}
				break;
			case GfxPipelineState::eSampler:
			{
				auto sampler = std::static_pointer_cast<VulkanSampler>(res.second.Sampler);
				assert(sampler);
				m_ResourceMap[res.first].Sampler = sampler->Handle;
				hash_combine(resourceHash, (size_t)sampler->Handle);
			}
				break;
			case GfxPipelineState::eCombinedTextureSampler:
			{
				auto imageView = std::static_pointer_cast<VulkanImageView>(res.second.Texture);
				auto sampler = std::static_pointer_cast<VulkanSampler>(res.second.Sampler);
				assert(imageView && sampler);
				m_ResourceMap[res.first].CombinedImageSampler.ImageView = imageView->Handle;
				m_ResourceMap[res.first].CombinedImageSampler.Sampler = sampler->Handle;
				hash_combine(resourceHash, (size_t)imageView->Handle);
				hash_combine(resourceHash, (size_t)sampler->Handle);
			}
				break;
			case GfxPipelineState::eUniformBuffer:
			{
				auto uniformBuffer = static_cast<VulkanBufferPtr>(res.second.UniformBuffer);
				assert(uniformBuffer);
				m_ResourceMap[res.first].Buffer = uniformBuffer->Handle;
				hash_combine(resourceHash, (size_t)uniformBuffer->Handle);
			}
				break;
			}
		}
	}

	auto it = m_DescriptorSets.find(resourceHash);
	if (it != m_DescriptorSets.end())
	{
		m_CurDescriptorSet = it->second;
		return;
	}

	auto descriptorSet = VulkanMainDescriptorPool::instance()->alloc(m_DescriptorSetLayout.Handle, m_ResourceMap);
	m_DescriptorSets.insert(std::make_pair(resourceHash, descriptorSet));
	m_CurDescriptorSet = descriptorSet;
#else
	for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
	{
		auto resources = state->ResourceMap[i];
		for (uint32_t j = 0u; j < resources.size(); ++j)
		{
			assert(findByBinding(resources, resources[j].Binding) != std::numeric_limits<uint32_t>::max());
			switch (resources[j].Type)
			{
			case GfxPipelineState::eTexture:
			{
				auto imageView = std::static_pointer_cast<VulkanImageView>(resources[j].Texture);
				assert(imageView); /// Using default???
				m_ResourceMap[resources[j].Binding].ImageView = imageView->Handle;
				hash_combine(resourceHash, (size_t)imageView->Handle);
			}
			break;
			case GfxPipelineState::eSampler:
			{
				auto sampler = std::static_pointer_cast<VulkanSampler>(resources[j].Sampler);
				assert(sampler);
				m_ResourceMap[resources[j].Binding].Sampler = sampler->Handle;
				hash_combine(resourceHash, (size_t)sampler->Handle);
			}
			break;
			case GfxPipelineState::eCombinedTextureSampler:
			{
				auto imageView = std::static_pointer_cast<VulkanImageView>(resources[j].Texture);
				auto sampler = std::static_pointer_cast<VulkanSampler>(resources[j].Sampler);
				assert(imageView && sampler);
				m_ResourceMap[resources[j].Binding].CombinedImageSampler.ImageView = imageView->Handle;
				m_ResourceMap[resources[j].Binding].CombinedImageSampler.Sampler = sampler->Handle;
				hash_combine(resourceHash, (size_t)imageView->Handle);
				hash_combine(resourceHash, (size_t)sampler->Handle);
			}
			break;
			case GfxPipelineState::eUniformBuffer:
			{
				auto uniformBuffer = static_cast<VulkanBufferPtr>(resources[j].UniformBuffer);
				assert(uniformBuffer);
				m_ResourceMap[resources[j].Binding].Buffer = uniformBuffer->Handle;
				hash_combine(resourceHash, (size_t)uniformBuffer->Handle);
			}
			break;
			}
		}
	}

	uint32_t index = std::numeric_limits<uint32_t>::max();
	for (uint32_t i = 0u; i < m_DescriptorSets.size(); ++i)
	{
		if (m_DescriptorSets[i].hash() == resourceHash)
		{
			index = i;
			break;
		}
	}
	if (index != std::numeric_limits<uint32_t>::max())
	{
		m_CurDescriptorSet = m_DescriptorSets[index];
		return;
	}

	auto descriptorSet = VulkanMainDescriptorPool::instance()->alloc(m_DescriptorSetLayout.Handle, m_ResourceMap);
	descriptorSet.setHash(resourceHash);
	m_CurDescriptorSet = descriptorSet;
	m_DescriptorSets.emplace_back(std::move(descriptorSet));
#endif
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
		1.0f
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

void VulkanGraphicsPipeline::initShaderResourceMap(const GfxDescriptorLayoutDesc& desc)
{
	/// For verify
	for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
	{
#if defined(UsingUnorderedMap)
		for (auto &res : desc[i])
		{
			assert(m_ResourceMap.find(res.Binding) == m_ResourceMap.end());

			VulkanDescriptorSet::VulkanResourceInfo resInfo;
			resInfo.Type = (VkDescriptorType)res.Type;
			m_ResourceMap.insert(std::make_pair(res.Binding, resInfo));
		}
#else
		for (uint32_t j = 0u; j < desc[i].size(); ++j)
		{
			auto index = findByBinding(m_ResourceMap, desc[i][j].Binding);
			assert(index == std::numeric_limits<uint32_t>::max());

			VulkanDescriptorSet::VulkanResourceInfo resInfo;
			resInfo.Type = (VkDescriptorType)desc[i][j].Type;
			resInfo.Binding = desc[i][j].Binding;
			m_ResourceMap.emplace_back(std::move(resInfo));
		}
#endif
	}
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

VulkanGraphicsPipelinePtr VulkanPipelinePool::getOrCreateGfxPipeline(VkRenderPass renderPass, const GfxPipelineState* state)
{
	for each (auto &it in m_GfxPipelines)
	{
		if (*it.first == *state)
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
