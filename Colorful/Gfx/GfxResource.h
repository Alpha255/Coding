#pragma once

#include "Colorful/Gfx/GfxFormat.h"

NAMESPACE_START(Gfx)

enum class EShaderLanguage
{
	GLSL,
	HLSL
};

enum EShaderStage
{
	Vertex,
	Hull,
	Domain,
	Geometry,
	Fragment,
	Compute,
	ShaderStageCount
};

enum class ETextureType
{
	Null,
	T_1D,
	T_1D_Array,
	T_2D,
	T_2D_Array,
	T_3D,
	T_Cube,
	T_Cube_Array,
	Buffer
};

enum class EIndexFormat
{
	UInt16 = sizeof(uint16_t),
	UInt32 = sizeof(uint32_t)
};

enum class EBufferUsage
{
	Default,   /// read and write access by the GPU
	Immutable, /// can only be read by the GPU
	Dynamic,   /// accessible by both the GPU (read only) and the CPU (write only). 
	Staging    /// supports data transfer (copy) from the GPU to the CPU.
};

enum class ECpuAccessFlags : uint8_t
{
	Read,
	Write,
	ReadWrite,
};

enum EBindFlags
{
	VertexBuffer = 1 << 0,
	IndexBuffer = 1 << 1,
	UniformBuffer = 1 << 2,
	ShaderResource = 1 << 3,
	StreamOutput = 1 << 4,
	RenderTarget = 1 << 5,
	DepthStencil = 1 << 6,
	UnorderedAccess = 1 << 7,
	IndirectBuffer = 1 << 8,
	InputAttachment = 1 << 9
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

template<class T>
class GfxObject
{
public:
	using Type = std::remove_extent_t<T>;

	GfxObject() = default;

	GfxObject(Type* handle)
		: m_Handle(handle)
	{
	}

	GfxObject(const GfxObject& other)
		: m_Handle(other.m_Handle)
	{
	}

	GfxObject(GfxObject&& other)
		: m_Handle(other.m_Handle)
	{
		std::exchange(other.m_Handle, {});
	}

	GfxObject& operator=(const GfxObject& other)
	{
		if (m_Handle != other.m_Handle)
		{
			m_Handle = other.m_Handle;
		}
		return *this;
	}

	GfxObject& operator=(GfxObject&& other)
	{
		if (m_Handle != other.m_Handle)
		{
			m_Handle = std::exchange(other.m_Handle, {});
		}
		return *this;
	}

	virtual ~GfxObject() 
	{
		std::exchange(m_Handle, {});
	}

	inline T** reference()
	{
		return &m_Handle;
	}

	inline const T** reference() const
	{
		return &m_Handle;
	}

	inline T* get()
	{
		assert(m_Handle);
		return m_Handle;
	}

	inline const T* const get() const
	{
		assert(m_Handle);
		return m_Handle;
	}

	inline T* operator->()
	{
		assert(m_Handle);
		return m_Handle;
	}

	inline T* operator->() const
	{
		assert(m_Handle);
		return m_Handle;
	}

	inline const bool8_t isValid() const
	{
		return m_Handle != nullptr;
	}
protected:
	T* m_Handle = nullptr;
};

enum class EDescriptorType
{
	Null,
	Sampler,
	CombinedImageSampler,
	SampledImage,
	StorageImage,
	UniformTexelBuffer,
	StorageTexelBuffer,
	UniformBuffer,
	StorageBuffer,
	UniformBufferDynamic,
	StorageBufferDynamic,
	InputAttachment,
};

enum EVertexUsage : uint32_t
{
	Position = 1 << 0,
	Normal = 1 << 1,
	Tangent = 1 << 2,
	BiNormal = 1 << 3,
	BiTangent = 1 << 4,
	Texcoord0 = 1 << 5,
	Texcoord1 = 1 << 6,
	Texcoord2 = 1 << 7,
	Texcoord3 = 1 << 8,
	Color = 1 << 9,
	Weight = 1 << 10,
	VertexUsge_Count = 8
};

enum class ESampleCount : uint8_t
{
	SampleCount_1_Bit = 1 << 0,
	SampleCount_2_Bit = 1 << 1,
	SampleCount_4_Bit = 1 << 2,
	SampleCount_8_Bit = 1 << 3,
	SampleCount_16_Bit = 1 << 4,
	SampleCount_32_Bit = 1 << 5,
	SampleCount_64_Bit = 1 << 6
};

struct TextureDesc
{
	struct SubresourceRange
	{
		uint32_t Width = 0u;
		uint32_t Height = 0u;
		uint32_t Depth = 1u;
		uint32_t Offset = 0u;
		uint32_t MipIndex = 0u;
		uint32_t ArrayIndex = 0u;
		uint32_t RowBytes = 0u;
		uint32_t SliceBytes = 0u;
	};

	ETextureType Type = ETextureType::T_2D;
	EFormat Format = EFormat::Unknown;
	EBufferUsage Usage = EBufferUsage::Immutable;
	uint32_t BindFlags = EBindFlags::ShaderResource;

	uint32_t Width = 0u;
	uint32_t Height = 0u;
	uint32_t ArraySize = 1u;
	uint32_t Layers = 1u;
	uint32_t MipLevels = 1u;

	ESampleCount SampleCount = ESampleCount::SampleCount_1_Bit;
	bool8_t GenMipmaps = false;
	bool8_t sRGB = false;

	byte8_t* Data = nullptr;
	size_t DataSize = 0u;
	std::string Name;

	std::vector<SubresourceRange> Subresources;
};

struct VertexInputLayoutDesc
{
	enum class EVertexInputRate
	{
		Vertex,
		Instance,
	};

	struct VertexLayout
	{
		uint32_t Location = 0u;
		uint32_t Stride = 0u;

		EFormat Format = EFormat::Unknown;
		std::string Usage;
	};

	struct VertexInput
	{
		uint32_t Binding = 0u;
		uint32_t Stride = 0u;
		EVertexInputRate InputRate = EVertexInputRate::Vertex;

		std::vector<VertexLayout> Layouts;

		inline void appendVertexLayout(VertexLayout& layout)
		{
			Stride += layout.Stride;
			Layouts.emplace_back(std::move(layout));
		}
	};

	EPrimitiveTopology PrimitiveTopology = EPrimitiveTopology::TriangleList;
	std::vector<VertexInput> VertexInputs;
};

class GfxResource
{
public:
	GfxResource() = default;
	GfxResource(EDescriptorType descriptorType)
		: m_DescirptorType(descriptorType)
	{
	}
	virtual ~GfxResource() = default;

	void setDebugName(const char8_t* name)
	{
		if (name)
		{
			m_DebugName.assign(name);
		}
	}

	const char8_t* debugName() const
	{
		return m_DebugName.c_str();
	}

	EDescriptorType descriptorType() const
	{
		return m_DescirptorType;
	}
protected:
	std::string m_DebugName; /// use cpp17::string_view
	EDescriptorType m_DescirptorType = EDescriptorType::Null;
};
DECLARE_SHARED_PTR(GfxResource)

class InputLayout : public GfxResource
{
public:
	InputLayout(EPrimitiveTopology primitiveTopology)
		: m_PrimitiveTopology(primitiveTopology)
	{
		assert(m_PrimitiveTopology != EPrimitiveTopology::Unknown);
	}

	EPrimitiveTopology primitiveTopology() const
	{
		return m_PrimitiveTopology;
	}
protected:
	EPrimitiveTopology m_PrimitiveTopology = EPrimitiveTopology::TriangleList;
};
DECLARE_SHARED_PTR(InputLayout)

class Shader : public GfxResource
{
public:
	struct ResourceBinding
	{
		EDescriptorType Type;
		uint32_t Binding = ~0u;
		uint32_t Size = ~0u;
		std::string Name;
	};

	struct ReflectionInfos
	{
		uint32_t UniformBufferCount = 0u;
		uint32_t TextureCount = 0u;
		uint32_t SamplerCount = 0u;
		uint32_t SampledTextureCount = 0u;

		std::vector<ResourceBinding> ResourceBindings; /// map ???
	};

	Shader(EShaderStage stage, ReflectionInfos& reflection)
		: m_Stage(stage)
		, m_Reflection(std::move(reflection))
	{
		assert(m_Stage != EShaderStage::ShaderStageCount);
	}

	inline EShaderStage stage() const
	{
		return m_Stage;
	}

	inline const ReflectionInfos& reflection() const
	{
		return m_Reflection;
	}
protected:
	EShaderStage m_Stage = EShaderStage::ShaderStageCount;
	ReflectionInfos m_Reflection;
};
DECLARE_SHARED_PTR(Shader)

struct ShaderDesc
{
	EShaderStage Stage = EShaderStage::ShaderStageCount;
	size_t BinarySize = 0u;
	EShaderLanguage Language = EShaderLanguage::GLSL;
	Shader::ReflectionInfos Reflections;
	std::vector<uint32_t> Binary;
};

class Sampler : public GfxResource
{
public:
	using GfxResource::GfxResource;
};
DECLARE_SHARED_PTR(Sampler)

class Texture : public GfxResource
{
public:
	enum class EImageLayout
	{
		Undefined,
		TransferDst,
		ColorAttachment,
		DepthStencilAttachment,
		TransferSrc,
		Present,
		FragmentShaderRead,
		PixelDepthStencilRead,
		ComputeShaderReadWrite,
		FragmentShaderReadWrite
	};

	Texture(const TextureDesc& desc, EDescriptorType type = EDescriptorType::SampledImage)
		: GfxResource(type)
		, m_Width(desc.Width)
		, m_Height(desc.Height)
		, m_Layers(desc.Layers)
		, m_ArraySize(desc.ArraySize)
		, m_MipLevels(desc.MipLevels)
		, m_Format(desc.Format)
		, m_Type(desc.Type)
	{
	}

	uint32_t width() const
	{
		return m_Width;
	}

	uint32_t height() const
	{
		return m_Height;
	}

	uint32_t layers() const
	{
		return m_Layers;
	}

	uint32_t arraySize() const
	{
		return m_ArraySize;
	}

	uint32_t mipLevels() const
	{
		return m_MipLevels;
	}

	void bindSampler(SamplerPtr sampler)
	{
		if (sampler)
		{
			assert(m_DescirptorType == EDescriptorType::SampledImage || m_DescirptorType == EDescriptorType::CombinedImageSampler);
			m_Sampler = sampler;
			m_DescirptorType = EDescriptorType::CombinedImageSampler;
		}
	}

	SamplerPtr sampler() const
	{
		return m_Sampler;
	}
protected:
	uint32_t m_Width = 0u;
	uint32_t m_Height = 0u;
	uint32_t m_Layers = 0u;
	uint32_t m_ArraySize = 0u;
	uint32_t m_MipLevels = 1u;
	EFormat m_Format = EFormat::Unknown;
	ETextureType m_Type = ETextureType::Null;
	SamplerPtr m_Sampler = nullptr;
};
DECLARE_SHARED_PTR(Texture)

class GpuBuffer : public GfxResource
{
public:
	using GfxResource::GfxResource;

	inline void* map(size_t size = ~0ULL, size_t offset = 0u) /// VK_WHOLE_SIZE
	{
		return map_native(size, offset);
	}

	inline void update(const void* data, size_t size, size_t offset = 0u, bool8_t persistence = false)
	{
		update_native(data, size, offset, persistence);
	}

	inline void flushMappedRange(size_t size = ~0ULL, size_t offset = 0u) /// VK_WHOLE_SIZE
	{
		flushMappedRange_native(size, offset);
	}

	virtual void unmap() = 0;
protected:
	struct MappedMemory
	{
		void* Memory = nullptr;
		size_t Offset = 0u;
		size_t Size = 0u;
	};

	virtual void* map_native(size_t size, size_t offset) = 0;
	virtual void update_native(const void* data, size_t size, size_t offset, bool8_t persistence) = 0;
	virtual void flushMappedRange_native(size_t size, size_t offset) = 0;

	MappedMemory m_MappedMemory;
	size_t m_Size = 0u;
};
DECLARE_SHARED_PTR(GpuBuffer)

using PipelineResourceTable = std::array<std::vector<std::pair<Shader::ResourceBinding, GfxResourcePtr>>, EShaderStage::ShaderStageCount>;
using PipelineShaderStages = std::array<ShaderPtr, EShaderStage::ShaderStageCount>;

NAMESPACE_END(Gfx)
