#include "D3DObject.h"
#include "D3DEngine.h"

void D3DSwapChain::Create(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed)
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
	m_Desc.Windowed = bWindowed;
	m_Desc.SampleDesc.Count = 1;
	m_Desc.SampleDesc.Quality = 0;
	m_Desc.SwapEffect = eBufferCount > 1 ? DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL : DXGI_SWAP_EFFECT_DISCARD;
	m_Desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	IDXGIFactory *pFactory = nullptr;
	IDXGISwapChain *pSwapChain = nullptr;

	HRCheck(CreateDXGIFactory(IID_IDXGIFactory, reinterpret_cast<void **>(&pFactory)));
	DXGIFactory dxgiFactory;
	dxgiFactory.MakeObject(pFactory);
	HRCheck(dxgiFactory->CreateSwapChain(D3DEngine::Instance().GetDevice().Get(), &m_Desc, &pSwapChain));
	MakeObject(pSwapChain);
}

bool D3DSwapChain::Resize(uint32_t width, uint32_t height)
{
	assert(IsValid());

	if (width == m_Desc.BufferDesc.Width && height == m_Desc.BufferDesc.Height)
	{
		return false;
	}

	m_Desc.BufferDesc.Width = width;
	m_Desc.BufferDesc.Height = height;

	HRCheck(m_Object->ResizeBuffers(m_Desc.BufferCount, width, height, m_Desc.BufferDesc.Format, m_Desc.Flags));

	return true;
}

