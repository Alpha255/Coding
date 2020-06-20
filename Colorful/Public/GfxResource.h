#pragma once

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

	virtual void setInputLayout(const std::vector<GfxVertexAttributes>&, size_t) {}
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

