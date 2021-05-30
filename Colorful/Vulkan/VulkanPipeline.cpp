#include "Colorful/Vulkan/VulkanPipeline.h"
#include "Colorful/Vulkan/VulkanEnumTranslator.h"
#include "Colorful/Vulkan/VulkanDevice.h"
#include "Colorful/Vulkan/VulkanSwapchain.h"

NAMESPACE_START(Gfx)

VulkanPipelineLayout::VulkanPipelineLayout(const PipelineResourceTable& desc)
{
	/// fixed size?
	std::vector<VkDescriptorSetLayoutBinding> bindings;
	for (uint32_t i = 0u; i < EShaderStage::ShaderStageCount; ++i)
	{
		for (uint32_t j = 0u; j < desc[i].size(); ++j)
		{
			const auto& resourceDesc = desc[i][j];
			bindings.emplace_back(
				VkDescriptorSetLayoutBinding
				{
					resourceDesc.first.Binding,
					VkEnumTranslator::descriptorType(resourceDesc.first.Type),
					1u,
					static_cast<VkShaderStageFlags>(VkEnumTranslator::shaderStage(static_cast<EShaderStage>(i))),
					nullptr
				});
		}
	}

	VkDescriptorSetLayoutCreateInfo descriptorLayoutCreateInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		nullptr,
		0u,
		static_cast<uint32_t>(bindings.size()),
		bindings.data()
	};
	VERIFY_VK(vkCreateDescriptorSetLayout(s_Device->get(), &descriptorLayoutCreateInfo, VK_MEMORY_ALLOCATOR, &m_DescriptorSetLayout));

	VkPipelineLayoutCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0u,
		1u,
		&m_DescriptorSetLayout,
		0u,
		nullptr /// PushRange ?????
	};
	VERIFY_VK(vkCreatePipelineLayout(s_Device->get(), &createInfo, VK_MEMORY_ALLOCATOR, reference()));
}

VulkanPipelineLayout::~VulkanPipelineLayout()
{
	/// The pipeline layout represents a sequence of descriptor sets with each having a specific layout. 
	/// This sequence of layouts is used to determine the interface between shader stages and shader resources. 
	/// Each pipeline is created using a pipeline layout.
	vkDestroyDescriptorSetLayout(s_Device->get(), m_DescriptorSetLayout, VK_MEMORY_ALLOCATOR);
	vkDestroyPipelineLayout(s_Device->get(), get(), VK_MEMORY_ALLOCATOR);
	m_DescriptorSetLayout = VK_NULL_HANDLE;
}

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VkRenderPass renderPass, GraphicsPipelineState& state)
{
	assert(renderPass);

	m_PipelineLayout = create_shared<VulkanPipelineLayout>(state.resourceTable());

	std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
	auto& shaderStages = state.material()->pipelineShaderStages();
	assert(shaderStages[EShaderStage::Vertex]);
	for (uint32_t i = 0u; i < EShaderStage::ShaderStageCount; ++i)
	{
		if (shaderStages[i])
		{
			auto shader = std::static_pointer_cast<VulkanShader>(shaderStages[i]);
			shaderStageCreateInfos.emplace_back(std::move(
				VkPipelineShaderStageCreateInfo
				{
					VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					nullptr,
					0u,
					VkEnumTranslator::shaderStage(shader->stage()),
					shader->get(),
					"main",
					nullptr
				}));
		}
	}

	auto& rasterizationStateDesc = state.rasterizationStateDesc();
	VkPipelineRasterizationStateCreateInfo rasterizationState
	{
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		nullptr,
		0u, /// flags is reserved for future use
		rasterizationStateDesc.EnableDepthClamp,
		false,
		VkEnumTranslator::polygonMode(rasterizationStateDesc.PolygonMode),
		VkEnumTranslator::cullMode(rasterizationStateDesc.CullMode),
		VkEnumTranslator::frontFace(rasterizationStateDesc.FrontFace),
		rasterizationStateDesc.EnableDepthBias,
		rasterizationStateDesc.DepthBias,
		rasterizationStateDesc.DepthBiasClamp,
		rasterizationStateDesc.DepthBiasSlope,
		rasterizationStateDesc.LineWidth
	};

	auto& depthStencilStateDesc = state.depthStencilStateDesc();
	VkPipelineDepthStencilStateCreateInfo depthStencilState
	{
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		nullptr,
		0u,  /// flags is reserved for future use.
		depthStencilStateDesc.EnableDepth,
		depthStencilStateDesc.EnableDepthWrite,
		VkEnumTranslator::compareFunc(depthStencilStateDesc.DepthCompareFunc),
		depthStencilStateDesc.EnableDepthBoundsTest,
		depthStencilStateDesc.EnableStencil,
		{
			VkEnumTranslator::stencilOp(depthStencilStateDesc.FrontFaceStencilState.FailOp),
			VkEnumTranslator::stencilOp(depthStencilStateDesc.FrontFaceStencilState.PassOp),
			VkEnumTranslator::stencilOp(depthStencilStateDesc.FrontFaceStencilState.DepthFailOp),
			VkEnumTranslator::compareFunc(depthStencilStateDesc.FrontFaceStencilState.CompareFunc),
			depthStencilStateDesc.StencilReadMask,
			depthStencilStateDesc.StencilWriteMask,
			depthStencilStateDesc.FrontFaceStencilState.Reference
		},
		{
			VkEnumTranslator::stencilOp(depthStencilStateDesc.BackFaceStencilState.FailOp),
			VkEnumTranslator::stencilOp(depthStencilStateDesc.BackFaceStencilState.PassOp),
			VkEnumTranslator::stencilOp(depthStencilStateDesc.BackFaceStencilState.DepthFailOp),
			VkEnumTranslator::compareFunc(depthStencilStateDesc.BackFaceStencilState.CompareFunc),
			depthStencilStateDesc.StencilReadMask,
			depthStencilStateDesc.StencilWriteMask,
			depthStencilStateDesc.BackFaceStencilState.Reference
		},
		0.0f,
		1.0f
	};

	auto& blendStateDesc = state.blendStateDesc();
	std::vector<VkPipelineColorBlendAttachmentState> attachmentStates;
	for (uint32_t i = 0u; i < ELimitations::MaxRenderTargets; ++i)
	{
		if (blendStateDesc.ColorBlends[i].AttachmentIndex == i) /// Temp ???
		{
			attachmentStates.emplace_back(
				VkPipelineColorBlendAttachmentState
				{
					blendStateDesc.ColorBlends[i].Enable,
					VkEnumTranslator::blendFactor(blendStateDesc.ColorBlends[i].SrcColor),
					VkEnumTranslator::blendFactor(blendStateDesc.ColorBlends[i].DstColor),
					VkEnumTranslator::blendOp(blendStateDesc.ColorBlends[i].ColorOp),
					VkEnumTranslator::blendFactor(blendStateDesc.ColorBlends[i].SrcAlpha),
					VkEnumTranslator::blendFactor(blendStateDesc.ColorBlends[i].DstAlpha),
					VkEnumTranslator::blendOp(blendStateDesc.ColorBlends[i].AlphaOp),
					VkEnumTranslator::colorComponentFlags(blendStateDesc.ColorBlends[i].ColorMask)
				});
		}
	}
	VkPipelineColorBlendStateCreateInfo blendState
	{
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		nullptr,
		0u,  /// flags is reserved for future use.
		blendStateDesc.EnableLogicOp,
		VkEnumTranslator::logicOp(blendStateDesc.LogicOp),
		static_cast<uint32_t>(attachmentStates.size()),
		attachmentStates.data()
	};

	VkPipelineTessellationStateCreateInfo tessellationStateCreateInfo
	{
	};

	auto& multisampleStateDesc = state.multisampleStateDesc();
	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		nullptr,
		0u,
		static_cast<VkSampleCountFlagBits>(multisampleStateDesc.SampleCount),
		multisampleStateDesc.EnableSampleShading,
		multisampleStateDesc.MinSampleShading,
		multisampleStateDesc.SampleMask,
		multisampleStateDesc.EnableAlphaToCoverage,
		multisampleStateDesc.EnableAlphaToOne
	};

	/// Set viewport/scissor dynamic
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

	auto inputLayout = std::static_pointer_cast<VulkanInputLayout>(state.material()->inputLayout());
	assert(inputLayout);

	auto inputStateCreateInfo = inputLayout->inputStateCreateInfo();
	auto inputAssemblyStateCreateInfo = inputLayout->inputAssemblyStateCreateInfo();

	VkGraphicsPipelineCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		nullptr,
		0u,
		static_cast<uint32_t>(shaderStageCreateInfos.size()),
		shaderStageCreateInfos.data(),
		&inputStateCreateInfo,
		&inputAssemblyStateCreateInfo,
		&tessellationStateCreateInfo,
		nullptr,
		&rasterizationState,
		&multisampleStateCreateInfo,
		&depthStencilState,
		&blendState,
		&dynamicStateCreateInfo,
		m_PipelineLayout->get(),
		renderPass,
		0u,
		VK_NULL_HANDLE,
		0u
	};

	/// Pending creations ???
	VERIFY_VK(vkCreateGraphicsPipelines(s_Device->get(), VkRenderStateCache::instance().pipelineCache(), 1u, &createInfo, VK_MEMORY_ALLOCATOR, reference()));

	if (rasterizationState.polygonMode == VK_POLYGON_MODE_FILL)
	{
		rasterizationState.polygonMode = VK_POLYGON_MODE_LINE;
		VERIFY_VK(vkCreateGraphicsPipelines(s_Device->get(), VkRenderStateCache::instance().pipelineCache(), 1u, &createInfo, VK_MEMORY_ALLOCATOR, &m_WireframePipeline));
	}
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
	if (m_WireframePipeline)
	{
		vkDestroyPipeline(s_Device->get(), m_WireframePipeline, VK_MEMORY_ALLOCATOR);
		m_WireframePipeline = VK_NULL_HANDLE;
	}
}

VulkanPipeline::~VulkanPipeline()
{
	vkDestroyPipeline(s_Device->get(), get(), VK_MEMORY_ALLOCATOR);
}

VulkanPipelineCache::VulkanPipelineCache()
{
	VkPipelineCacheCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
		nullptr,
		0u,   /// flags is reserved for future use
		0u,
		nullptr
	};
	VERIFY_VK(vkCreatePipelineCache(s_Device->get(), &createInfo, VK_MEMORY_ALLOCATOR, reference()));
}

VulkanPipelineCache::~VulkanPipelineCache()
{
	vkDestroyPipelineCache(s_Device->get(), get(), VK_MEMORY_ALLOCATOR);
}

VkRenderStateCache& VkRenderStateCache::instance()
{
	static VkRenderStateCache s_Instance;
	return s_Instance;
}

void VkRenderStateCache::initialize(VulkanSwapchain* swapchain)
{
	assert(swapchain);
	m_Swapchain = swapchain;
}

VulkanRenderPassPtr VkRenderStateCache::getOrCreateRenderPass(const RenderPassDesc& desc)
{
	for (auto& pair : m_RenderPassCache)
	{
		if (pair.first == desc.hash())
		{
			return pair.second;
		}
	}

	if (desc.AttachmentDescs.size() == 0u)
	{
		RenderPassDesc defaultRenderPassDesc;
		defaultRenderPassDesc.AttachmentDescs.resize(2u);
		defaultRenderPassDesc.AttachmentDescs[0] = 
		{
			FormatAttribute::attribute_Vk(m_Swapchain->colorFormat()).Format,
			1u,
			RenderPassDesc::EAttachmentLoadOp::Clear,
			RenderPassDesc::EAttachmentStoreOp::DontCare,
			RenderPassDesc::EAttachmentLoadOp::Clear,
			RenderPassDesc::EAttachmentStoreOp::DontCare,
			Texture::EImageLayout::Undefined,
			Texture::EImageLayout::Present
		};

		defaultRenderPassDesc.AttachmentDescs[1] =
		{
			FormatAttribute::attribute_Vk(m_Swapchain->depthStencilFormat()).Format,
			1u,
			RenderPassDesc::EAttachmentLoadOp::Clear,
			RenderPassDesc::EAttachmentStoreOp::DontCare,
			RenderPassDesc::EAttachmentLoadOp::Clear,
			RenderPassDesc::EAttachmentStoreOp::DontCare,
			Texture::EImageLayout::Undefined,
			Texture::EImageLayout::DepthStencilAttachment
		};

		defaultRenderPassDesc.SubPasses.resize(1u);
		defaultRenderPassDesc.SubPasses[0] =
		{
			{},
			{ 
				{
					0u,
					Texture::EImageLayout::ColorAttachment
				} 
			},
			{},
			{},
			{
				1u,
				Texture::EImageLayout::DepthStencilAttachment
			}
		};

		defaultRenderPassDesc.SubPassDependencies.resize(2u);
		defaultRenderPassDesc.SubPassDependencies[0] =
		{
			VK_SUBPASS_EXTERNAL,
			0u,
			RenderPassDesc::EPipelineStageFlags::BottomOfPipe,
			RenderPassDesc::EPipelineStageFlags::ColorAttachmentOutput,
			RenderPassDesc::EAccessFlags::MemoryRead,
			RenderPassDesc::EAccessFlags::ColorAttachmentRead | RenderPassDesc::EAccessFlags::ColorAttachmentWrite,
		};

		defaultRenderPassDesc.SubPassDependencies[1] =
		{
			0u,
			VK_SUBPASS_EXTERNAL,
			RenderPassDesc::EPipelineStageFlags::ColorAttachmentOutput,
			RenderPassDesc::EPipelineStageFlags::BottomOfPipe,
			RenderPassDesc::EAccessFlags::ColorAttachmentRead | RenderPassDesc::EAccessFlags::ColorAttachmentWrite,
			RenderPassDesc::EAccessFlags::MemoryRead,
		};

		auto renderPass = create_shared<VulkanRenderPass>(defaultRenderPassDesc);
		m_RenderPassCache.push_back(std::make_pair(desc.hash(), renderPass));
		return renderPass;
	}

	auto renderPass = create_shared<VulkanRenderPass>(desc);
	m_RenderPassCache.push_back(std::make_pair(desc.hash(), renderPass));
	return renderPass;
}

VulkanGraphicsPipelinePtr VkRenderStateCache::getOrCreateGraphicPipeline(VkRenderPass renderPass, GraphicsPipelineState& state)
{

}

VulkanFramebufferPtr VkRenderStateCache::getOrCreateFrameBuffer(VkRenderPass renderPass, const FrameBufferDesc& desc)
{

}

void VkRenderStateCache::finalize()
{
	m_FrameBufferCache.clear();
	m_GfxPipelineCache.clear();
	m_RenderPassCache.clear();

	m_PipelineCache.reset();
}

NAMESPACE_END(Gfx)
