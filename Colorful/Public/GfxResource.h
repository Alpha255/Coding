#pragma once

#include "Gear/Gear.h"

#if 0
#include "Colorful/Public/GfxDefinitions.h"

struct GfxVertexAttributes
{
	eRVertexUsage Usage = eRVertexUsge_MaxEnum;
	eRFormat Format = eRFormat_MaxEnum;

	static size_t formatStride(eRFormat format);
};

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

class GfxShader
{
public:
	GfxShader(eRShaderUsage usage)
		: m_Usage(usage)
	{
	}

	inline eRShaderUsage usage() const
	{
		return m_Usage;
	}

	inline const GfxShaderReflections& reflections() const
	{
		return m_Reflections;
	}
protected:
	eRShaderUsage m_Usage = eRShaderUsage_MaxEnum;
	GfxShaderReflections m_Reflections;
private:
};
using GfxShaderPtr = std::shared_ptr<GfxShader>;

class GfxRenderSurface
{
protected:

};
using GfxRenderSurfacePtr = std::shared_ptr<GfxRenderSurface>;
#endif

NAMESPACE_START(Gfx)

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
class SharedObject
{
public:
	SharedObject() = default;
	virtual ~SharedObject() = default;

	inline const bool8_t isValid() const
	{
		return m_Object != nullptr;
	}

	inline T* get() const
	{
		return m_Object.get();
	}

	inline T* operator->() const
	{
		assert(isValid());
		return get();
	}

	inline uint32_t refCount() const
	{
		return m_Object.use_count();
	}

	using Type = T;
protected:
	std::shared_ptr<T> m_Object = nullptr;
private:
};

template<class T> 
struct HandleObject
{
	T Handle = nullptr;

	inline const bool8_t isValid() const
	{
		return Handle != nullptr;
	}

	virtual ~HandleObject() = default;
};

template<class T> 
class D3DObject : public std::conditional_t<true, SharedObject<T>, HandleObject<T>>
{
public:
	D3DObject(T* other = nullptr)
	{
		if (other)
		{
			m_Object.reset(other, std::function<void(T*&)>([](T*& object)
			{
				object->Release();
				object = nullptr;
			}));
		}
		else
		{
			if (m_Object)
			{
				m_Object.reset();
				m_Object = nullptr;
			}
		}
	}
};

template<class T>
class VkObject : public std::conditional_t<false, SharedObject<T>, HandleObject<T>>
{
};

NAMESPACE_END(Gfx)
