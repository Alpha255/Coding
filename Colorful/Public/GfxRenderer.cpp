#include "Colorful/Public/GfxRenderer.h"
#include "Colorful/Public/GfxResourceManager.h"

NAMESPACE_START(Gfx)

EXPORT_API GfxRenderer* GRenderer = nullptr;
EXPORT_API GfxResourceManager* GResourceManager = nullptr;

extern "C"
{
	EXPORT_API void createRenderer(GfxRendererPtr& ptr)
	{
		assert(0);
		ptr.reset(nullptr);
	}
}

NAMESPACE_END(Gfx)
