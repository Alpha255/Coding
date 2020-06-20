#include "Colorful/Public/GfxRenderState.h"

void GfxPipelineState::setCombinedTextureSampler(eRShaderUsage shader, const GfxTexturePtr& texture, const GfxSamplerPtr& sampler, uint32_t slot)
{
	auto it = ResourceMap[shader].find(slot);
	if (it != ResourceMap[shader].end())
	{
		it->second.Texture = texture;
		it->second.Sampler = sampler;
	}
	else
	{
		GfxResourceInfo bindingInfo
		{
			texture,
			sampler,
			nullptr,
			eCombinedTextureSampler
		};
		ResourceMap[shader].insert(std::make_pair(slot, bindingInfo));
	}
}

void GfxPipelineState::setTexure(eRShaderUsage shader, const GfxTexturePtr& texture, uint32_t slot)
{
	auto it = ResourceMap[shader].find(slot);
	if (it != ResourceMap[shader].end())
	{
		it->second.Texture = texture;
	}
	else
	{
		GfxResourceInfo bindingInfo
		{
			texture,
			nullptr,
			nullptr,
			eTexture
		};
		ResourceMap[shader].insert(std::make_pair(slot, bindingInfo));
	}
}

void GfxPipelineState::setUniformBuffer(eRShaderUsage shader, const GfxGpuBufferPtr& buffer, uint32_t slot)
{
	auto it = ResourceMap[shader].find(slot);
	if (it != ResourceMap[shader].end())
	{
		it->second.UniformBuffer = buffer;
	}
	else
	{
		GfxResourceInfo bindingInfo
		{
			nullptr,
			nullptr,
			buffer,
			eUniformBuffer
		};
		ResourceMap[shader].insert(std::make_pair(slot, bindingInfo));
	}
}

void GfxPipelineState::setSampler(eRShaderUsage shader, const GfxSamplerPtr& sampler, uint32_t slot)
{
	auto it = ResourceMap[shader].find(slot);
	if (it != ResourceMap[shader].end())
	{
		it->second.Sampler = sampler;
	}
	else
	{
		GfxResourceInfo bindingInfo
		{
			nullptr,
			sampler,
			nullptr,
			eSampler
		};
		ResourceMap[shader].insert(std::make_pair(slot, bindingInfo));
	}
}