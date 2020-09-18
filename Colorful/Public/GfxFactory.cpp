#if 0

#include "Colorful/Public/GfxFactory.h"
#include "Colorful/Public/GfxEngine.h"

GfxFactory::GfxFactory()
{
	assert(g_GfxEngine);

	GfxSamplerDesc desc{};
	m_LinearSampler = g_GfxEngine->createSampler(desc);

	m_DefaultTexture = g_GfxEngine->createTexture("white.dds");
}

#endif
