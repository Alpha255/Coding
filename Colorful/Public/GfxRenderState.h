#pragma once

#include "Colorful/Public/GfxMaterial.h"

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

	friend bool8_t operator==(const GfxViewport& left, const GfxViewport& right)
	{
		return (Vec4)left == (Vec4)right;
	}

	friend bool8_t operator!=(const GfxViewport& left, const GfxViewport& right)
	{
		return !(left == right);
	}
};

struct GfxScissor : public Vec4
{
	GfxScissor() = default;
	GfxScissor(float32_t offsetX, float32_t offsetY, float32_t extentWidth, float32_t extentHeight)
		: Vec4(offsetX, offsetY, extentWidth, extentHeight)
	{
	}

	friend bool8_t operator==(const GfxScissor& left, const GfxScissor& right)
	{
		return (Vec4)left == (Vec4)right;
	}

	friend bool8_t operator!=(const GfxScissor& left, const GfxScissor& right)
	{
		return !(left == right);
	}
};

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
		uint32_t Reference = 0u;
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
	GfxRenderSurfacePtr ColorSurface[eMaxRenderTargets]{};
	GfxRenderSurfacePtr DepthSurface = nullptr;

	uint32_t Width = 0u;
	uint32_t Height = 0u;
	uint32_t Layers = 1u;

	size_t hash() const
	{
		size_t hash = 0u;
		for (uint32_t i = 0u; i < eMaxRenderTargets; ++i)
		{
			if (ColorSurface[i])
			{
				hash_combine(hash, (size_t)ColorSurface[i].get());
			}
		}

		if (DepthSurface)
		{
			hash_combine(hash, (size_t)DepthSurface.get());
		}
	}
};

class GfxFrameBuffer
{
};
using GfxFrameBufferPtr = std::shared_ptr<GfxFrameBuffer>;

struct GfxPipelineState
{
	struct GfxClearValue
	{
		Vec4 Color = Color::DarkBlue;
		float32_t Depth = 1.0f;
		uint32_t Stencil = 0u;
	};

	enum eGfxResourceType
	{
		eTexture,
		eSampler,
		eCombinedTextureSampler,
		eUniformBuffer,
		eResourceType_MaxEnum
	};

	struct GfxResourceInfo
	{
		GfxTexturePtr Texture = nullptr;
		GfxSamplerPtr Sampler = nullptr;
		GfxGpuBufferPtr UniformBuffer = nullptr;
		eGfxResourceType Type = eResourceType_MaxEnum;
#if !defined(UsingUnorderedMap)
		uint32_t Binding = std::numeric_limits<uint32_t>().max();
#endif
	};

	inline void setPrimitiveTopology(eRPrimitiveTopology primitiveTopology)
	{
		PrimitiveTopology = primitiveTopology;
	}

	inline void setViewport(const GfxViewport& viewport)
	{
		Viewport = viewport;
	}

	inline void setScissor(const GfxScissor& scissor)
	{
		Scissor = scissor;
	}

	inline void setRasterizerState(const GfxRasterizerStateDesc& desc)
	{
		RasterizerStateDesc = desc;
	}

	inline void setBlendState(const GfxBlendStateDesc& desc)
	{
		BlendStateDesc = desc;
	}

	inline void setDepthStencilState(const GfxDepthStencilStateDesc& desc)
	{
 		DepthStencilStateDesc = desc;
	}

	inline void setVertexBuffer(const GfxGpuBufferPtr& buffer)
	{
		VertexBuffer = buffer;
	}

	inline void setIndexBuffer(const GfxGpuBufferPtr& buffer, eRIndexType type = eRIndexType::eUInt32)
	{
		IndexBuffer = buffer;
		IndexType = type;
	}

	inline void setFrameBuffer(const GfxFrameBufferPtr& frameBuffer)
	{
		FrameBuffer = frameBuffer;
	}

	inline void clearFrameBuffer(const Vec4& color = Color::DarkBlue, float32_t depth = 1.0f, uint32_t stencil = 0u)
	{
		ClearValue.Color = color;
		ClearValue.Depth = depth;
		ClearValue.Stencil = stencil;
	}

	inline void setMaterial(const std::string& matName)
	{
		if (!Material.isValid())
		{
			Material.Name = matName;
			Material.deserialize();
		}
	}

	void setCombinedTextureSampler(eRShaderUsage shader, const GfxTexturePtr& texture, const GfxSamplerPtr& sampler, uint32_t slot);
	void setTexure(eRShaderUsage shader, const GfxTexturePtr& texture, uint32_t slot);
	void setUniformBuffer(eRShaderUsage shader, const GfxGpuBufferPtr& buffer, uint32_t slot);
	void setSampler(eRShaderUsage shader, const GfxSamplerPtr& sampler, uint32_t slot);

	eRPrimitiveTopology PrimitiveTopology = eTriangleList;
	GfxRasterizerStateDesc RasterizerStateDesc{};
	GfxBlendStateDesc BlendStateDesc{};
	GfxDepthStencilStateDesc DepthStencilStateDesc{};
	GfxViewport Viewport;
	GfxScissor Scissor;
	GfxClearValue ClearValue{};
	eRIndexType IndexType = eRIndexType::eUInt32;
	bool8_t Wireframe = false;

	GfxGpuBufferPtr VertexBuffer = nullptr; /// Support multi vertex stream???
	GfxGpuBufferPtr IndexBuffer = nullptr;
	GfxFrameBufferPtr FrameBuffer = nullptr;
#if defined(UsingUnorderedMap)
	std::array<std::unordered_map<uint32_t, GfxResourceInfo>, eRShaderUsage_MaxEnum> ResourceMap;
#else
	std::array<std::vector<GfxResourceInfo>, eRShaderUsage_MaxEnum> ResourceMap;
#endif
	GfxMaterial Material;
	uint32_t VertexStrideAlignment = alignof(Vec4);

	friend bool8_t operator==(const GfxPipelineState& left, const GfxPipelineState& right)
	{
		return left.Material.Name == right.Material.Name &&
			Gear::isEqual(left.RasterizerStateDesc, right.RasterizerStateDesc) &&
			Gear::isEqual(left.BlendStateDesc, right.BlendStateDesc) &&
			Gear::isEqual(left.DepthStencilStateDesc, right.DepthStencilStateDesc) &&
			left.PrimitiveTopology == right.PrimitiveTopology;
	}
	/// MultisampleState
};

class GfxRenderPass
{
};
using GfxRenderPassPtr = std::shared_ptr<GfxRenderPass>;

#if !defined(UsingUnorderedMap)
template<class T>
uint32_t findByBinding(const std::vector<T>& target, uint32_t binding)
{
	for (uint32_t i = 0u; i < target.size(); ++i)
	{
		if (target[i].Binding == binding)
		{
			return i;
		}
	}

	return std::numeric_limits<uint32_t>::max();
}
#endif

template<class T>
struct TKey
{
	virtual bool8_t operator==(const T& other) const = 0;

	inline size_t hash() const
	{
		return Hash;
	}
protected:
	size_t Hash = 0u;
};

struct GfxRenderPassKey
{
	uint8_t NumRenderTargets = 0u;
	uint8_t SampleCount = 0u;
	eRFormat DepthStencilFormat = eUnknownFormat;
	eRFormat RenderTargetFormat[eMaxRenderTargets]{};

	bool8_t operator==(const GfxRenderPassKey&) const
	{
		return false;
	}

	size_t hash() const
	{
		return Hash;
	}
private:
	size_t Hash = 0u;
};

struct GfxFrameBufferKey
{
	GfxRenderPassPtr RenderPass = nullptr;
	uint32_t NumRenderTargets = 0u;
	GfxRenderSurfacePtr DepthStencil = nullptr;
	GfxRenderSurfacePtr RenderTargets[eMaxRenderTargets]{};

	bool8_t operator==(const GfxFrameBufferKey&) const
	{
		return false;
	}

	size_t hash() const
	{
		return Hash;
	}
private:
	size_t Hash = 0u;
};