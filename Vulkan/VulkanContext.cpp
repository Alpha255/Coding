#include "VulkanContext.h"
#include "VulkanEngine.h"
#include "Util/Geometry.h"

void VulkanContext::Draw(uint32_t vertexCount, uint32_t startVertex, uint32_t primitive)
{
	///vkCmdDraw();
}

void VulkanContext::DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive)
{
	///vkCmdDrawIndexed();
}

void VulkanContext::Create(const VkPipelineShaderStageCreateInfo *pShaderStage, const size_t shaderCount)
{
	m_RenderTargetView.Create();

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
	pipelineLayoutCreateInfo.setLayoutCount = 0;
	pipelineLayoutCreateInfo.pSetLayouts = NULL;
	VkResult result = vkCreatePipelineLayout(VulkanEngine::Instance().GetDevice().Get(), &pipelineLayoutCreateInfo, 0, &m_Layout);
	assert(result == VK_SUCCESS);

	VkVertexInputBindingDescription vertexBindings[1] = {};
	vertexBindings[0].binding = 0;
	vertexBindings[0].stride = sizeof(Geometry::Vertex);
	vertexBindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription attributes[4] = {};
	attributes[0].location = 0;
	attributes[0].binding = 0;
	attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributes[0].offset = offsetof(Geometry::Vertex, Position);
	attributes[1].location = 1;
	attributes[1].binding = 0;
	attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributes[1].offset = offsetof(Geometry::Vertex, Normal);
	attributes[2].location = 1;
	attributes[2].binding = 0;
	attributes[2].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributes[2].offset = offsetof(Geometry::Vertex, Tangent);
	attributes[3].location = 1;
	attributes[3].binding = 0;
	attributes[3].format = VK_FORMAT_R32G32_SFLOAT;
	attributes[3].offset = offsetof(Geometry::Vertex, UV);

	VkPipelineVertexInputStateCreateInfo viStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
	viStateInfo.vertexBindingDescriptionCount = 1;
	viStateInfo.pVertexBindingDescriptions = vertexBindings;
	viStateInfo.vertexAttributeDescriptionCount = _countof(attributes);
	viStateInfo.pVertexAttributeDescriptions = attributes;

	VkPipelineInputAssemblyStateCreateInfo iaStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	iaStateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	iaStateInfo.primitiveRestartEnable = VK_FALSE;

	// set dynamically
	VkPipelineViewportStateCreateInfo vpStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
	vpStateInfo.pNext = 0;
	vpStateInfo.viewportCount = 1;
	vpStateInfo.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rsStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
	rsStateInfo.depthClampEnable = VK_FALSE;
	rsStateInfo.rasterizerDiscardEnable = VK_FALSE;
	rsStateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rsStateInfo.cullMode = VK_CULL_MODE_NONE;
	rsStateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rsStateInfo.lineWidth = 1.0f;

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

	VkPipelineDepthStencilStateCreateInfo noDepth;
	noDepth = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
	noDepth.depthTestEnable = VK_FALSE;
	noDepth.depthWriteEnable = VK_FALSE;
	noDepth.depthCompareOp = VK_COMPARE_OP_ALWAYS;
	noDepth.depthBoundsTestEnable = VK_FALSE;
	noDepth.stencilTestEnable = VK_FALSE;
	noDepth.minDepthBounds = 0.0f;
	noDepth.maxDepthBounds = 1.0f;

	// Create mPipeline state VI-IA-VS-VP-RS-FS-CB
	VkGraphicsPipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
	pipelineInfo.pVertexInputState = &viStateInfo;
	pipelineInfo.pInputAssemblyState = &iaStateInfo;
	pipelineInfo.pViewportState = &vpStateInfo;
	pipelineInfo.pRasterizationState = &rsStateInfo;
	pipelineInfo.pColorBlendState = &cbStateInfo;
	pipelineInfo.pMultisampleState = &msStateInfo;
	///pipelineInfo.pTessellationState = &tessStateInfo;
	pipelineInfo.pDynamicState = &dynStateInfo;
	pipelineInfo.pDepthStencilState = &noDepth;

	pipelineInfo.stageCount = shaderCount;
	pipelineInfo.pStages = pShaderStage;

	pipelineInfo.renderPass = m_RenderTargetView.Get();
	pipelineInfo.subpass = 0;

	pipelineInfo.layout = m_Layout;

	result = vkCreateGraphicsPipelines(VulkanEngine::Instance().GetDevice().Get(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Handle);
	assert(result == VK_SUCCESS);
}

void VulkanContext::SetVertexBuffer(const VulkanBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot)
{
}

void VulkanContext::SetIndexBuffer(const VulkanBuffer &indexBuffer, uint32_t fmt, uint32_t offset)
{

}