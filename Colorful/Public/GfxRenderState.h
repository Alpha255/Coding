#pragma once

#include "Colorful/Public/GfxResource.h"

class GfxComputePipeline
{
};

struct GfxViewport : public Vec4
{
	GfxViewport() = default;
	GfxViewport(float32_t x, float32_t y, float32_t width, float32_t height)
		: Vec4(x, y, width, height)
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

struct GfxScissor : public Vec4
{
	GfxScissor() = default;
	GfxScissor(float32_t offsetX, float32_t offsetY, float32_t extentWidth, float32_t extentHeight)
		: Vec4(offsetX, offsetY, extentWidth, extentHeight)
	{
	}
};

class GfxRenderPass
{
public:
	virtual void pendingGfxPipline(const struct GfxPipelineState &) = 0;
	virtual void bindGfxPipeline(const struct GfxPipelineState &) = 0;

	virtual void drawIndexed(const struct GfxPipelineState &, uint32_t indexCount, uint32_t firstIndex, int32_t vertexOffset) = 0;
protected:
private:
	std::string m_Description;
};
using GfxRenderPassPtr = std::shared_ptr<GfxRenderPass>;

struct GfxRasterizerStateDesc
{
	eRPolygonMode PolygonMode = eSolid;
	eRCullMode CullMode = eCullNone;
	eRFrontFace FrontFace = eCounterclockwise;
	bool8_t EnableDepthBias = false;
	float32_t DepthBias = 0.0f;
	float32_t DepthBiasClamp = 0.0f;
	float32_t DepthBiasSlope = 0.0f;
};

struct GfxBlendStateDesc
{
	struct ColorBlendState
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

	bool8_t EnableLogicOp = false;
	eRLogicOp LogicOp = eRLogicOp::eNo;
	ColorBlendState ColorBlendStates[eMaxRenderTargets]{};
};

struct GfxDepthStencilStateDesc
{
	struct StencilOp
	{
		eRStencilOp FailOp = eRStencilOp::eKeep;
		eRStencilOp PassOp = eRStencilOp::eKeep;
		eRStencilOp DepthFailOp = eRStencilOp::eKeep;
		eRCompareOp CompareOp = eRCompareOp::eAlways;
	};

	bool8_t EnableDepth = true;
	bool8_t EnableDepthWrite = false;
	eRCompareOp DepthCompareOp = eRCompareOp::eAlways;
	bool8_t EnableStencil = false;
	uint8_t StencilReadMask = 0xF;
	uint8_t StencilWriteMask = 0xF;
	StencilOp FrontFace{};
	StencilOp BackFace{};
};

struct GfxSamplerDesc
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

struct GfxFrameBufferDesc
{
	class GfxRenderSurface *ColorSurface[eMaxRenderTargets]{};
	class GfxRenderSurface *DepthSurface = nullptr;

	uint32_t Width = 0u;
	uint32_t Height = 0u;
	uint32_t Layers = 1u;
};

struct GfxPipelineState
{
	enum eDirtyFlags : uint8_t
	{
		eViewport = 1u,
		eScissor = 2u,
		eVertexBuffer = 4u,
		eIndexBuffer = 8u
	};
	uint8_t Dirty = 0u;
	inline bool8_t isDirty() const
	{
		return Dirty == 0u;
	}
	inline bool8_t isDirty(eDirtyFlags flags) const
	{
		return flags & Dirty;
	}
	inline void setDirty(eDirtyFlags flags)
	{
		Dirty |= flags;
	}
	inline void clearDirty()
	{
		Dirty = 0u;
	}

	inline void setPrimitiveTopology(eRPrimitiveTopology primitiveTopology)
	{
		PrimitiveTopology = primitiveTopology;
	}

	inline void setShader(const GfxShaderPtr shader)
	{
		assert(shader && shader->usage() < eRShaderUsage_MaxEnum);
		Shaders[shader->usage()] = shader;
	}

	inline void setViewport(const GfxViewport &viewport)
	{
		Viewport = viewport;
		setDirty(eViewport);
	}

	inline void setScissor(const GfxScissor &scissor)
	{
		Scissor = scissor;
		setDirty(eScissor);
	}

	inline void setRasterizerState(const GfxRasterizerStateDesc &desc)
	{
		RasterizerStateDesc = desc;
	}

	inline void setBlendState(const GfxBlendStateDesc &desc)
	{
		BlendStateDesc = desc;
	}

	inline void setDepthStencilState(const GfxDepthStencilStateDesc &desc)
	{
		DepthStencilStateDesc = desc;
	}

	inline void bindVertexBuffer(GfxGpuBuffer *buffer)
	{
		VertexBuffer = buffer;
		setDirty(eVertexBuffer);
	}

	inline void bindIndexBuffer(GfxGpuBuffer *buffer, eRIndexType type = eRIndexType::eUInt32)
	{
		IndexBuffer = buffer;
		setDirty(eIndexBuffer);
		IndexType = type;
	}

	inline void setRenderArea(const Vec4 &area)
	{
		RenderArea = area;
	}

	eRPrimitiveTopology PrimitiveTopology = eTriangleList;

	GfxShaderPtr Shaders[eRShaderUsage_MaxEnum]{};

	GfxRasterizerStateDesc RasterizerStateDesc{};
	GfxBlendStateDesc BlendStateDesc{};
	GfxDepthStencilStateDesc DepthStencilStateDesc{};
	GfxViewport Viewport;
	GfxScissor Scissor;
	GfxGpuBufferPtr VertexBuffer = nullptr;
	GfxGpuBufferPtr IndexBuffer = nullptr;

	friend bool8_t operator==(const GfxPipelineState &left, const GfxPipelineState &right)
	{
		for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
		{
			if (left.Shaders[i] != right.Shaders[i])
			{
				return false;
			}
		}
		return Gear::isEqual(left.RasterizerStateDesc, right.RasterizerStateDesc) &&
			Gear::isEqual(left.BlendStateDesc, right.BlendStateDesc) &&
			Gear::isEqual(left.DepthStencilStateDesc, right.DepthStencilStateDesc) &&
			left.PrimitiveTopology == right.PrimitiveTopology;
	}
	/// MultisampleState

	struct rClearValue
	{
		Vec4 Color = Color::DarkBlue;
		float32_t Depth = 1.0f;
		uint32_t Stencil = 0u;
	};
	rClearValue ClearValue{};
	inline void setClearValue(const Vec4 &color = Color::DarkBlue, float32_t depth = 1.0f, uint32_t stencil = 0u)
	{
		ClearValue.Color = color;
		ClearValue.Depth = depth;
		ClearValue.Stencil = stencil;
	}
	Vec4 RenderArea;
	eRIndexType IndexType = eRIndexType::eUInt32;
};