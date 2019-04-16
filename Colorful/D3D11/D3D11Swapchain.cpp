#include "D3D11Swapchain.h"
#include "D3D11Engine.h"

void D3D11SwapChain::Create(HWND hWnd, uint32_t width, uint32_t height, bool bFullScreen)
{
	m_Desc.BufferCount = eBufferCount;
	m_Desc.BufferDesc.Width = width;
	m_Desc.BufferDesc.Height = height;
	m_Desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_Desc.BufferDesc.RefreshRate.Numerator = 60;
	m_Desc.BufferDesc.RefreshRate.Denominator = 1;
	m_Desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_Desc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	m_Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
	m_Desc.OutputWindow = hWnd;
	m_Desc.Windowed = !bFullScreen;
	m_Desc.SampleDesc.Count = 1;
	m_Desc.SampleDesc.Quality = 0;
	m_Desc.SwapEffect = eBufferCount > 1 ? DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL : DXGI_SWAP_EFFECT_DISCARD;
	m_Desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	IDXGIFactory *pFactory = nullptr;
	IDXGISwapChain *pSwapChain = nullptr;

	Check(CreateDXGIFactory(IID_IDXGIFactory, reinterpret_cast<void **>(&pFactory)));
	DXGIFactory dxgiFactory;
	dxgiFactory.Reset(pFactory);
	Check(dxgiFactory->CreateSwapChain(D3D11Engine::Instance().GetDevice().Get(), &m_Desc, &pSwapChain));
	Reset(pSwapChain);
}

void D3D11SwapChain::Resize(uint32_t width, uint32_t height)
{
	assert(IsValid());

	m_Desc.BufferDesc.Width = width;
	m_Desc.BufferDesc.Height = height;

	Check(m_Object->ResizeBuffers(m_Desc.BufferCount, width, height, m_Desc.BufferDesc.Format, m_Desc.Flags));
}