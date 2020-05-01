#pragma once

#include "RGpuResource.h"

class rRenderPass : public rGpuResource
{
public:
	virtual void begin(const struct rGraphicsPipelineState &) = 0;
	virtual void execute(const struct rGraphicsPipelineState &) = 0;
	virtual void end() = 0;

	virtual void drawIndexed(uint32_t, uint32_t, uint32_t) = 0;
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
	eRFrontFace FrontFace = eCounterclockwise;
	bool8_t EnableDepthBias = false;
	float32_t DepthBias = 0.0f;
	float32_t DepthBiasClamp = 0.0f;
	float32_t DepthBiasSlope = 0.0f;
};

struct rBlendStateDesc
{
	bool8_t EnableLogicOp = false;
	eRLogicOp LogicOp = eRLogicOp::eNo;
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

struct rSamplerDesc
{
	eRFilter MinMagFilter = eLinear;
	eRSamplerAddressMode AddressModeU = eRepeat;
	eRSamplerAddressMode AddressModeV = eRepeat;
	eRSamplerAddressMode AddressModeW = eRepeat;
	float32_t MipLodBias = 0.0f;
	uint32_t MaxAnisotropy = 0u;
	eRCompareOp CompareOp = eRCompareOp::eNever;
	float32_t MinLod = 0.0f;
	float32_t MaxLod = 0.0f;
	eRBorderColor BorderColor = eFloatTransparentBlack;
};

class rSampler : public rGpuResource
{
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
		///assert(Shaders[shader->getUsage()] == nullptr);
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

	inline void bindVertexBuffer(rBuffer *buffer)
	{
		VertexBuffer = buffer;
	}

	inline void bindIndexBuffer(rBuffer *buffer)
	{
		IndexBuffer = buffer;
	}

	inline void setRenderArea(const vec4 &area)
	{
		RenderArea = area;
	}

	eRPrimitiveTopology PrimitiveTopology = eTriangleList;

	const rShader *Shaders[eRShaderUsage_MaxEnum]{};

	rRasterizerStateDesc RasterizerStateDesc{};
	rBlendStateDesc BlendStateDesc{};
	rDepthStencilStateDesc DepthStencilStateDesc{};
	rViewport Viewport;
	rScissor Scissor;
	rBuffer *VertexBuffer = nullptr;
	rBuffer *IndexBuffer = nullptr;

	friend bool8_t operator==(const rGraphicsPipelineState &left, const rGraphicsPipelineState &right)
	{
		for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
		{
			if (left.Shaders[i] != right.Shaders[i])
			{
				return false;
			}
		}
		return gear::isEqual(left.RasterizerStateDesc, right.RasterizerStateDesc) &&
			gear::isEqual(left.BlendStateDesc, right.BlendStateDesc) &&
			gear::isEqual(left.DepthStencilStateDesc, right.DepthStencilStateDesc) &&
			left.PrimitiveTopology == right.PrimitiveTopology;
	}
	/// MultisampleState

	struct rDrawOperation
	{
		enum eDrawOp
		{
			eDraw,
			eDrawIndexed,
			eDrawOp_MaxEnum
		};

		eDrawOp DrawOp = eDrawIndexed;
		size_t IndexCount = 0u;
		size_t FirstIndex = 0u;
		size_t VertexOffset = 0u;
	};
	rDrawOperation DrawOperation{};

	inline void drawIndexed(size_t indexCount, size_t firstIndex, size_t vertexOffset)
	{
		DrawOperation = rDrawOperation
		{
			rDrawOperation::eDrawIndexed,
			indexCount,
			firstIndex,
			vertexOffset
		};
	}

	struct rClearValue
	{
		vec4 Color = color::g_DarkBlue;
		float32_t Depth = 1.0f;
		uint32_t Stencil = 0u;
	};
	rClearValue ClearValue{};
	inline void setClearValue(const vec4 &color = color::g_DarkBlue, float32_t depth = 1.0f, uint32_t stencil = 0u)
	{
		ClearValue.Color = color;
		ClearValue.Depth = depth;
		ClearValue.Stencil = stencil;
	}
	vec4 RenderArea;
};

class rComputePipeline
{
};

struct rFrameBufferDesc
{
	rRenderSurface *ColorSurface[eMaxRenderTargets]{};
	rRenderSurface *DepthSurface = nullptr;

	uint32_t Width = 0u;
	uint32_t Height = 0u;
	uint32_t Layers = 1u;
};