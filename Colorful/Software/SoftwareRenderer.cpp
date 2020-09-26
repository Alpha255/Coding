#include "SoftwareRenderer.h"

NAMESPACE_START(Gfx)

void SoftwareRenderer::createDevice()
{
}

void SoftwareRenderer::createSwapchain(uint64_t windowHandle, uint32_t width, uint32_t height, bool8_t fullscreen, bool8_t vSync)
{
}

void SoftwareRenderer::toggleFullScreen(bool8_t fullscreen)
{
}

void SoftwareRenderer::toggleVSync(bool8_t vSync)
{
}

void SoftwareRenderer::finalize()
{
}

extern "C"
{
	EXPORT_API void createRenderer(GfxRendererPtr& ptr)
	{
		return ptr.reset(new SoftwareRenderer());
	}
}

NAMESPACE_END(Gfx)
