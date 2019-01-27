#include "VulkanContextState.h"
#include "VulkanEngine.h"

void VulkanContextState::Initialize()
{
	auto &deviceLimits = VulkanEngine::Instance().GetPhysicalDevice().GetDeviceProperties().limits;

	VertexBuffers.resize(deviceLimits.maxVertexInputBindings);
	Viewports.resize(deviceLimits.maxViewports);
	ScissorRects.resize(deviceLimits.maxViewports);

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
		RenderTargetViews[0],
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

void VulkanContextState::CommitState(VulkanContext &ctx)
{
	if (m_bDirty)
	{
		UpdateDescriptorSets();

		ctx.Recreate();
	}

	auto cmdBuffer = VulkanEngine::Instance().GetSwapchain().GetCurCommandBuffer().Get();

	///vkCmdSetViewport(m_Swapchain.GetCurCommandBuffer().Get(), 0U, 1U, &viewport);

	///VkDeviceSize vertex_offset[1] = { 0 };
	///vkCmdBindVertexBuffers(m_Swapchain.GetCurCommandBuffer().Get(), 0U, 1U, &vertexBuffer, vertex_offset);

	///vkCmdBindIndexBuffer(m_Swapchain.GetCurCommandBuffer().Get(), indexBuffer.Get(), 0U, VK_INDEX_TYPE_UINT16);

	///vkCmdSetScissor(m_Swapchain.GetCurCommandBuffer().Get(), 0U, 1U, &rect);

	///vkCmdDrawIndexed(m_Swapchain.GetCurCommandBuffer().Get(), indexCount, 1U, startIndex, offset, 0U);

	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, ctx.Get());
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, ctx.GetLayout(), 0U, (uint32_t)DescriptorSets.size(), DescriptorSets.data(), 0U, nullptr);

	m_bDirty = false;
}