#pragma once

#include "VulkanObject.h"

class VulkanRasterizerState
{
public:
	inline VulkanRasterizerState()
	{
		m_StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		m_StateInfo.pNext = nullptr;
		m_StateInfo.depthClampEnable = VK_FALSE;
		m_StateInfo.rasterizerDiscardEnable = VK_FALSE;
		m_StateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		m_StateInfo.cullMode = VK_CULL_MODE_NONE;
		m_StateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		m_StateInfo.lineWidth = 1.0f;
	}

	inline void Create(uint32_t fillMode, uint32_t cullMode, bool bFrontCCW, bool bDepthClip, bool bScissor)
	{
		m_StateInfo.polygonMode = (VkPolygonMode)fillMode;
		m_StateInfo.cullMode = (VkCullModeFlagBits)cullMode;
		m_StateInfo.frontFace = bFrontCCW ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
	}

	inline VkPipelineRasterizationStateCreateInfo Get() const
	{
		return m_StateInfo;
	}
protected:
private:
	VkPipelineRasterizationStateCreateInfo m_StateInfo = {};
};

class VulkanSamplerState : public VulkanObject<VkSampler>
{
public:
	void Create(uint32_t filter, uint32_t addressMode, float LODBias, uint32_t compFunc, const float *pBorderClr, float minLOD, float maxLOD);
	void Destory();
protected:
private:
};

class VulkanDepthStencilState
{
public:
	void Create(
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
		uint32_t stencilCompFuncBackFace);

	inline VkPipelineDepthStencilStateCreateInfo Get() const
	{
		return m_StateInfo;
	}
protected:
private:
	VkPipelineDepthStencilStateCreateInfo m_StateInfo = {};
};

class VulkanBlendState
{
public:
	void Create(
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
		uint8_t renderTargetWriteMask);
};

struct VulkanStaticState
{
	static void Initialize();

	static VulkanSamplerState LinearSampler;
	static VulkanDepthStencilState DisableDepthStencil;
	static VulkanRasterizerState SolidNoneCulling;
	static VulkanRasterizerState Solid;
	static VulkanBlendState NoneBlendState;
	static VulkanDepthStencilState NoneDepthStencilState;
};
