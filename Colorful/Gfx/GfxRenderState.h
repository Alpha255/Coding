#pragma once

#include "Colorful/Gfx/AssetTool/Material.h"
#include <d3d11.h>

NAMESPACE_START(Gfx)

using GfxFlags = uint32_t;

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

struct GfxResourceDesc
{
	virtual size_t hash() const
	{
		return Hash;
	}

protected:
	mutable size_t Hash = ~0u;
};

struct RenderPassDesc : public GfxResourceDesc
{
	enum EPipelineStageFlags : uint32_t
	{
		PipelineStageFlag_None = 0,
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
		TransformFeedback = 0x01000000,
		ConditionalRendering = 1 << 18,
		AcceleartionStructureBuild = 0x02000000,
		RayTracingShader = 0x00200000,
		ShadingRateImage = 0x00400000,
		TaskShader = 0x00080000,
		MeshShader = 0x00100000,
		FragmentDensityProcess = 0x00800000,
		CommandPreProcess = 0x00020000
	};

	enum EAccessFlags : uint32_t
	{
		AccessFlag_None = 0,
		IndirectCommandRead = 1 << 0,
		IndexRead = 1 << 1,
		VertexRead = 1 << 2,
		UniformRead = 1 << 3,
		InputAttachmentRead = 1 << 4,
		ShaderRead = 1 << 5,
		ShaderWrite = 1 << 6,
		ColorAttachmentRead = 1 << 7,
		ColorAttachmentWrite = 1 << 8,
		DepthStencilAttachmentRead = 1 << 9,
		DepthStencilAttachmentWrite = 1 << 10,
		TransferRead = 1 << 11,
		TransferWrite = 1 << 12,
		HostRead = 1 << 13,
		HostWrite = 1 << 14,
		MemoryRead = 1 << 15,
		MemoryWrite = 1 << 16,
		TransformFeedbackWrite = 0x02000000,
		TransformFeedbackCounterRead = 0x04000000,
		TransformFeedbackCounterWrite = 0x08000000,
		ConditionalRenderingRead = 1 << 20,
		ColorAttachmentReadNonCoherent = 0x00080000,
		AcclerationStructureRead = 0x00200000,
		AcclerationStructureWrite = 0x00400000,
		ShadingRateImageRead = 0x00800000,
		FragmentDensityMapRead = 0x01000000,
		CommandPreProcessRead = 0x00020000,
		CommandPreProcessWrite = 0x00040000
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
		DontCare
	};

	struct AttachmentReference
	{
		uint32_t Index = ~0u; /// VK_ATTACHMENT_UNUSED
		Texture::EImageLayout Layout = Texture::EImageLayout::Undefined;
	};

	struct AttachmentDesc
	{
		EFormat Format = EFormat::Unknown;
		uint8_t SampleCount = 1u;
		EAttachmentLoadOp LoadOp = EAttachmentLoadOp::Clear;
		EAttachmentStoreOp StoreOp = EAttachmentStoreOp::DontCare;
		EAttachmentLoadOp StencilLoadOp = EAttachmentLoadOp::Clear;
		EAttachmentStoreOp StencilStoreOp = EAttachmentStoreOp::DontCare;
		Texture::EImageLayout InitialLayout = Texture::EImageLayout::Undefined;
		Texture::EImageLayout FinalLayout = Texture::EImageLayout::Undefined;
	};

	struct SubPassDependency
	{
		uint32_t SrcSubpassIndex = ~0u;  /// VK_SUBPASS_EXTERNAL
		uint32_t DstSubpassIndex = ~0u;
		GfxFlags SrcStageFlags = 0u;
		GfxFlags DstStageFlags = 0u;
		GfxFlags SrcAccessFlags = 0u;
		GfxFlags DstAccessFlags = 0u;
	};

	struct SubPassDesc
	{
		std::vector<AttachmentReference> InputAttachments;
		std::vector<AttachmentReference> ColorAttachments;
		std::vector<AttachmentReference> ResolveAttachments;
		std::vector<uint32_t> PreserveAttachments;
		AttachmentReference DepthStencilAttachment;
	};

	std::string Name = "Default";
	std::vector<AttachmentDesc> AttachmentDescs;
	std::vector<SubPassDesc> SubPasses;
	std::vector<SubPassDependency> SubPassDependencies;

	RenderPassDesc() = default;
	RenderPassDesc(const char8_t* name)
		: Name(name)
	{
	}
	RenderPassDesc(RenderPassDesc&&) = default;

	uint64_t hash() const override final
	{
		if (Hash == ~0u)
		{
			Hash = computeHash(AttachmentDescs.size());
			for (auto& attachmentDesc : AttachmentDescs)
			{
				hash_combine(Hash, attachmentDesc.Format);
				hash_combine(Hash, attachmentDesc.SampleCount);
			}
		}

		return Hash;
	}
};

struct FrameBufferDesc
{
	std::vector<TexturePtr> ColorAttachments;
	TexturePtr DepthStencilAttachment;

	uint32_t Width = 0u;
	uint32_t Height = 0u;
	uint32_t Layers = 0u;
};

struct RasterizationStateDesc
{
	EPolygonMode PolygonMode = EPolygonMode::Solid;
	ECullMode CullMode = ECullMode::BackFace;
	EFrontFace FrontFace = EFrontFace::Counterclockwise;
	bool8_t EnableDepthBias = false;
	bool8_t EnableDepthClamp = false;

	float32_t DepthBias = 0.0f;
	float32_t DepthBiasClamp = 0.0f;
	float32_t DepthBiasSlope = 0.0f;
	float32_t LineWidth = 1.0f;
};

struct ColorBlendDesc
{
	uint32_t AttachmentIndex = ELimitations::MaxRenderTargets;

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

	ColorBlendDesc ColorBlends[ELimitations::MaxRenderTargets];
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
	bool8_t EnableDepthBoundsTest = false;
	bool8_t EnableStencil = false;
	ECompareFunc DepthCompareFunc = ECompareFunc::Always;

	uint8_t StencilReadMask = 0xF;
	uint8_t StencilWriteMask = 0xF;
	StencilStateDesc FrontFaceStencilState;
	StencilStateDesc BackFaceStencilState;
};

struct MultisampleStateDesc
{
	ESampleCount SampleCount = ESampleCount::SampleCount_1_Bit;
	bool8_t EnableSampleShading = false;
	bool8_t EnableAlphaToCoverage = false;
	bool8_t EnableAlphaToOne = false;

	float32_t MinSampleShading = 0.0f;
	uint32_t* SampleMask = nullptr; /// ??? 
};

struct SamplerDesc : public GfxResourceDesc
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

	uint64_t hash() const override final
	{
		/// compare to memcmp ???
		if (Hash == ~0u)
		{
			Hash = computeHash(
				MinMagFilter, 
				AddressModeU, 
				AddressModeV, 
				AddressModeW, 
				CompareOp, 
				BorderColor, 
				MaxAnisotropy, 
				MipLodBias, 
				MinLod, 
				MaxLod);
		}

		return Hash;
	}
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
	struct VertexStream
	{
		GpuBufferPtr IndexBuffer = nullptr;
		EIndexFormat IndexFormat = EIndexFormat::UInt32;

		std::vector<GpuBufferPtr> VertexBuffers;
		std::vector<uint32_t> VertexOffsets;
	};

	GraphicsPipelineState(
		MaterialPtr material, 
		const RasterizationStateDesc& rasterizationStateDesc,
		const MultisampleStateDesc& multisampleStateDesc,
		const DepthStencilStateDesc& depthStencilStateDesc,
		const BlendStateDesc& blendStateDesc)
		: m_Material(material)
		, m_RasterizationStateDesc(rasterizationStateDesc)
		, m_MultisampleStateDesc(multisampleStateDesc)
		, m_DepthStencilStateDesc(depthStencilStateDesc)
		, m_BlendStateDesc(blendStateDesc)
	{
		rebuildResourceTable();
	}

	inline void setMaterial(MaterialPtr material)
	{
		assert(!m_Compiled);
		m_Material = material;
		rebuildResourceTable();
	}

	inline void compile()
	{
		assert(!m_Compiled);
		m_Compiled = true;
	}

	inline MaterialPtr material() const
	{
		return m_Material;
	}

	inline const PipelineResourceTable& resourceTable() const
	{
		return m_ResourceTable;
	}

	inline const RasterizationStateDesc& rasterizationStateDesc() const
	{
		return m_RasterizationStateDesc;
	}

	inline const BlendStateDesc& blendStateDesc() const
	{
		return m_BlendStateDesc;
	}

	inline const DepthStencilStateDesc& depthStencilStateDesc() const
	{
		return m_DepthStencilStateDesc;
	}

	inline const MultisampleStateDesc& multisampleStateDesc() const
	{
		return m_MultisampleStateDesc;
	}

	inline void setViewport(const Viewport& viewport)
	{
		m_Viewport = viewport;
	}

	inline void setScissor(const Scissor& scissor)
	{
		m_Scissor = scissor;
	}

	inline bool8_t isResourceDirty() const
	{
		return m_ResourceDirty;
	}

	inline void setIndexBuffer(GpuBufferPtr indexBuffer, EIndexFormat indexFormat = EIndexFormat::UInt32)
	{
		m_VertexStream.IndexBuffer = indexBuffer;
		m_VertexStream.IndexFormat = indexFormat;
	}

	inline void setVertexBuffer(GpuBufferPtr vertexBuffer, uint32_t slot, uint32_t offset = 0u)
	{
		assert(slot < ELimitations::MaxVertexStreams);

		auto size = static_cast<size_t>(slot) + 1u;
		if (m_VertexStream.VertexBuffers.size() < size)
		{
			auto count = size - m_VertexStream.VertexBuffers.size();

			m_VertexStream.VertexBuffers.insert(m_VertexStream.VertexBuffers.end(), count, nullptr);
			m_VertexStream.VertexOffsets.insert(m_VertexStream.VertexOffsets.end(), count, 0u);
		}

		m_VertexStream.VertexBuffers[slot] = vertexBuffer;
		m_VertexStream.VertexOffsets[slot] = offset;
	}

	template<EShaderStage Stage>
	void setResource(uint32_t binding, GfxResourcePtr resource)
	{
		assert(Stage < EShaderStage::ShaderStageCount);
		for (auto& r : m_ResourceTable[Stage])
		{
			if (r.first.Binding == binding && r.second != resource)
			{
				assert(r.first.Type == resource->descriptorType());
				r.second = resource;
				m_ResourceDirty = true;
				break;
			}
		}
	}

	template<EShaderStage Stage>
	void setResource(const char8_t* name, GfxResourcePtr resource)
	{
		assert(Stage < EShaderStage::ShaderStageCount);
		for (auto& r : m_ResourceTable[Stage])
		{
			if (_stricmp(r.first.Name.c_str(), name) == 0 && r.second != resource)
			{
				assert(r.first.Type == resource->descriptorType());
				r.second = resource;
				m_ResourceDirty = true;
				break;
			}
		}
	}

	template<EShaderStage Stage>
	void undateUniformBuffer(uint32_t binding, const void* data, size_t size, size_t offset = 0u)
	{
		assert(Stage < EShaderStage::ShaderStageCount);
		for (auto& r : m_ResourceTable[Stage])
		{
			if (r.first.Binding == binding)
			{
				assert(r.first.Type == EDescriptorType::UniformBuffer);
				std::static_pointer_cast<GpuBuffer>(r.second)->update(data, size, offset);
				break;
			}
		}
	}

	template<EShaderStage Stage>
	void undateUniformBuffer(const char8_t* name, const void* data, size_t size, size_t offset = 0u)
	{
		assert(Stage < EShaderStage::ShaderStageCount);
		for (auto& r : m_ResourceTable[Stage])
		{
			if (_stricmp(r.first.Name.c_str(), name) == 0)
			{
				assert(r.first.Type == EDescriptorType::UniformBuffer);
				std::static_pointer_cast<GpuBuffer>(r.second)->update(data, size, offset);
				break;
			}
		}
	}
protected:
	void rebuildResourceTable()
	{
		assert(m_Material);
		m_ResourceTable.fill({});
		for (uint32_t index = EShaderStage::Vertex; index < EShaderStage::ShaderStageCount; ++index)
		{
			auto& shader = m_Material->pipelineShaderStages()[index];
			if (shader)
			{
				for (auto& binding : shader->reflection().ResourceBindings)
				{
					m_ResourceTable[index].emplace_back(std::make_pair(binding, nullptr));
				}
			}
		}
	}
private:
	RasterizationStateDesc m_RasterizationStateDesc;
	BlendStateDesc m_BlendStateDesc;
	DepthStencilStateDesc m_DepthStencilStateDesc;
	MultisampleStateDesc m_MultisampleStateDesc;
	MaterialPtr m_Material;
	Viewport m_Viewport;
	Scissor m_Scissor;
	VertexStream m_VertexStream;

	bool8_t m_Compiled = false;
	bool8_t m_ResourceDirty = false;
	PipelineResourceTable m_ResourceTable;
};
DECLARE_SHARED_PTR(GraphicsPipelineState)

class ComputePipelineState
{

};
DECLARE_SHARED_PTR(ComputePipelineState)

class RayTracingPipelineState
{

};
DECLARE_SHARED_PTR(RayTracingPipelineState)

NAMESPACE_END(Gfx)