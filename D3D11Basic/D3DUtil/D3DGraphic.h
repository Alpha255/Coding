#pragma once

#include "D3DMath.h"

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
		eFSIndexBuffer,
		eFSVertexBuffer,
		eFSVertexShader,
		eFSPixelShader,
		eFSInputLayout,
		eFSRenderTarget,
		eFSDepthStencil,
		eFSViewports,
		eFSRasterizerState,
		eFSBlendState,
		eFSDepthStencilState,
		eFSPrimitiveTopology,
		eFSCount
	};

	enum eD3DResourceType
	{
		eRTTexture,
		eRTShader
	};

	enum eD3DShaderType
	{
		eSTVertexShader,
		eSTPixelShader
	};

	enum eD3DBufferType
	{
		eBTVertexBuffer,
		eBTIndexBuffer,
		eBTConstantsBuffer
	};

	static void CreateInstance(void) { if (nullptr == m_sInstance) { m_sInstance = new D3DGraphic(); assert(m_sInstance); } }
	static D3DGraphic* GetInstance(void) { return m_sInstance; }
	static void DestoryInstance(void) { SafeDelete(m_sInstance); }

	void InitD3DEnvironment(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed);

	void CreateDepthStencil(__out ID3D11DepthStencilView** ppDSV, DXGI_FORMAT fmt, uint32_t width, uint32_t height);

	void CreateTexture2D(__out ID3D11Texture2D** ppTexture, DXGI_FORMAT fmt, uint32_t width, uint32_t height,
		uint32_t bindFlags, uint32_t mipLevels = 1U, uint32_t arraySize = 1U, uint32_t cpuFlags = 0U, uint32_t sampleCount = 0U, uint32_t sampleQuality = 0U,
		uint32_t miscFlags = 0U, D3D11_USAGE usage = D3D11_USAGE_DEFAULT);

	void CreateShaderResourceView(__out ID3D11ShaderResourceView** ppSRV, char* pFileName);

	void CreateStreamBuffer(__out ID3D11Buffer** ppBuffer, D3D11_BIND_FLAG bindFlag, uint32_t byteWidth, D3D11_USAGE usage, const void* pBuf, uint32_t cpuAccessFlag = 0U);

	void CreateBlendState(__out ID3D11BlendState** ppBlendState, D3D11_BLEND_DESC* pBlendDesc);

	void CreateVertexShader(__out ID3D11VertexShader** ppVS, char* pFileName, char* pEntryPoint);
	void CreatePixelShader(__out ID3D11PixelShader** ppPS, char* pFileName, char* pEntryPoint);
	void CreateVertexShaderAndInputLayout(__out ID3D11VertexShader** ppVS, __out ID3D11InputLayout** ppLayout, D3D11_INPUT_ELEMENT_DESC* pInputElement, uint32_t size, char* pFileName, char* pEntryPoint);

	void CreateInputLayout(__out ID3D11InputLayout** ppInputLayout, D3D11_INPUT_ELEMENT_DESC* pInputElement, uint32_t size, ID3DBlob* pRes);

	void CreateRasterizerState(__out ID3D11RasterizerState** ppRasterizerState, D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode = D3D11_CULL_BACK, bool cw = false, bool depthClip = true);

	void ClearRenderTarget(ID3D11RenderTargetView* pRenderTarget, float* pClearColor);
	void ClearDepthStencil(ID3D11DepthStencilView* pDepthStencil, float depth, uint8_t stencil);

	void ResizeBackBuffer(uint32_t width, uint32_t height);

	inline void SetVertexBuffer(ID3D11Buffer* pBuffer, uint32_t stride, uint32_t offset, uint32_t index = 0U);
	inline void SetIndexBuffer(ID3D11Buffer* pBuffer, DXGI_FORMAT format, uint32_t offset = 0U);
	inline void SetInputLayout(ID3D11InputLayout* pInputLayout);
	inline void SetRenderTarget(ID3D11RenderTargetView* pRenderTarget, uint32_t index = 0U);
	inline void SetDepthStencil(ID3D11DepthStencilView* pDepthStencil);
	inline void SetViewports(D3D11_VIEWPORT* pViewports, uint32_t count = 1U);
	inline void SetRasterizerState(ID3D11RasterizerState* pRasterizerState);
	inline void SetDepthStencilState(ID3D11DepthStencilState* pDepthStencilState);
	inline void SetBlendState(ID3D11BlendState* pBlendState, DirectX::XMFLOAT4, uint32_t mask);
	inline void SetVertexShader(ID3D11VertexShader* pVertexShader);
	inline void SetPixelShader(ID3D11PixelShader* pPixelShader);
	inline void SetConstantBuffer(Ref<ID3D11Buffer>& pConstantBuf, uint32_t slot, eD3DShaderType shaderType);

	void UpdateConstantBuffer(ID3D11Resource* pTarget, const void* pSource, uint32_t size);

	void Draw(uint32_t vertexCount, uint32_t startIndex, D3D_PRIMITIVE_TOPOLOGY prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, D3D_PRIMITIVE_TOPOLOGY prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
protected:
	D3DGraphic();
	D3DGraphic(const D3DGraphic&) {}
	~D3DGraphic() {}

	inline void RecreateBackBuffer()
	{
		ID3D11Texture2D* pBackBuffer = nullptr;
		HRCheck(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer));
		HRCheck(m_D3DDevice->CreateRenderTargetView(pBackBuffer, nullptr, m_DefaultRenderTarget.GetReference()));
		SafeRelease(pBackBuffer);
	}

	void CompileShaderFile(__out ID3DBlob** ppRes, char* pFileName, char* pEntryPoint, char* pTarget);

	void FlushPipelineState();
private:
	static D3DGraphic* m_sInstance;

	struct D3DIndexBuffer
	{
		ID3D11Buffer* Buffers;
		DXGI_FORMAT   Format;
		uint32_t      Offset;
	};

	struct D3DVertexBuffer
	{
		ID3D11Buffer* Buffers[eCTVertexStream];
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
		ID3D11RenderTargetView*  RenderTarget;
		ID3D11DepthStencilView*  DepthStencil;

		float                    BlendFactor[4];
		uint32_t                 SampleMask;
		uint32_t                 ViewportCount;
	}m_D3DPipelineState;

	DXGI_SWAP_CHAIN_DESC m_SwapChainDesc;

	Ref<IDXGISwapChain> m_SwapChain;
	Ref<ID3D11Device> m_D3DDevice;
	Ref<ID3D11DeviceContext> m_D3DContext;
	Ref<ID3D11RenderTargetView> m_DefaultRenderTarget;
	Ref<ID3D11DepthStencilView> m_DefaultDepthStencil;

	bool m_FlushState[eFSCount];
};
