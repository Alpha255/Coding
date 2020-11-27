#pragma once

#include "Colorful/Public/GfxResource.h"
#include <d3d11.h>

NAMESPACE_START(Gfx)

enum ELimitations : uint32_t
{
	MaxRenderTargets = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
	MaxViewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE,
	MaxScissorRects = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE,
	MaxSamplers = D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT,
	MaxUniformBuffers = D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT,
	MaxTextures = D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT,
	MaxRWBufferss = D3D11_1_UAV_SLOT_COUNT,
	MaxVertexStreams = D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT
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

enum class EPrimitiveTopology
{
	Unknown,
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

struct RenderPassDesc
{
	enum class EPipelineStageFlag : uint32_t
	{
		TopOfPipe = 1 << 0,
		DrawIndirect = 1 << 1,
		VertexInput = 1 << 2,
		VertexShader = 1 << 3,
		HullShader = 1 << 4,
		DomainShader = 1 << 5,
		GeometryShader = 1 << 6,
		FragmentShader = 1 << 7,
		BeforeEarlyZ = 1 << 8,
		AfterEarlyZ = 1 << 9,
		ColorAttachmentOutput = 1 << 10,
		ComputeShader = 1 << 11,
		Transfer = 1 << 12,
		BottomOfPipe = 1 << 13,
		Host = 1 << 14,
		AllGraphics = 1 << 15,
		AllCommands = 1 << 16,
		TransformFeedback = 1 << 17,
		ConditionalRendering = 1 << 18,
		RayTracingShader = 1 << 19,
		AcceleartionStructureBuild = 1 << 20,
		ShadingRateImage = 1 << 21,
		TaskShader = 1 << 22,
		MeshShader = 1 << 23,
		FragmentDensityProcess = 1 << 24,
		CommandPreProcess = 1 << 25
	};

	enum class EAccessFlag : uint32_t
	{
		IndirectCommandRead = 1 << 0,
		IndexRead = 1 << 2,
		VertexRead = 1 << 3,
		UniformRead = 1 << 4,
		InputAttachmentRead = 1 << 5,
		ShaderRead = 1 << 6,
		ShaderWrite = 1 << 7,
		ColorAttachmentRead = 1 << 8,
		ColorAttachmentWrite = 1 << 9,
		DepthStencilAttachmentRead = 1 << 10,
		DepthStencilAttachmentWrite = 1 << 11,
		TransferRead = 1 << 12,
		TransferWrite = 1 << 13,
		HostRead = 1 << 14,
		HostWrite = 1 << 15,
		MemoryRead = 1 << 16,
		MemoryWrite = 1 << 17,
		TransformFeedbackWrite = 1 << 18,
		TransformFeedbackCounterRead = 1 << 19,
		TransformFeedbackCounterWrite = 1 << 20,
		ConditionalRenderingRead = 1 << 21,
		ColorAttachmentReadNonCoherent = 1 << 22,
		AcclerationStructureRead = 1 << 23,
		AcclerationStructureWrite = 1 << 24,
		ShadingRateImageRead = 1 << 25,
		FragmentDensityMapRead = 1 << 26,
		CommandPreProcessRead = 1 << 27,
		CommandPreProcessWrite = 1 << 28
	};

	enum class EAttachmentLoadOp
	{
		Load,
		Clear,
		DontCare
	};

	enum class EAttachmentStoreOp
	{
		Store,
		DontCare,
		None_QCOM
	};

	enum class EAttachmentLayout
	{
		Undefined,
		General,
		ColorAttachmentOptimal,
		DepthStencilAttachmentOptimal,
		DepthStencilReadonlyOptimal,
		ShaderReadonlyOptimal,
		TransferSrcOptimal,
		TransferDstOptimal,
		PreInitialized,
		DepthReadonlyStencilAttachmentOptimal,
		DepthAttachmentStencilReadonlyOptimal,
		DepthAttachmentOptimal,
		DepthReadonlyOptimal,
		StencilAttachmentOptimal,
		StencilReadonlyOptimal,
		PresentSrc,
		SharedPresent,
		ShadingRateOptimal_NV,
		FragmentDensityMapOptimal,
	};

	struct AttachmentReference
	{
		uint32_t Index = ~0U;
		EAttachmentLayout Layout = EAttachmentLayout::Undefined;
	};

	struct AttachmentDesc
	{
		EFormat Format = EFormat::Unknown;
		uint32_t SampleCount = 1u;
		EAttachmentLoadOp LoadOp = EAttachmentLoadOp::Clear;
		EAttachmentStoreOp StoreOp = EAttachmentStoreOp::DontCare;
		EAttachmentLoadOp StencilLoadOp = EAttachmentLoadOp::Clear;
		EAttachmentStoreOp StencilStoreOp = EAttachmentStoreOp::DontCare;
		EAttachmentLayout InitialLayout = EAttachmentLayout::Undefined;
		EAttachmentLayout FinalLayout = EAttachmentLayout::Undefined;
	};

	struct SubpassDependency
	{
		uint32_t SrcSubpassIndex = ~0u;
		uint32_t DstSubpassIndex = ~0u;
		EPipelineStageFlag SrcStage = EPipelineStageFlag::TopOfPipe;
		EPipelineStageFlag DstStage = EPipelineStageFlag::TopOfPipe;
		EAccessFlag SrcAccess = EAccessFlag::IndirectCommandRead;
		EAccessFlag DstAccess = EAccessFlag::IndirectCommandRead;
	};

	struct SubPassDesc
	{
		std::vector<AttachmentReference> InputAttachments;
		std::vector<AttachmentReference> ColorAttachments;
		std::vector<AttachmentReference> ResolveAttachments;
		std::vector<AttachmentReference> PreserveAttachments;
		AttachmentReference DepthStencilAttachment;
	};

	struct RenderPassKey
	{
		uint8_t NumRenderTargets = 0u;
		uint8_t SampleCount = 0u;

		EFormat DepthStencilFormat = EFormat::Unknown;
		EFormat RenderTargetFormat[ELimitations::MaxRenderTargets]{};

		bool8_t operator==(const RenderPassKey& other) const
		{
			if (hash() != other.hash() ||
				NumRenderTargets != other.NumRenderTargets ||
				SampleCount != other.SampleCount ||
				DepthStencilFormat != other.DepthStencilFormat)
			{
				return false;
			}

			for (uint32_t i = 0u; i < NumRenderTargets; ++i)
			{
				if (RenderTargetFormat[i] != other.RenderTargetFormat[i])
				{
					return false;
				}
			}

			return true;
		}

		size_t hash() const
		{
			if (Hash == 0u)
			{
				Hash = Gear::computeHash(NumRenderTargets, SampleCount, DepthStencilFormat);
				for (uint32_t i = 0u; i < NumRenderTargets; ++i)
				{
					Gear::hash_combine(Hash, RenderTargetFormat[i]);
				}
			}

			return Hash;
		}
	private:
		mutable size_t Hash = 0u;
	};

	std::vector<AttachmentDesc> AttachmentDescs;
	std::vector<SubPassDesc> SubPasses;
	std::vector<SubpassDependency> Denpendencies;
};

struct RasterizerStateDesc
{
	EPolygonMode PolygonMode = EPolygonMode::Solid;
	ECullMode CullMode = ECullMode::None;
	EFrontFace FrontFace = EFrontFace::Counterclockwise;
	bool8_t EnableDepthBias = false;

	float32_t DepthBias = 0.0f;
	float32_t DepthBiasClamp = 0.0f;
	float32_t DepthBiasSlope = 0.0f;
};

struct ColorBlendDesc
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

struct BlendStateDesc
{
	bool8_t EnableLogicOp = false;
	ELogicOp LogicOp = ELogicOp::No;

	ColorBlendDesc ColorBlends[ELimitations::MaxRenderTargets]{};
};

struct StencilStateDesc
{
	EStencilOp FailOp = EStencilOp::Keep;
	EStencilOp PassOp = EStencilOp::Keep;
	EStencilOp DepthFailOp = EStencilOp::Keep;
	ECompareFunc CompareFunc = ECompareFunc::Always;

	uint32_t Reference = 0u;
};

struct DepthStencilStateDesc
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

struct SamplerDesc
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

struct FrameBufferDesc
{

};

struct Viewport : public Vec4
{
	Viewport() = default;
	Viewport(float32_t x, float32_t y, float32_t width, float32_t height)
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

struct Scissor : public Vec4
{
	Scissor() = default;
	Scissor(float32_t offsetX, float32_t offsetY, float32_t extentWidth, float32_t extentHeight)
		: Vec4(offsetX, offsetY, extentWidth, extentHeight)
	{
	}
};

class GraphicsPipelineState
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

class ComputePipelineState
{

};

class RayTracingPipelineState
{

};

NAMESPACE_END(Gfx)