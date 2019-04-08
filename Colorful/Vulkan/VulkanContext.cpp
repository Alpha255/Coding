#include "VulkanContext.h"
#include "VulkanEngine.h"

void VulkanPipelineLayout::Create(const VulkanDescriptorSetLayout &descriptorSetLayout)
{
	assert(!IsValid());

	VkPipelineLayoutCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0U,
		1U,
		&descriptorSetLayout,
		0U,
		nullptr
	};

	Check(vkCreatePipelineLayout(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));
}

void VulkanPipelineLayout::Destory()
{
	assert(IsValid());

	vkDestroyPipelineLayout(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	Reset();
}

void VulkanPipelineCache::Create()
{
	assert(!IsValid());

	VkPipelineCacheCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
		nullptr,
		0U,
		0U,
		nullptr
	};

	Check(vkCreatePipelineCache(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));
}

void VulkanPipelineCache::Destory()
{
	assert(IsValid());

	vkDestroyPipelineCache(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	Reset();
}

void VulkanPipeline::Create(const VkGraphicsPipelineCreateInfo &createInfo, const VulkanPipelineCache &cache)
{
	assert(!IsValid() && createInfo.layout != VK_NULL_HANDLE);

	m_CreateInfo = createInfo;

	Check(vkCreateGraphicsPipelines(VulkanEngine::Instance().GetDevice().Get(), cache.Get(), 1U, &m_CreateInfo, nullptr, &m_Handle));
}

void VulkanPipeline::Destory()
{
	assert(IsValid());
	
	m_Layout.Destory();

	vkDestroyPipeline(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	Reset();
}

bool VulkanContext::IsSamePipelineState(const VkGraphicsPipelineCreateInfo &left, const VkGraphicsPipelineCreateInfo &right)
{
	if (!Base::IsEqual(left.stageCount, right.stageCount))
	{
		return false;
	}

	if (!Base::IsEqual(*left.pStages, *right.pStages))
	{
		return false;
	}

	if (!Base::IsEqual(*left.pVertexInputState, *right.pVertexInputState))
	{
		return false;
	}

	if (!Base::IsEqual(*left.pInputAssemblyState, *right.pInputAssemblyState))
	{
		return false;
	}

	if (!Base::IsEqual(*left.pTessellationState, *right.pTessellationState))
	{
		return false;
	}

	if (!Base::IsEqual(*left.pViewportState, *right.pViewportState))
	{
		return false;
	}

	if (!Base::IsEqual(*left.pRasterizationState, *right.pRasterizationState))
	{
		return false;
	}

	if (!Base::IsEqual(*left.pMultisampleState, *right.pMultisampleState))
	{
		return false;
	}

	if (!Base::IsEqual(*left.pDepthStencilState, *right.pDepthStencilState))
	{
		return false;
	}

	if (!Base::IsEqual(*left.pColorBlendState, *right.pColorBlendState))
	{
		return false;
	}

	if (!Base::IsEqual(*left.pDynamicState, *right.pDynamicState))
	{
		return false;
	}

	if (!Base::IsEqual(left.subpass, right.subpass))
	{
		return false;
	}

	return true;
}

void VulkanContext::SetImageView(const VulkanImageView &imageView, uint32_t slot, eRShaderType targetShader)
{
	if (m_ImageViews[targetShader][slot] == imageView)
	{
		return;
	}

	m_ImageViews[targetShader][slot] = imageView;
	
	VulkanDescriptorSetInfos descriptorSetInfo
	{
		VulkanDescriptorPool::eCombinedImage,
		m_DescriptorSetLayoutBindingIndex++,
		imageView,
		VulkanBuffer(),
		targetShader
	};

	m_DescriptorSetInfos.emplace_back(descriptorSetInfo);

	m_Dirty = true;
}

void VulkanContext::SetUniformBuffer(const VulkanBuffer &uniformBuffer, uint32_t slot, eRShaderType targetShader)
{
	if (m_UniformBuffers[targetShader][slot] == uniformBuffer)
	{
		return;
	}

	m_UniformBuffers[targetShader][slot] = uniformBuffer;

	VulkanDescriptorSetInfos descriptorSetInfo
	{
		VulkanDescriptorPool::eUniformBuffer,
		m_DescriptorSetLayoutBindingIndex++,
		VulkanImageView(),
		uniformBuffer,
		targetShader
	};

	m_DescriptorSetInfos.emplace_back(descriptorSetInfo);

	m_Dirty = true;
}

void VulkanContext::BuildPipline()
{
	for (uint32_t i = 0U; i < m_Pipelines.size(); ++i)
	{
		auto &pipelineInfo = m_Pipelines[i].GetPipelineInfo();
		if (IsSamePipelineState(pipelineInfo, m_CurPipelineInfo))
		{
			m_CurPipline = m_Pipelines[i];
			return;
		}
	}

#if 1
	m_CurPipelineInfo.stageCount = (uint32_t)m_StateInfos.ShaderStageInfos.size();
	m_CurPipelineInfo.pStages = m_StateInfos.ShaderStageInfos.data();
	m_CurPipelineInfo.renderPass = VulkanEngine::Instance().GetRenderPass().Get();
#endif

	VulkanPipeline pipeline;
	pipeline.Create(m_CurPipelineInfo, m_Cache);
	pipeline.SetLayout(m_PipelineLayouts[0]);
	m_Pipelines.emplace_back(pipeline);

	m_CurPipline = pipeline;

	m_StateInfos.ShaderStageInfos.clear();
	m_StateInfos.ShaderStageInfoArray.fill(std::make_pair(VkPipelineShaderStageCreateInfo(), false));
}

void VulkanContext::BuildDescriptorSet()
{
	assert(m_DescriptorSetLayoutBindingIndex == m_DescriptorSetInfos.size());

	std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
	for (uint32_t i = 0U; i < m_DescriptorSetLayoutBindingIndex; ++i)
	{
		VkDescriptorSetLayoutBinding descriptorSetLayoutBinding
		{
			m_DescriptorSetInfos[i].Slot,
			(VkDescriptorType)m_DescriptorSetInfos[i].Type,
			1U,
			GetShaderStage(m_DescriptorSetInfos[i].ShaderType),
			nullptr
		};
		descriptorSetLayoutBindings.emplace_back(descriptorSetLayoutBinding);
	}
	VulkanDescriptorSet descriptorSet = m_DescriptorPool.Alloc(descriptorSetLayoutBindings);
	m_DescriptorSets.emplace_back(descriptorSet);

	VulkanPipelineLayout pipelineLayout;
	pipelineLayout.Create(descriptorSet.GetLayout());
	m_PipelineLayouts.emplace_back(pipelineLayout);

	m_CurPipelineInfo.layout = pipelineLayout.Get();

	for (uint32_t i = 0U; i < m_DescriptorSetLayoutBindingIndex; ++i)
	{
		auto &descriptorSetInfo = m_DescriptorSetInfos[i];
		switch (descriptorSetInfo.Type)
		{
		case VulkanDescriptorPool::eCombinedImage:
			descriptorSet.SetupCombinedImage(descriptorSetInfo.Image, descriptorSetInfo.Slot);
			break;
		case VulkanDescriptorPool::eUniformBuffer:
			descriptorSet.SetupUniformBuffer(descriptorSetInfo.UniformBuffer, descriptorSetInfo.Slot);
			break;
		default:
			assert(0);
			break;
		}
	}

	m_DescriptorSetLayoutBindingIndex = 0U;
	m_DescriptorSetInfos.clear();
}

void VulkanContext::DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive)
{
	m_StateInfos.InputAssemblyInfo = VkPipelineInputAssemblyStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		nullptr,
		0U,
		(VkPrimitiveTopology)primitive,
		VK_FALSE
	};


	for each (auto &shaderStage in m_StateInfos.ShaderStageInfoArray)
	{
		if (shaderStage.second)
		{
			m_StateInfos.ShaderStageInfos.emplace_back(shaderStage.first);
		}
	}

	if (m_Dirty)
	{
		BuildDescriptorSet();

		BuildPipline();

		m_Dirty = false;
	}

	auto size = VulkanEngine::Instance().GetSwapchain().GetSize();

	VkRenderPassBeginInfo renderPassBeginInfo
	{
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		nullptr,
		VulkanEngine::Instance().GetRenderPass().Get(),
		VK_NULL_HANDLE,
		{
			{
				0, 0
			},
			{
				size.width,
				size.height
			}
		},
		_countof(m_ClearColor),
		m_ClearColor
	};

	VkCommandBufferBeginInfo commandBufferBeginInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		nullptr,
		0U,
		nullptr
	};

	auto backBuffers = VulkanEngine::Instance().GetSwapchain().GetBackBuffers();
	for (uint32_t i = 0U; i < backBuffers.size(); ++i)
	{
		VkCommandBuffer commandBuffer = backBuffers[i].CommandBuffer.Get(0U);
		VkFramebuffer frameBuffer = backBuffers[i].FrameBuffer.Get();

		renderPassBeginInfo.framebuffer = frameBuffer;

		Check(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_CurPipline.Get());

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayouts[0].Get(), 0U, 1U, &m_DescriptorSets[0], 0U, nullptr);

		vkCmdSetViewport(commandBuffer, 0U, 1U, &m_Viewports[0U]);
		//for (uint32_t j = 0U; j < eMaxViewports; ++j)
		//{
		//	if (m_Viewports[j].GetSlot() != UINT32_MAX)
		//	{
		//	}
		//}
		vkCmdSetScissor(commandBuffer, 0U, 1U, &m_Scissors[0U]);
		//for (uint32_t j = 0U; j < eMaxScissors; ++j)
		//{
		//	if (m_Scissors[j].GetSlot() != UINT32_MAX)
		//	{
		//	}
		//}

		VkDeviceSize offsets = 0U;
		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer.Buffer.Get(), m_IndexBuffer.Offset, m_IndexBuffer.Format);
		vkCmdBindVertexBuffers(commandBuffer, 0U, 1U, &m_VertexBuffers[0U].Buffer, &offsets);
		vkCmdDrawIndexed(commandBuffer, indexCount, 1U, startIndex, offset, 0U);
		//for (uint32_t j = 0U; j < eMaxVertexBuffers; ++j)
		//{
		//	if (m_VertexBuffers[j].Buffer.IsValid())
		//	{
		//	}
		//}

		vkCmdEndRenderPass(commandBuffer);

		Check(vkEndCommandBuffer(commandBuffer));
	}
}

void VulkanContext::Finalize()
{
	m_Cache.Destory();
	m_DescriptorPool.Destory();

	for (uint32_t i = 0U; i < m_Pipelines.size(); ++i)
	{
		m_Pipelines[i].Destory();
	}

	m_Pipelines.clear();
}

#if 0
void VulkanContext::Recreate()
{
	if (IsValid())
	{
		m_Cache.Destory();

		Destory();
	}

	m_Cache.Create();

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0U,
		1U,
		&m_State.SamplerBindInfos[ePixelShader].DescriptorSetLayout,
		0U,
		nullptr

	};
	VKCheck(vkCreatePipelineLayout(VulkanEngine::Instance().GetDevice(), &pipelineLayoutCreateInfo, 0, &m_Layout));

	m_State.ViewportState = 
	{
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		nullptr,
		0U,
		(uint32_t)m_State.Viewports.size(),
		m_State.Viewports.data(),
		(uint32_t)m_State.ScissorRects.size(),
		m_State.ScissorRects.data()
	};

	assert(m_State.VertexShader);
	VkPipelineShaderStageCreateInfo shaderStageCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		nullptr,
		0U,
		VK_SHADER_STAGE_VERTEX_BIT,
		m_State.VertexShader,
		"main",
		nullptr
	};
	m_State.ShaderStages.emplace_back(shaderStageCreateInfo);

	if (m_State.PixelShader)
	{
		VkPipelineShaderStageCreateInfo shaderStageCreateInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0U,
			VK_SHADER_STAGE_FRAGMENT_BIT,
			m_State.PixelShader,
			"main",
			nullptr
		};
		m_State.ShaderStages.emplace_back(shaderStageCreateInfo);
	}

	std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
	for each (auto &vertexBuffer in m_State.VertexBuffers)
	{
		if (vertexBuffer.Buffer != VK_NULL_HANDLE)
		{
			vertexInputBindingDescriptions.emplace_back(vertexBuffer.VertexInputBindingDescription);
			vertexInputAttributeDescriptions.assign(vertexBuffer.VertexInputAttributeDescription.begin(), vertexBuffer.VertexInputAttributeDescription.end());
		}
	}

	m_State.VertexInputState =
	{
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		nullptr,
		0U,
		(uint32_t)vertexInputBindingDescriptions.size(),
		vertexInputBindingDescriptions.data(),
		(uint32_t)vertexInputAttributeDescriptions.size(),
		vertexInputAttributeDescriptions.data()
	};

	/// Create mPipeline state VI-IA-VS-VP-RS-FS-CB
	VkGraphicsPipelineCreateInfo createInfo 
	{
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		nullptr,
		0U,
		m_State.ShaderStages.size(),
		m_State.ShaderStages.data(),
		&m_State.VertexInputState,
		&m_State.InputAssemblyState,
		&m_State.TessellationState,
		&m_State.ViewportState,
		&m_State.RasterizationState,
		&m_State.MultisampleState,
		&m_State.DepthStencilState,
		&m_State.ColorBlendState,
		&m_State.DynamicState,
		m_Layout,
		VulkanEngine::Instance().GetDefaultRenderPass().Get(),
		0U,
		VK_NULL_HANDLE,
		0
	};

	VKCheck(vkCreateGraphicsPipelines(VulkanEngine::Instance().GetDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &m_Handle));
}

void VulkanContext::DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive)
{
	if (primitive != m_State.InputAssemblyState.topology)
	{
		m_State.InputAssemblyState =
		{
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			nullptr,
			0U,
			(VkPrimitiveTopology)primitive,
			VK_FALSE
		};

		m_State.SetDirty(true);
	}

	m_State.CommitState(*this, indexCount, startIndex, offset);
}

//void VulkanContext::SetVertexBuffer(const VulkanBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot)
//{
//}
//
//void VulkanContext::SetIndexBuffer(const VulkanBuffer &indexBuffer, uint32_t fmt, uint32_t offset)
//{
//
//}

//void VulkanContext::Draw(uint32_t vertexCount, uint32_t startVertex, uint32_t primitive)
//{
//	///vkCmdDraw();
//}
//
//void VulkanContext::DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive)
//{
//	///vkCmdDrawIndexed();
//}
#endif