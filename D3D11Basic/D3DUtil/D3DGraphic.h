#pragma once

#include "Common.h"

#include <d3d11.h>
#include <d3dcompiler.h>

class D3DGraphic
{
public:
	enum eD3DCounter
	{
		eCTVertexStream = 1,
		eCTRenderTarget = 1,
		eCTBackBuffer = 2,
	};

	enum eD3DFlushState
	{
		eFSRenderTarget,
		eFSDepthStencil,
		eFSViewport,
		eFSRasterizerState,
		eFSBlendState,
		eFSDepthStencilState,
		eFSCount
	};

	enum eD3DResourceType
	{
		eRTTexture,
		eRTShader
	};

	static void CreateInstance(void) { if (nullptr == m_sInstance) { m_sInstance = new D3DGraphic(); assert(m_sInstance); } }
	static D3DGraphic* GetInstance(void) { return m_sInstance; }
	static void DestoryInstance(void) { SafeDelete(m_sInstance); }

	void InitD3DEnvironment(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed);

	void CreateDepthStencilView(__out ID3D11DepthStencilView** ppDSV, DXGI_FORMAT fmt, uint32_t width, uint32_t height);

	void CreateTexture2D(__out ID3D11Texture2D** ppTexture, DXGI_FORMAT fmt, uint32_t width, uint32_t height,
		uint32_t bindFlags, uint32_t mipLevels = 1U, uint32_t arraySize = 1U, uint32_t cpuFlags = 0U, uint32_t sampleCount = 0U, uint32_t sampleQuality = 0U,
		uint32_t miscFlags = 0U, D3D11_USAGE usage = D3D11_USAGE_DEFAULT);

	void CreateShaderResourceView(__out ID3D11ShaderResourceView** ppSRV, char* pFileName);

	void CreateStreamBuffer(__out ID3D11Buffer** ppBuffer, D3D11_BIND_FLAG bindFlag, uint32_t byteWidth, D3D11_USAGE usage, const void* pBuf, uint32_t cpuAccessFlag = 0U);

	void CreateBlendState(__out ID3D11BlendState** ppBlendState, D3D11_BLEND_DESC* pBlendDesc);

	void CreateVertexShader(__out ID3D11VertexShader** ppVS, char* pFileName, char* pEntryPoint);
	void CreatePixelShader(__out ID3D11PixelShader** ppPS, char* pFileName, char* pEntryPoint);

protected:
	D3DGraphic();
	D3DGraphic(const D3DGraphic&) {}
	~D3DGraphic() {}

	inline void RecreateBackBuffer()
	{
		ID3D11Texture2D* pBackBuffer = nullptr;
		HRCheck(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer));
		HRCheck(m_D3DDevice->CreateRenderTargetView(pBackBuffer, nullptr, m_DefaultRenderTarget.GetResource()));
		SafeRelease(pBackBuffer);
	}

	void CompileShaderFile(__out ID3DBlob** ppRes, char* pFileName, char* pEntryPoint, char* pTarget);
private:
	static D3DGraphic* m_sInstance;

	struct D3DIndexBuffer
	{
		ID3D11Buffer* IndexBuffers;
		DXGI_FORMAT   Format;
		uint32_t      Offset;
	};

	struct D3DVertexBuffer
	{
		ID3D11Buffer* VertexBuffers[eCTVertexStream];
		uint32_t      Stride[eCTVertexStream];
		uint32_t      Offset[eCTVertexStream];
	};

	struct D3DPipelineState
	{
		ID3D11InputLayout*       InputLayout;
		D3DVertexBuffer          VertexBuffer;
		D3DIndexBuffer           IndexBuffer;
		D3D_PRIMITIVE_TOPOLOGY   PrimitiveTopology;
		ID3D11VertexShader*      VertexShader;
		ID3D11RasterizerState*   RasterizerState;
		D3D11_VIEWPORT*          Viewports;
		ID3D11PixelShader*       PixelShader;
		ID3D11BlendState*        BlendState;
		ID3D11DepthStencilState* DepthStencilState;
		ID3D11RenderTargetView*  RenderTargetView;
		ID3D11DepthStencilView*  DepthStencilView;

		float                    BlendFactor[4];
		uint32_t                 SampleMask;
	}m_D3DPipeLineState;

	DXGI_SWAP_CHAIN_DESC m_SwapChainDesc;

	Ref<IDXGISwapChain> m_SwapChain;
	Ref<ID3D11Device> m_D3DDevice;
	Ref<ID3D11DeviceContext> m_D3DContext;
	Ref<ID3D11RenderTargetView> m_DefaultRenderTarget;
	Ref<ID3D11DepthStencilView> m_DefaultDepthStencilView;

	bool m_FlushState[eFSCount];
};
