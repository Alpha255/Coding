#pragma once

#if 0
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
#endif

#include "Gear/Gear.h"
#include <d3d11.h>

enum ELimitations : uint32_t
{
	MaxRenderTargets = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT
};

enum class EPolygonMode : uint8_t
{
	Solid,
	Wireframe,
	Point
};

enum class ECullMode : uint8_t
{
	None,
	FrontFace,
	BackFace
};

enum class EFrontFace : uint8_t
{
	Counterclockwise,
	Clockwise
};

enum class EBlendFactor : uint8_t
{
	Zero,
	One,
	Constant,
	InverseConstant,

	SrcAlpha,
	InverseSrcAlpha,
	DstAlpha,
	InverseDstAlpha,
	SrcAlphaSaturate,
	Src1Alpha,
	InverseSrc1Alpha,

	SrcColor,
	InverseSrcColor,
	DstColor,
	InverseDstColor,
	Src1Color,
	InverseSrc1Color
};

enum class EBlendOp : uint8_t
{
	Add,
	Subtract,
	ReverseSubtract,
	Min,
	Max
};

enum EColorWriteMask : uint8_t
{
	None = 1 << 0,
	Red = 1 << 1,
	Green = 1 << 2,
	Blue = 1 << 3,
	Alpha = 1 << 4,
	All = Red | Green | Blue | Alpha
};

enum class ELogicOp : uint8_t
{
	Clear,
	And,
	And_Reverse,
	Copy,
	And_Inverted,
	No,
	Xor,
	Or,
	Nor,
	Equivalent,
	Invert,
	Or_Reverse,
	Copy_Inverted,
	Or_Inverted,
	Nand,
	Set
};

enum class ECompareFunc : uint8_t
{
	Always,
	Never,
	Less,
	LessOrEqual,
	Equal, 
	NotEqual,
	Greater,
	GreaterOrEqual
};

enum class EStencilOp : uint8_t
{
	Keep,
	Zero,
	Replace,                 /// Set the stencil data to the reference value
	IncrementAndClamp,       /// Increment the stencil value by 1, and clamp the result
	DecrementAndClamp,       /// Decrement the stencil value by 1, and clamp the result
	Invert,
	IncrementAndWrap,        /// Increment the stencil value by 1, and wrap the result if necessary
	DecrementAndWrap         /// Increment the stencil value by 1, and wrap the result if necessary
};

enum class ETextureFilter : uint8_t
{
	Nearest,
	Linear,
	Anisotropic
};

enum class ESamplerAddressMode : uint8_t
{
	Repeat,
	MirroredRepeat,
	ClampToEdge,
	ClampToBorder,
	MirrorClampToEdge
};

enum class EBorderColor : uint8_t
{
	FloatTransparentBlack,
	IntTransparentBlack,
	FloatOpaqueBlack,
	IntOpaqueBlack,
	FloatOpaqueWhite,
	IntOpaqueWhite
};

enum class EPrimitiveTopology : uint8_t
{
	PointList,
	LineList,
	LineStrip,
	TriangleList,
	TriangleStrip,
	LineListAdj,
	LineStripAdj,
	TriangleListAdj,
	TriangleStripAdj,
	PatchList
};

struct EXPORT_API RasterizerStateDesc
{
	EPolygonMode PolygonMode = EPolygonMode::Solid;
	ECullMode CullMode = ECullMode::None;
	EFrontFace FrontFace = EFrontFace::Counterclockwise;
	bool8_t EnableDepthBias = false;

	float32_t DepthBias = 0.0f;
	float32_t DepthBiasClamp = 0.0f;
	float32_t DepthBiasSlope = 0.0f;
};

struct EXPORT_API ColorBlendDesc
{
	bool8_t Enable = false;
	uint8_t ColorMask = EColorWriteMask::None;
	EBlendFactor SrcColor = EBlendFactor::SrcColor;
	EBlendFactor DstColor = EBlendFactor::Zero;

	EBlendOp ColorOp = EBlendOp::Add;
	EBlendFactor SrcAlpha = EBlendFactor::SrcAlpha;
	EBlendFactor DstAlpha = EBlendFactor::Zero;
	EBlendOp AlphaOp = EBlendOp::Add;
};

struct EXPORT_API BlendStateDesc
{
	bool8_t EnableLogicOp = false;
	ELogicOp LogicOp = ELogicOp::No;

	ColorBlendDesc ColorBlends[ELimitations::MaxRenderTargets]{};
};

struct EXPORT_API StencilStateDesc
{
	EStencilOp FailOp = EStencilOp::Keep;
	EStencilOp PassOp = EStencilOp::Keep;
	EStencilOp DepthFailOp = EStencilOp::Keep;
	ECompareFunc CompareFunc = ECompareFunc::Always;

	uint32_t Reference = 0u;
};

struct EXPORT_API DepthStencilStateDesc
{
	bool8_t EnableDepth = true;
	bool8_t EnableDepthWrite = false;
	bool8_t EnableStencil = false;
	ECompareFunc DepthCompareFunc = ECompareFunc::Always;

	uint8_t StencilReadMask = 0xF;
	uint8_t StencilWriteMask = 0xF;
	StencilStateDesc FrontFaceStencilState{};
	StencilStateDesc BackFaceStencilState{};
};

struct EXPORT_API SamplerStateDesc
{
	ETextureFilter MinMagFilter = ETextureFilter::Linear;
	ESamplerAddressMode AddressModeU = ESamplerAddressMode::Repeat;
	ESamplerAddressMode AddressModeV = ESamplerAddressMode::Repeat;
	ESamplerAddressMode AddressModeW = ESamplerAddressMode::Repeat;

	ECompareFunc CompareOp = ECompareFunc::Never;
	EBorderColor BorderColor = EBorderColor::FloatTransparentBlack;

	uint32_t MaxAnisotropy = 0u;
	float32_t MipLodBias = 0.0f;
	float32_t MinLod = 0.0f;
	float32_t MaxLod = 0.0f;
};

class EXPORT_API GraphicsPipelineState
{
public:
protected:
private:
	EPrimitiveTopology m_PrimitiveTopology = EPrimitiveTopology::TriangleList;

	RasterizerStateDesc m_RasterizerStateDesc{};
	BlendStateDesc m_BlendStateDesc{};
	DepthStencilStateDesc m_DepthStencilStateDesc{};

	bool8_t m_Compiled = false;
};

class EXPORT_API ComputePipelineState
{

};

class EXPORT_API RayTracingPipelineState
{

};