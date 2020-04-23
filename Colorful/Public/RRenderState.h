#pragma once

#include "RGpuResource.h"

class rSampler
{
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

struct rStencilOpDesc
{
	eRStencilOp FailOp = eRStencilOp::eKeep;
	eRStencilOp PassOp = eRStencilOp::eKeep;
	eRStencilOp DepthFailOp = eRStencilOp::eKeep;
	eRCompareOp CompareOp = eRCompareOp::eAlways;
};

struct rColorBlendStateDesc
{
	bool8_t Enable = false;
	eRBlendFactor SrcColor = eRBlendFactor::eSrcColor;
	eRBlendFactor DstColor = eRBlendFactor::eZero;
	eRBlendOp ColorOp = eRBlendOp::eAdd;
	eRBlendFactor SrcAlpha = eRBlendFactor::eSrcAlpha;
	eRBlendFactor DstAlpha = eRBlendFactor::eZero;
	eRBlendOp AlphaOp = eRBlendOp::eAdd;
	uint32_t ColorMask = eRColorWriteMask::eColorNone;
};

struct rRasterizerStateDesc
{
	eRPolygonMode PolygonMode = eSolid;
	eRCullMode CullMode = eCullNone;
	eRFrontFace FrontFace = eClockwise;
	bool8_t EnableDepthBias = false;
	float32_t DepthBias = 0.0f;
	float32_t DepthBiasClamp = 0.0f;
	float32_t DepthBiasSlope = 0.0f;
};

struct rBlendStateDesc
{
	bool8_t EnableLogicOp = false;
	eRLogicOp logicOp = eRLogicOp::eNo;
	rColorBlendStateDesc ColorBlendStateDesc[eMaxRenderTargets]{};
};

struct rDepthStencilStateDesc
{
	bool8_t EnableDepth = true;
	bool8_t EnableDepthWrite = false;
	eRCompareOp DepthCompareOp = eRCompareOp::eAlways;
	bool8_t EnableStencil = false;
	uint8_t StencilReadMask = 0xF;
	uint8_t StencilWriteMask = 0xF;
	rStencilOpDesc FrontFace{};
	rStencilOpDesc BackFace{};
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
		const rColorBlendStateDesc * const clrBlendState) = 0;

	virtual void setDepthStencilState(
		bool8_t enableDepth,
		bool8_t enableDepthWrite,
		eRCompareOp depthCompareOp,
		bool8_t enableStencil,
		uint8_t stencilReadMask,
		uint8_t stencilWriteMask,
		const rStencilOpDesc &front,
		const rStencilOpDesc &back) = 0;

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

struct rViewport : public vec4
{
	rViewport() = default;
	rViewport(float32_t x, float32_t y, float32_t width, float32_t height)
		: vec4(x, y, width, height)
	{
	}
	inline constexpr float32_t minDepth() const
	{
		return 0.0f;
	}
	inline constexpr float32_t maxDepth() const
	{
		return 1.0f;
	}
};

struct rScissor : public vec4
{
	rScissor() = default;
	rScissor(float32_t extentWidth, float32_t extentHeight, float32_t offsetX, float32_t offsetY)
		: vec4(extentWidth, extentHeight, offsetX, offsetY)
	{
	}
};

struct rGraphicsPipelineState
{
	inline void setPrimitiveTopology(eRPrimitiveTopology primitiveTopology)
	{
		PrimitiveTopology = primitiveTopology;
	}

	inline void setShader(const rShader *shader)
	{
		assert(shader);
		assert(shader->getUsage() < eRShaderUsage_MaxEnum);
		assert(Shaders[shader->getUsage()] == nullptr);
		Shaders[shader->getUsage()] = shader;
	}

	inline void setViewport(const rViewport &viewport)
	{
		Viewport = viewport;
	}

	inline void setScissor(const rScissor &scissor)
	{
		Scissor = scissor;
	}

	inline void setRasterizerState(const rRasterizerStateDesc &desc)
	{
		RasterizerStateDesc = desc;
	}

	inline void setBlendState(const rBlendStateDesc &desc)
	{
		BlendStateDesc = desc;
	}

	inline void setDepthStencilState(const rDepthStencilStateDesc &desc)
	{
		DepthStencilStateDesc = desc;
	}

	eRPrimitiveTopology PrimitiveTopology = eTriangleList;

	const rShader *Shaders[eRShaderUsage_MaxEnum]{};

	rRasterizerStateDesc RasterizerStateDesc{};
	rBlendStateDesc BlendStateDesc{};
	rDepthStencilStateDesc DepthStencilStateDesc{};
	rViewport Viewport;
	rScissor Scissor;
	/// MultisampleState
};

class rComputePipeline
{
};
