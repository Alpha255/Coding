#include "VulkanContext.h"
#include "VulkanEngine.h"

//void VulkanContext::Draw(uint32_t vertexCount, uint32_t startVertex, uint32_t primitive)
//{
//	///vkCmdDraw();
//}
//
//void VulkanContext::DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive)
//{
//	///vkCmdDrawIndexed();
//}

void VulkanContext::Create()
{
	if (IsValid())
	{
		return;
	}

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0U,
		0U,
		nullptr,
		0U,
		nullptr

	};
	VKCheck(vkCreatePipelineLayout(VulkanEngine::Instance().GetDevice(), &pipelineLayoutCreateInfo, 0, &m_Layout));

	VkPipelineVertexInputStateCreateInfo viStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
	viStateInfo.vertexBindingDescriptionCount = 1;
	viStateInfo.pVertexBindingDescriptions = nullptr;
	viStateInfo.vertexAttributeDescriptionCount = 0;
	viStateInfo.pVertexAttributeDescriptions = nullptr;

	VkPipelineInputAssemblyStateCreateInfo iaStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	iaStateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	iaStateInfo.primitiveRestartEnable = VK_FALSE;

	// set dynamically
	VkPipelineViewportStateCreateInfo vpStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
	vpStateInfo.pNext = 0;
	vpStateInfo.viewportCount = 1;
	vpStateInfo.scissorCount = 1;

	VkPipelineColorBlendAttachmentState attachments[1] = {};
	attachments[0].blendEnable = VK_FALSE;
	attachments[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo cbStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	cbStateInfo.logicOpEnable = VK_FALSE;
	cbStateInfo.attachmentCount = _countof(attachments);
	cbStateInfo.pAttachments = attachments;

	VkPipelineMultisampleStateCreateInfo msStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
	msStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	msStateInfo.alphaToCoverageEnable = VK_FALSE;
	msStateInfo.sampleShadingEnable = VK_FALSE;
	msStateInfo.minSampleShading = 1.0f;
	uint32_t smplMask = 0x1;
	msStateInfo.pSampleMask = &smplMask;

	VkPipelineTessellationStateCreateInfo tessStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };
	tessStateInfo.patchControlPoints = 0;

	VkPipelineDynamicStateCreateInfo dynStateInfo;
	VkDynamicState dynStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	memset(&dynStateInfo, 0, sizeof(dynStateInfo));
	dynStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynStateInfo.dynamicStateCount = 2;
	dynStateInfo.pDynamicStates = dynStates;

	/// Create mPipeline state VI-IA-VS-VP-RS-FS-CB
	VkGraphicsPipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
	pipelineInfo.pVertexInputState = &viStateInfo;
	pipelineInfo.pInputAssemblyState = &iaStateInfo;
	pipelineInfo.pViewportState = &vpStateInfo;
	pipelineInfo.pRasterizationState = &m_RasterizationState;
	pipelineInfo.pColorBlendState = &cbStateInfo;
	pipelineInfo.pMultisampleState = &msStateInfo;
	///pipelineInfo.pTessellationState = &tessStateInfo;
	pipelineInfo.pDynamicState = &dynStateInfo;
	pipelineInfo.pDepthStencilState = &m_DepthStencilState;

	//pipelineInfo.stageCount = shaderCount;
	//pipelineInfo.pStages = pShaderStage;

	///pipelineInfo.renderPass = m_RenderTargetView.Get();
	pipelineInfo.subpass = 0;

	pipelineInfo.layout = m_Layout;

	VKCheck(vkCreateGraphicsPipelines(VulkanEngine::Instance().GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Handle));
}

//void VulkanContext::SetVertexBuffer(const VulkanBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot)
//{
//}
//
//void VulkanContext::SetIndexBuffer(const VulkanBuffer &indexBuffer, uint32_t fmt, uint32_t offset)
//{
//
//}