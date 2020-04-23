#pragma once

#include "RObject.h"

class rSampler
{
};

struct rStencilOp
{
	eRStencilOp FailOp = eRStencilOp::eRStencilOp_MaxEnum;
	eRStencilOp PassOp = eRStencilOp::eRStencilOp_MaxEnum;
	eRStencilOp DepthFailOp = eRStencilOp::eRStencilOp_MaxEnum;
	eRCompareOp CompareOp = eRCompareOp::eRCompareOp_MaxEnum;
};

struct rColorBlendState
{
	bool8_t Enable = false;
	eRBlendFactor SrcColor = eRBlendFactor::eRBlendFactor_MaxEnum;
	eRBlendFactor DstColor = eRBlendFactor::eRBlendFactor_MaxEnum;
	eRBlendOp ColorOp = eRBlendOp::eRBlendOp_MaxEnum;
	eRBlendFactor SrcAlpha = eRBlendFactor::eRBlendFactor_MaxEnum;
	eRBlendFactor DstAlpha = eRBlendFactor::eRBlendFactor_MaxEnum;
	eRBlendOp AlphaOp = eRBlendOp::eRBlendOp_MaxEnum;
	uint32_t ColorMask = eRColorWriteMask::eColorNone;
};

class rRenderpass
{
public:
	virtual void begin() = 0;
	virtual void end() = 0;
	virtual void execute(const class rGraphicsPipeline *pipline) = 0;
protected:
private:
	std::string m_Description;
};

class rGraphicsPipeline
{
public:
	inline void setPrimitiveTopology(eRPrimitiveTopology primitiveTopology)
	{
		m_PrimitiveTopology = primitiveTopology;
	}

	inline void setClearValue(const vec4 &color, const float32_t depth, const uint8_t stencil)
	{
		m_ClearValue.Color = color;
		m_ClearValue.Depth = depth;
		m_ClearValue.Stencil = stencil;
	}

	inline void setShader(const rShader *shader)
	{
		assert(shader);
		assert(shader->getUsage() < eRShaderUsage_MaxEnum);
		assert(m_Shaders[shader->getUsage()] == nullptr);
		m_Shaders[shader->getUsage()] = shader;
	}

	virtual void setRasterizerState(
		eRPolygonMode polygonMode,
		eRCullMode cullMode,
		eRFrontFace frontFace,
		bool8_t enableDepthBias = false,
		float32_t depthBias = 0.0f,
		float32_t depthBiasClamp = 0.0f,
		float32_t depthBiasSlope = 0.0f) = 0;

	virtual void setBlendState(
		bool8_t enableLogicOp,
		eRLogicOp logicOp,
		uint32_t attachmentCount,
		const rColorBlendState * const clrBlendState) = 0;

	virtual void setDepthStencilState(
		bool8_t enableDepth,
		bool8_t enableDepthWrite,
		eRCompareOp depthCompareOp,
		bool8_t enableStencil,
		uint8_t stencilReadMask,
		uint8_t stencilWriteMask,
		const rStencilOp &front,
		const rStencilOp &back) = 0;

	virtual void setDescriptorLayout(const rDescriptorLayoutDesc &desc) = 0;

	virtual void setViewport(const vec4 &) = 0;

	virtual void setScissor(const vec4 &) = 0;

	virtual void build(const rRenderpass &renderpass) = 0;
protected:
	eRPrimitiveTopology m_PrimitiveTopology = eTriangleList;
	struct ClearValue
	{
		vec4 Color;
		float32_t Depth = 1.0f;
		uint8_t Stencil = 0u;
	} m_ClearValue;

	const rShader *m_Shaders[eRShaderUsage_MaxEnum]{};
private:
};

class rComputePipeline
{
};
