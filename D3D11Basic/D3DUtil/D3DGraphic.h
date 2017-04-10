#pragma once

#include "Common.h"

#include <d3d11.h>
#include <d3dcompiler.h>

class D3DGraphic
{
public:
	static void CreateInstance(void) { if (nullptr == m_sInstance) { m_sInstance = new D3DGraphic(); assert(m_sInstance); } }
	static D3DGraphic* GetInstance(void) { return m_sInstance; }
	static void DestoryInstance(void) { SafeDelete(m_sInstance); }

	void InitD3DDevice(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed);

protected:
	enum ED3DCounter
	{
		CTVertexBuffer = 1,
		CTRenderTarget = 1,
	};

	D3DGraphic() {}
	D3DGraphic(const D3DGraphic&) {}
	~D3DGraphic() {}
private:
	static D3DGraphic* m_sInstance;

	Ref<IDXGISwapChain> m_SwapChain;
	Ref<ID3D11Device> m_D3DDevice;
	Ref<ID3D11DeviceContext> m_D3DContext;
	Ref<ID3D11RenderTargetView> m_BackBuffer;
	Ref<ID3D11DepthStencilView> m_DepthStencilView;
};
