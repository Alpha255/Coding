#pragma once

#include "Gear/Gear.h"

#if 0
#include "Colorful/Public/GfxDefinitions.h"

struct GfxShaderReflection
{
	uint32_t Type = std::numeric_limits<uint32_t>::max();
	uint32_t Binding = 0u;
};
using GfxShaderReflections = std::vector<GfxShaderReflection>;
using GfxDescriptorLayoutDesc = std::array<GfxShaderReflections, eRShaderUsage_MaxEnum>;

class GfxGpuBuffer
{
public:
	virtual void update(const void* data, size_t size, size_t offset) = 0;
	virtual void free() = 0;
};
using GfxGpuBufferPtr = GfxGpuBuffer*;

class GfxSampler
{
};
using GfxSamplerPtr = std::shared_ptr<GfxSampler>;

class GfxTexture
{
};
using GfxTexturePtr = std::shared_ptr<GfxTexture>;

class GfxRenderSurface
{
protected:

};
using GfxRenderSurfacePtr = std::shared_ptr<GfxRenderSurface>;
#endif

NAMESPACE_START(Gfx)

enum class EShaderLanguage : uint8_t
{
	GLSL,
	HLSL
};

enum class EShaderStage : uint8_t
{
	Vertex,
	Hull,
	Domain,
	Geometry,
	Fragment,
	Compute,
	ShaderStageCount
};

enum class ETextureType : uint8_t
{
	T_1D,
	T_1D_Array,
	T_2D,
	T_2D_Array,
	T_3D,
	T_Cube,
	T_Cube_Array,
	Buffer
};

enum class EIndexType : uint8_t
{
	UInt16,
	UInt32
};

enum class EBufferUsage : uint8_t
{
	Default,   /// read and write access by the GPU
	Immutable, /// can only be read by the GPU
	Dynamic,   /// accessible by both the GPU (read only) and the CPU (write only). 
	Staging    /// supports data transfer (copy) from the GPU to the CPU.
};

enum class EBufferCpuAccess : uint8_t
{
	Read,
	Write,
	ReadWrite,
};

enum EBufferBindFlags : uint8_t
{
	VertexBuffer = 1 << 0,
	IndexBuffer = 1 << 1,
	UniformBuffer = 1 << 2,
	ShaderResource = 1 << 3,
	StreamOutput = 1 << 4,
	UnorderedAccess = 1 << 5,
	IndirectBuffer = 1 << 6
};

enum class EFormat : uint8_t
{
	Unknown,
	D16_UNorm,
	D32_Float,
	D32_Float_S8X24_Uint,
	D24_UNorm_S8_UInt,
	RGBA32_Typeless,
	RGBA32_Float,
	RGBA32_UInt,
	RGBA32_SInt,
	RGB32_Typeless,
	RGB32_Float,
	RGB32_UInt,
	RGB32_SInt,
	RGBA16_Typeless,
	RGBA16_Float,
	RGBA16_UNorm,
	RGBA16_UInt,
	RGBA16_SNorm,
	RGBA16_SInt,
	RG32_Typeless,
	RG32_Float,
	RG32_UInt,
	RG32_SInt,
	R32G8X24_Typeless,
	R32_Float_X8X24_Typeless,
	X32_Typeless_G8X24_UInt,
	RGB10A2_Typeless,
	RGB10A2_UNorm,
	RGB10A2_UInt,
	RG11B10_Float,
	RGBA8_Typeless,
	RGBA8_UNorm,
	RGBA8_UNorm_SRGB,
	RGBA8_UInt,
	RGBA8_SNorm,
	RGBA8_SInt,
	RG16_Typeless,
	RG16_Float,
	RG16_UNorm,
	RG16_UInt,
	RG16_SNorm,
	RG16_SInt,
	R32_Typeless,
	R32_Float,
	R32_UInt,
	R32_SInt,
	R24G8_Typeless,
	R24_UNorm_X8_Typeless,
	X24_Typelesss_G8_UInt,
	RG8_Typeless,
	RG8_UNorm,
	RG8_UInt,
	RG8_SNorm,
	RG8_SInt,
	R16_Typeless,
	R16_Float,
	R16_UNorm,
	R16_UInt,
	R16_SNorm,
	R16_SInt,
	R8_Typeless,
	R8_UNorm,
	R8_UInt,
	R8_SNorm,
	R8_SInt,
	A8_UNorm,
	R1_UNorm,
	RGB9E5_SharedXP,
	RG8_BG8_UNorm,
	GR8_GB8_UNorm,
	BC1_Typeless,
	BC1_UNorm,
	BC1_UNorm_SRGB,
	BC2_Typeless,
	BC2_UNorm,
	BC2_UNorm_SRGB,
	BC3_Typeless,
	BC3_UNorm,
	BC3_UNorm_SRGB,
	BC4_Typeless,
	BC4_UNorm,
	BC4_SNorm,
	BC5_Typeless,
	BC5_UNorm,
	BC5_SNorm,
	B5G6R5_UNorm,
	BGR5A1_UNorm,
	BGRA8_UNorm,
	BGRX8_UNorm,
	RGB10_XR_Bias_A2_UNorm,
	BGRA8_Typeless,
	BGRA8_UNorm_SRGB,
	BGRX8_Typeless,
	BGRX8_UNorm_SRGB,
	BC6H_Typeless,
	BC6H_UF16,
	BC6H_SF16,
	BC7_Typeless,
	BC7_UNorm,
	BC7_UNorm_SRGB,
	AYUV,
	Y410,
	Y416,
	NV12,
	P010,
	P016,
	Opaque_420,
	YUY2,
	Y210,
	Y216,
	NV11,
	AI44,
	IA44,
	P8,
	AP8,
	BGRA4_UNorm,
	P208,
	V208,
	V408,
	Force_UInt
};

template<class T>
class GfxObject
{
public:
	using Type = std::remove_extent_t<T>;

	GfxObject() = default;

	GfxObject(const std::string& debugName)
		: m_DebugName(debugName)
	{
	}

	GfxObject(std::string&& debugName)
		: m_DebugName(std::move(debugName))
	{
	}

	GfxObject(Type* handle, const std::string& debugName = std::string())
		: m_Handle(handle)
		, m_DebugName(debugName)
	{
	}

	GfxObject(const GfxObject& other)
		: m_Handle(other.m_Handle)
		, m_DebugName(other.m_DebugName)
	{
	}

	GfxObject(GfxObject&& other)
		: m_Handle(other.m_Handle)
		, m_DebugName(std::move(other.m_DebugName))
	{
		std::exchange(other.m_Handle, {});
	}

	GfxObject& operator=(const GfxObject& other)
	{
		if (m_Handle != other.m_Handle)
		{
			m_Handle = other.m_Handle;
			m_DebugName = other.m_DebugName;
		}
		return *this;
	}

	GfxObject& operator=(GfxObject&& other)
	{
		if (m_Handle != other.m_Handle)
		{
			m_DebugName = std::move(other.m_DebugName);
			m_Handle = std::exchange(other.m_Handle, {});
		}
		return *this;
	}

	virtual ~GfxObject() 
	{
		std::exchange(m_Handle, {});
	}

	const std::string& debugName() const
	{
		return m_DebugName;
	}

	void setDebugName(const std::string& debugName)
	{
		m_DebugName = debugName;
	}

	void setDebugName(std::string&& debugName)
	{
		m_DebugName.assign(std::forward<std::string>(debugName));
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
		return m_Handle;
	}

	inline const T* const get() const
	{
		return m_Handle;
	}

	inline T* operator->()
	{
		return m_Handle;
	}

	inline size_t hash() const
	{
		return std::hash<Type*>()(m_Handle);
	}

	inline const bool8_t isValid() const
	{
		return m_Handle != nullptr;
	}
protected:
	T* m_Handle = nullptr;
	std::string m_DebugName;
private:
};

template<class T>
class D3DObject : public GfxObject<T>
{
public:
	D3DObject() = default;

	D3DObject(Type* handle, const std::string& debugName = std::string())
		: GfxObject<T>(handle, debugName)
	{
	}

	D3DObject(const D3DObject& other)
		: GfxObject<T>(other.m_DebugName)
	{
		if (m_Handle != other.m_Handle)
		{
			if (m_Handle)
			{
				m_Handle->Release();
			}
			if (other.m_Handle)
			{
				other.m_Handle->AddRef();
			}

			m_Handle = other.m_Handle;
		}
	}

	D3DObject(D3DObject&& other)
		: GfxObject<T>(std::move(other.m_DebugName))
	{
		if (m_Handle != other.m_Handle)
		{
			if (m_Handle)
			{
				m_Handle->Release();
			}

			m_Handle = std::exchange(other.m_Handle, {});
		}
	}

	D3DObject& operator=(const D3DObject& other)
	{
		if (m_Handle != other.m_Handle)
		{
			if (m_Handle)
			{
				m_Handle->Release();
			}
			if (other.m_Handle)
			{
				other.m_Handle->AddRef();
			}

			m_Handle = other.m_Handle;
			m_DebugName = other.m_DebugName;
		}
		return *this;
	}

	D3DObject& operator=(D3DObject&& other)
	{
		if (m_Handle != other.m_Handle)
		{
			if (m_Handle)
			{
				m_Handle->Release();
			}

			m_Handle = std::exchange(other.m_Handle, {});
			m_DebugName = std::move(other.m_DebugName);
		}
		return *this;
	}

	virtual ~D3DObject()
	{
		if (m_Handle)
		{
			m_Handle->Release();
		}
	}
protected:
private:
};

template<class T>
class VkObject : public GfxObject<T>
{
};

DECLARE_SHARED_PTR(Shader)
class Shader
{
public:
	Shader() = delete;
	Shader(EShaderStage stage)
		: m_Stage(stage)
	{
	}
	~Shader() = default;

	inline EShaderStage stage() const
	{
		return m_Stage;
	}
protected:
	EShaderStage m_Stage;
private:
};

struct ShaderReflection
{
	struct UniformBufferDesc
	{
		uint32_t Index;
		size_t Size;
	};

	enum class EInputResourceType : uint8_t
	{
		UniformBuffer,
		Texture,
		Sampler,
		CombinedTextureSampler,
		StorageBuffer
	};

	struct InputResourceDesc
	{
		EInputResourceType Type;
		ETextureType TextureType;
		uint32_t Binding;
	};
};

enum class EVertexUsage : uint32_t
{
	Position  = 1 << 0,
	Normal    = 1 << 1,
	Tangent   = 1 << 2,
	BiNormal  = 1 << 3,
	BiTangent = 1 << 4,
	Texcoord0 = 1 << 5,
	Texcoord1 = 1 << 6,
	Texcoord2 = 1 << 7,
	Texcoord3 = 1 << 8,
	Texcoord4 = 1 << 9,
	Texcoord5 = 1 << 10,
	Texcoord6 = 1 << 11,
	Texcoord7 = 1 << 12,
	Color0    = 1 << 13,
	Color1    = 1 << 14,
	Color2    = 1 << 15,
	Color3    = 1 << 16,
	Weight    = 1 << 17,
	VertexUsge_Count = 18
};

struct EXPORT_API FormatAttribute
{
	EFormat Format = EFormat::Unknown;
	uint32_t Stride = 0u;
	const char8_t* const Name = nullptr;
};

struct EXPORT_API TextureDesc
{
	ETextureType Dimension = ETextureType::T_2D;
	EFormat Format = EFormat::Unknown;
	EBufferUsage Usage = EBufferUsage::Immutable;
	EBufferBindFlags BindFlag = EBufferBindFlags::ShaderResource;

	uint32_t Width = 0u;
	uint32_t Height = 0u;

	union
	{
		uint32_t ArraySize = 1u;
		uint32_t Depth;
	};

	uint32_t MipLevels = 1u;
	uint32_t SampleCount = 1u;
};

NAMESPACE_END(Gfx)
