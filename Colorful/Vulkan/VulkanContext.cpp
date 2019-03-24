#include "VulkanContext.h"
#include "VulkanEngine.h"

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
	assert(!IsValid());

	m_CreateInfo = createInfo;

	Check(vkCreateGraphicsPipelines(VulkanEngine::Instance().GetDevice().Get(), cache.Get(), 1U, &m_CreateInfo, nullptr, &m_Handle));
}

void VulkanPipeline::Destory()
{
	assert(IsValid());

	vkDestroyPipeline(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	Reset();
}

void VulkanContext::BuildPipline()
{
	for (uint32_t i = 0U; i < m_Pipelines.size(); ++i)
	{
		auto &pipelineInfo = m_Pipelines[i].GetPipelineInfo();
		if (IsEqual(pipelineInfo, m_CurPipelineInfo))
		{
			m_CurPipline = m_Pipelines[i];
			m_CurPipelineInfo = {};
			return;
		}
	}

	VulkanPipeline pipeline;
	pipeline.Create(m_CurPipelineInfo, m_Cache);
	m_Pipelines.emplace_back(pipeline);

	m_CurPipline = pipeline;
	m_CurPipelineInfo = {};
}

void VulkanContext::Finalize()
{
	for (uint32_t i = 0U; i < m_Pipelines.size(); ++i)
	{
		m_Pipelines[i].Destory();
	}

	m_Pipelines.clear();
	m_Cache.Destory();
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