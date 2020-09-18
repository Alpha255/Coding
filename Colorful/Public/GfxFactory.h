#pragma once

#if 0

#include "Colorful/Public/GfxResource.h"

class GfxFactory : public LazySingleton<GfxFactory>
{
	lazySingletonDeclare(GfxFactory);
public:
	GfxSamplerPtr linearSampler() const
	{
		assert(m_LinearSampler);
		return m_LinearSampler;
	}

	GfxTexturePtr defaultTexture() const
	{
		assert(m_DefaultTexture);
		return m_DefaultTexture;
	}
protected:
	GfxFactory();
private:
	GfxSamplerPtr m_LinearSampler = nullptr;
	GfxTexturePtr m_DefaultTexture = nullptr;
};

struct GfxPipelineResource
{
	std::vector<GfxGpuBufferPtr> UniformBuffers;
	std::vector<GfxSamplerPtr> Samplers;
	std::vector<GfxTexturePtr> Textures;
	std::vector<GfxGpuBufferPtr> RWBuffers;
};

struct GfxGpuResourceLayout
{
	std::array<GfxPipelineResource, eRShaderUsage_MaxEnum> PipelineResources;
};

class GfxGpuResourceFactory
{

};

#endif
