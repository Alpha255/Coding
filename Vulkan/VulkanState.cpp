#include "VulkanState.h"
#include "VulkanEngine.h"

void VulkanSamplerState::Create(uint32_t filter, uint32_t addressMode, float LODBias, uint32_t compFunc, const float *pBorderClr, float minLOD, float maxLOD)
{
	VkSamplerCreateInfo createInfo = 
	{ 
		VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		nullptr,
		0U,
		(VkFilter)filter,
		(VkFilter)filter,
		(VkSamplerMipmapMode)filter,
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

VulkanSamplerState VulkanStaticState::LinearSampler;
VulkanDepthStencilState VulkanStaticState::DisableDepthStencil;
VulkanRasterizerState VulkanStaticState::SolidNoneCulling;
VulkanRasterizerState VulkanStaticState::Solid;
VulkanBlendState VulkanStaticState::NoneBlendState;
VulkanDepthStencilState VulkanStaticState::NoneDepthStencilState;

void VulkanStaticState::Initialize()
{

}