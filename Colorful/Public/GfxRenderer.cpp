#include "Colorful/Public/GfxRenderer.h"

NAMESPACE_START(Gfx)

EXPORT_API GfxRenderer* GRenderer = nullptr;

extern "C"
{
	EXPORT_API void createRenderer(GfxRendererPtr& ptr)
	{
		assert(0);
		ptr.reset(nullptr);
	}
}

NAMESPACE_END(Gfx)
