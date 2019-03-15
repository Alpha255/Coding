#include "VulkanContextState.h"
#include "VulkanEngine.h"

void VulkanContextState::Initialize()
{
	auto &deviceLimits = VulkanEngine::Instance().GetPhysicalDevice().GetDeviceProperties().limits;

	VertexBuffers.resize(deviceLimits.maxVertexInputBindings);
	Viewports.resize(deviceLimits.maxViewports);
	ScissorRects.resize(deviceLimits.maxViewports);
	ShaderResourceViews.resize(16U);

	for (uint32_t i = 0U; i < eRShaderTypeCount; ++i)
	{
		///SamplerBindInfos[i].Samplers.resize(deviceLimits.maxDescriptorSetSamplers);
		SamplerBindInfos[i].Samplers.resize(16U);
	}

	uint32_t sampleMask = 0x1;
	MultisampleState =
	{
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		nullptr,
		0U,
		VK_SAMPLE_COUNT_1_BIT,
		VK_FALSE,
		1.0f,
		&sampleMask,
		VK_FALSE,
		VK_FALSE
	};

	TessellationState = 
	{
		VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
		nullptr,
		0U,
		0U
	};

	VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	DynamicState = 
	{
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		nullptr,
		0U,
		_countof(dynamicStates),
		dynamicStates
	};

	DepthStencilState = 
	{
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO
	};

	VkPipelineColorBlendAttachmentState color_attachment[1] = {};
	color_attachment[0].blendEnable = VK_TRUE;
	color_attachment[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	color_attachment[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	color_attachment[0].colorBlendOp = VK_BLEND_OP_ADD;
	color_attachment[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	color_attachment[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_attachment[0].alphaBlendOp = VK_BLEND_OP_ADD;
	color_attachment[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	VkPipelineColorBlendStateCreateInfo;
	ColorBlendState = 
	{
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		nullptr,
		0U,
		VK_FALSE,
		VK_LOGIC_OP_CLEAR,
		1U,
		color_attachment,
		{ 0.0f, 0.0f, 0.0f, 0.0f }
	};

	DescriptorPool.Create();
}

void VulkanContextState::UpdateDescriptorSets()
{
	for (uint32_t i = 0U; i < eRShaderTypeCount; ++i)
	{
		std::vector<VkSampler> validSamplers;
		for each (auto sampler in SamplerBindInfos[i].Samplers)
		{
			if (sampler != VK_NULL_HANDLE)
			{
				validSamplers.emplace_back(sampler);
			}
		}

		if (validSamplers.size() > 0U)
		{
			SamplerBindInfos[i].DescriptorSetLayout.Create(validSamplers, i);

			SamplerBindInfos[i].DescriptorSet = DescriptorPool.AllocDescriptorSet(SamplerBindInfos[i].DescriptorSetLayout);
		}
	}

	VkDescriptorImageInfo descriptorImageInfo
	{
		SamplerBindInfos[ePixelShader].Samplers[0],
		ShaderResourceViews[0],
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	};
	VkWriteDescriptorSet writeDescriptorSet
	{
		VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		nullptr,
		SamplerBindInfos[ePixelShader].DescriptorSet.Get(),
		0U,
		0U,
		1U,
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		&descriptorImageInfo,
		nullptr,
		nullptr
	};
	vkUpdateDescriptorSets(VulkanEngine::Instance().GetDevice(), 1, &writeDescriptorSet, 0, nullptr);
}

void VulkanContextState::CommitState(VulkanContext &ctx, uint32_t indexCount, uint32_t startIndex, int32_t offset)
{
	if (m_bDirty)
	{
		UpdateDescriptorSets();

		ctx.Recreate();
	}

	auto cmdBuffer = VulkanEngine::Instance().GetSwapchain().GetCurCommandBuffer().Get();

	VulkanEngine::Instance().GetSwapchain().Begin();

	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, ctx.Get());
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, ctx.GetLayout(), 0U, 1U, &SamplerBindInfos[ePixelShader].DescriptorSet, 0U, nullptr);

	VkDeviceSize vertex_offset[1] = { 0 };
	vkCmdBindVertexBuffers(cmdBuffer, 0U, 1U, &VertexBuffers[0].Buffer, vertex_offset);

	vkCmdBindIndexBuffer(cmdBuffer, IndexBuffer.Buffer, 0U, (VkIndexType)IndexBuffer.Format);

	vkCmdSetViewport(cmdBuffer, 0U, 1U, &Viewports[0]);

	vkCmdSetScissor(cmdBuffer, 0U, 1U, &ScissorRects[0]);

	///vkCmdPushConstants(VulkanEngine::Instance().GetSwapchain().GetCurCommandBuffer().Get(), s_Context.GetLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0U, sizeof(Matrix), &wvp);
	///vkCmdPushConstants(VulkanEngine::Instance().GetSwapchain().GetCurCommandBuffer().Get(), s_Context.GetLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0U, sizeof(Matrix), &wvp);

	vkCmdDrawIndexed(cmdBuffer, indexCount, 1U, startIndex, offset, 0U);

	VulkanEngine::Instance().GetSwapchain().End();

	m_bDirty = false;
}