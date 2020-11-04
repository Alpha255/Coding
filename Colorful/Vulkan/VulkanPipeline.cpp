#include "Colorful/Vulkan/VulkanPipeline.h"

NAMESPACE_START(Gfx)

VulkanPipelineLayout::VulkanPipelineLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout)
{
	assert(device && descriptorSetLayout);

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
	VERIFY_VK(vkCreatePipelineLayout(device, &createInfo, VK_MEMORY_ALLOCATOR, reference()));
}

VulkanPipelineCache::VulkanPipelineCache(VkDevice device)
{
	assert(device);

	VkPipelineCacheCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
		nullptr,
		0u,   /// flags is reserved for future use
		0u,
		nullptr
	};
	VERIFY_VK(vkCreatePipelineCache(device, &createInfo, VK_MEMORY_ALLOCATOR, reference()));
}

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VkDevice device, VkPipelineCache pipelineCache, const GraphicsPipelineState& state)
{
	assert(device && pipelineCache);
#if 0
	GfxDescriptorLayoutDesc descriptorLayoutDesc{};
	std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
	for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
	{
		if (state->Material.Shaders[i])
		{
			auto shader = std::static_pointer_cast<VulkanShader>(state->Material.Shaders[i]);

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

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		nullptr,
		0u,
		VulkanEnum::toPrimitiveTopology(state->PrimitiveTopology),
		false
	};

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
			stateDesc.FrontFace.Reference
		},
		{
			VulkanEnum::toStencilOp(stateDesc.BackFace.FailOp),
			VulkanEnum::toStencilOp(stateDesc.BackFace.PassOp),
			VulkanEnum::toStencilOp(stateDesc.BackFace.DepthFailOp),
			VulkanEnum::toCompareOp(stateDesc.BackFace.CompareOp),
			stateDesc.StencilReadMask,
			stateDesc.StencilWriteMask,
			stateDesc.BackFace.Reference
		},
		0.0f,
		1.0f
	};

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

	VkPipelineTessellationStateCreateInfo tessellationStateCreateInfo
	{
	};

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
		static_cast<uint32_t>(dynamicStates.size()),
		dynamicStates.data()
	};

	VkGraphicsPipelineCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		nullptr,
		0u,
		(uint32_t)shaderStageCreateInfos.size(),
		shaderStageCreateInfos.data(),
		&inputLayout,
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
	VERIFY_VK(vkCreateGraphicsPipelines(device, pipelineCache, 1u, &createInfo, VK_MEMORY_ALLOCATOR, reference()));

	if (rasterizationState.polygonMode == VK_POLYGON_MODE_FILL)
	{
		rasterizationState.polygonMode = VK_POLYGON_MODE_LINE;
		GfxVerifyVk(vkCreateGraphicsPipelines(device, pipelineCache, 1u, &createInfo, vkMemoryAllocator, &m_WireframePipeline.Handle));
	}
#endif
}

NAMESPACE_END(Gfx)
