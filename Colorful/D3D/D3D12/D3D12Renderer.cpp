#include "Colorful/D3D/D3D12/D3D12Renderer.h"

NAMESPACE_START(Gfx)

GfxResourceManagerPtr D3D12Renderer::createDevice()
{
	m_Debug = std::make_unique<D3D12Debug>();

	DXGIFactoryPtr Factory = std::make_unique<DXGIFactory>();
	DXGIAdapterListPtr AdapterList = std::make_unique<DXGIAdapterList>(Factory->get(), Factory->get6());

	m_Device = std::make_unique<D3D12Device>(AdapterList->get1(), AdapterList->get4());
	return nullptr;
}

void D3D12Renderer::createSwapchain(uint64_t instance, uint64_t windowHandle, uint32_t width, uint32_t height, bool8_t fullscreen, bool8_t VSync)
{
	(void)instance;
	(void)windowHandle;
	(void)width;
	(void)height;
	(void)fullscreen;
	(void)VSync;
}

void D3D12Renderer::toggleFullScreen(bool8_t fullscreen)
{
	(void)fullscreen;
}

void D3D12Renderer::toggleVSync(bool8_t VSync)
{
	(void)VSync;
}

void D3D12Renderer::finalize()
{
}

NAMESPACE_END(Gfx)
