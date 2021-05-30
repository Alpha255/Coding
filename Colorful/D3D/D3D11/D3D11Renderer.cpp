#include "Colorful/D3D/D3D11/D3D11Renderer.h"

NAMESPACE_START(Gfx)

void D3D11Renderer::initialize(uint64_t windowHandle, uint32_t width, uint32_t height, bool8_t fullscreen, bool8_t VSync)
{
	DXGIFactoryPtr Factory = std::make_unique<DXGIFactory>();
	DXGIAdapterListPtr AdapterList  = std::make_unique<DXGIAdapterList>(Factory->get(), Factory->get6());

	m_Device = std::make_shared<D3D11Device>(AdapterList->get1(), AdapterList->get4());

	(void)windowHandle;
	(void)width;
	(void)height;
	(void)fullscreen;
	(void)VSync;
}

void D3D11Renderer::toggleFullScreen(bool8_t fullscreen)
{
	(void)fullscreen;
}

void D3D11Renderer::toggleVSync(bool8_t VSync)
{
	(void)VSync;
}

void D3D11Renderer::finalize()
{
}

extern "C"
{
	EXPORT_API void createRenderer(GfxRendererPtr& ptr)
	{
		ptr = std::make_unique<D3D11Renderer>();
	}
}

NAMESPACE_END(Gfx)
