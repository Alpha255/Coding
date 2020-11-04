#include "Colorful/Public/GfxRenderer.h"
#include "Colorful/Public/GfxResourceManager.h"

NAMESPACE_START(Gfx)

EXPORT_API GfxRendererPtr GRenderer = nullptr;
EXPORT_API GfxResourceManagerPtr GResourceMgr = nullptr;

EXPORT_API GfxRendererPtr createRenderer()
{
	assert(0);
	return nullptr;
}

NAMESPACE_END(Gfx)
