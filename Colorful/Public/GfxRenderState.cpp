#if 0
#include "Colorful/Public/GfxRenderState.h"

void GfxPipelineState::setCombinedTextureSampler(eRShaderUsage shader, const GfxTexturePtr& texture, const GfxSamplerPtr& sampler, uint32_t slot)
{
#if defined(UsingUnorderedMap)
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
#else
	auto index = findByBinding(ResourceMap[shader], slot);
	if (index != std::numeric_limits<uint32_t>::max())
	{
		ResourceMap[shader][index].Texture = texture;
		ResourceMap[shader][index].Sampler = sampler;
	}
	else
	{
		GfxResourceInfo bindingInfo
		{
			texture,
			sampler,
			nullptr,
			eCombinedTextureSampler,
			slot
		};
		ResourceMap[shader].emplace_back(std::move(bindingInfo));
	}
#endif
}

void GfxPipelineState::setTexure(eRShaderUsage shader, const GfxTexturePtr& texture, uint32_t slot)
{
#if defined(UsingUnorderedMap)
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
#else
	auto index = findByBinding(ResourceMap[shader], slot);
	if (index != std::numeric_limits<uint32_t>::max())
	{
		ResourceMap[shader][index].Texture = texture;
	}
	else
	{
		GfxResourceInfo bindingInfo
		{
			texture,
			nullptr,
			nullptr,
			eTexture,
			slot
		};
		ResourceMap[shader].emplace_back(std::move(bindingInfo));
	}
#endif
}

void GfxPipelineState::setUniformBuffer(eRShaderUsage shader, const GfxGpuBufferPtr& buffer, uint32_t slot)
{
#if defined(UsingUnorderedMap)
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
#else
	auto index = findByBinding(ResourceMap[shader], slot);
	if (index != std::numeric_limits<uint32_t>::max())
	{
		ResourceMap[shader][index].UniformBuffer = buffer;
	}
	else
	{
		GfxResourceInfo bindingInfo
		{
			nullptr,
			nullptr,
			buffer,
			eUniformBuffer,
			slot
		};
		ResourceMap[shader].emplace_back(std::move(bindingInfo));
	}
#endif
}

void GfxPipelineState::setSampler(eRShaderUsage shader, const GfxSamplerPtr& sampler, uint32_t slot)
{
#if defined(UsingUnorderedMap)
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
#else
	auto index = findByBinding(ResourceMap[shader], slot);
	if (index != std::numeric_limits<uint32_t>::max())
	{
		ResourceMap[shader][index].Sampler = sampler;
	}
	else
	{
		GfxResourceInfo bindingInfo
		{
			nullptr,
			sampler,
			nullptr,
			eSampler,
			slot
		};
		ResourceMap[shader].emplace_back(std::move(bindingInfo));
	}
#endif
}
#endif