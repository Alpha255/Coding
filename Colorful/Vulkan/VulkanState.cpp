#include "VulkanState.h"
#include "VulkanEngine.h"

void VulkanSamplerState::Create(uint32_t filter, uint32_t addressMode, float LODBias, eRComparisonFunc compFunc, const float *pBorderClr, float minLOD, float maxLOD)
{
	assert(!IsValid());

	VkSamplerCreateInfo createInfo = 
	{ 
		VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		nullptr,
		0U,
		(VkFilter)filter,
		(VkFilter)filter,
		VK_SAMPLER_MIPMAP_MODE_LINEAR,
		(VkSamplerAddressMode)addressMode,
		(VkSamplerAddressMode)addressMode,
		(VkSamplerAddressMode)addressMode,
		LODBias,
		false,
		1.0f,
		true,
		(VkCompareOp)compFunc,
		minLOD,
		maxLOD,
		VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
		false
	};

	VKCheck(vkCreateSampler(VulkanEngine::Instance().GetDevice(), &createInfo, 0, &m_Handle));
}

void VulkanSamplerState::Destory()
{
	assert(IsValid());

	vkDestroySampler(VulkanEngine::Instance().GetDevice(), m_Handle, nullptr);

	Reset();
}

void VulkanDepthStencilState::Create(
	bool bDepthEnable,
	uint32_t depthWriteMask,
	uint32_t compFunc,
	bool bStencilEnable,
	uint8_t stencilReadMask,
	uint8_t stencilWriteMask,
	uint32_t stencilFailOpFrontFace,
	uint32_t stencilDepthFailOpFrontFace,
	uint32_t stencilPassOpFrontFace,
	uint32_t stencilCompFuncFrontFace,
	uint32_t stencilFailOpBackFace,
	uint32_t stencilDepthFailOpBackFace,
	uint32_t stencilPassOpBackFace,
	uint32_t stencilCompFuncBackFace)
{
	m_StateInfo = 
	{
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		nullptr,
		0U,
		bDepthEnable,
		depthWriteMask,
		(VkCompareOp)compFunc,
		false,
		bStencilEnable,
		{
			(VkStencilOp)stencilFailOpFrontFace,
			(VkStencilOp)stencilPassOpFrontFace,
			(VkStencilOp)stencilDepthFailOpFrontFace,
			(VkCompareOp)stencilCompFuncFrontFace,
			stencilReadMask,
			stencilWriteMask,
			0
		},
		{
			(VkStencilOp)stencilFailOpBackFace,
			(VkStencilOp)stencilPassOpBackFace,
			(VkStencilOp)stencilDepthFailOpBackFace,
			(VkCompareOp)stencilCompFuncBackFace,
			stencilReadMask,
			stencilWriteMask,
			0
		},
		0.0f,
		1.0f
	};
}

void VulkanBlendState::Create(
	bool bAlphaToCoverage,
	bool bIndependentBlend,
	uint32_t surfaceIndex,
	bool bBlend,
	eRBlend srcColor,
	eRBlend dstColor,
	eRBlendOp colorOp,
	eRBlend srcAlpha,
	eRBlend dstAlpha,
	eRBlendOp alphaOp,
	uint8_t renderTargetWriteMask)
{

}

void VulkanDescriptorSetLayout::Create(std::vector<VkSampler> &samplers, uint32_t targetShader)
{
	assert(!IsValid());

	std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings;

	VkShaderStageFlags shaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	switch (targetShader)
	{
	case eVertexShader:
		shaderStage = VK_SHADER_STAGE_VERTEX_BIT;
		break;
	case ePixelShader:
		shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
		break;
	default:
		assert(0);
		break;
	}

	for (uint32_t i = 0U; i < samplers.size(); ++i)
	{
		VkDescriptorSetLayoutBinding setLayoutBinding
		{
			i,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			1U,
			shaderStage,
			&samplers[i]
		};

		setLayoutBindings.emplace_back(setLayoutBinding);
	}

	VkDescriptorSetLayoutCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		nullptr,
		0U,
		(uint32_t)setLayoutBindings.size(),
		setLayoutBindings.data()
	};

	VKCheck(vkCreateDescriptorSetLayout(VulkanEngine::Instance().GetDevice(), &createInfo, nullptr, &m_Handle));
}

void VulkanDescriptorSetLayout::Destory()
{
	assert(IsValid());

	vkDestroyDescriptorSetLayout(VulkanEngine::Instance().GetDevice(), m_Handle, nullptr);
}

VulkanSamplerState VulkanStaticState::LinearSampler;
VulkanDepthStencilState VulkanStaticState::DisableDepthStencil;
VulkanRasterizerState VulkanStaticState::SolidNoneCulling;
VulkanRasterizerState VulkanStaticState::Solid;
VulkanBlendState VulkanStaticState::NoneBlendState;
VulkanDepthStencilState VulkanStaticState::NoneDepthStencilState;

void VulkanStaticState::Initialize()
{
	LinearSampler.Create(eLinear, eWrap, 0.0f, eRComparisonFunc::eNever, nullptr, 0.0f, FLT_MAX);
}