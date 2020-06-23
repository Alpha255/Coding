#pragma once

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
private:
	GfxSamplerPtr m_LinearSampler = nullptr;
	GfxTexturePtr m_DefaultTexture = nullptr;
};
