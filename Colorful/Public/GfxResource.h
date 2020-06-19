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
	enum eResourceType
	{
		eTexture,
		eSampler,
		eCombinedTextureSampler,
		eUniformBuffer,
		eResourceType_MaxEnum
	};

	struct ResourceBindingInfo
	{
		GfxTexturePtr Texture = nullptr;
		GfxSamplerPtr Sampler = nullptr;
		GfxGpuBufferPtr UniformBuffer = nullptr;
		eResourceType Type = eResourceType_MaxEnum;
	};

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

	inline const std::unordered_map<uint32_t, ResourceBindingInfo>& resources() const
	{
		return m_ResourceMap;
	}

	inline void setCombinedTextureSampler(const GfxTexturePtr& texture, const GfxSamplerPtr& sampler, uint32_t slot)
	{
		auto it = m_ResourceMap.find(slot);
		if (it != m_ResourceMap.end())
		{
			it->second.Texture = texture;
			it->second.Sampler = sampler;
		}
		else
		{
			ResourceBindingInfo bindingInfo
			{
				texture,
				sampler,
				nullptr,
				eCombinedTextureSampler
			};
			m_ResourceMap.insert(std::make_pair(slot, bindingInfo));
		}
	}

	inline void setTexure(const GfxTexturePtr& texture, uint32_t slot)
	{
		auto it = m_ResourceMap.find(slot);
		if (it != m_ResourceMap.end())
		{
			it->second.Texture = texture;
		}
		else
		{
			ResourceBindingInfo bindingInfo
			{
				texture,
				nullptr,
				nullptr,
				eTexture
			};
			m_ResourceMap.insert(std::make_pair(slot, bindingInfo));
		}
	}

	inline void setUniformBuffer(const GfxGpuBufferPtr& buffer, uint32_t slot)
	{
		auto it = m_ResourceMap.find(slot);
		if (it != m_ResourceMap.end())
		{
			it->second.UniformBuffer = buffer;
		}
		else
		{
			ResourceBindingInfo bindingInfo
			{
				nullptr,
				nullptr,
				buffer,
				eUniformBuffer
			};
			m_ResourceMap.insert(std::make_pair(slot, bindingInfo));
		}
	}

	inline void setSampler(const GfxSamplerPtr& sampler, uint32_t slot)
	{
		auto it = m_ResourceMap.find(slot);
		if (it != m_ResourceMap.end())
		{
			it->second.Sampler = sampler;
		}
		else
		{
			ResourceBindingInfo bindingInfo
			{
				nullptr,
				sampler,
				nullptr,
				eSampler
			};
			m_ResourceMap.insert(std::make_pair(slot, bindingInfo));
		}
	}

	virtual void setInputLayout(const std::vector<GfxVertexAttributes>&, size_t) {}
protected:
	eRShaderUsage m_Usage = eRShaderUsage_MaxEnum;
	GfxShaderReflections m_Reflections;
	std::unordered_map<uint32_t, ResourceBindingInfo> m_ResourceMap;
private:
};
using GfxShaderPtr = std::shared_ptr<GfxShader>;

class GfxRenderSurface
{
protected:

};
using GfxRenderSurfacePtr = std::shared_ptr<GfxRenderSurface>;

