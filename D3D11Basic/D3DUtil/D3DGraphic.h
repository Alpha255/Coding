#pragma once

#include "D3DMath.h"

#include <d3d11.h>
#include <d3dcompiler.h>

class D3DGraphic
{
public:
	enum eCounter
	{
		eVertexStreamCount = 1,
		eRenderTargetCount = 1,
		eBackBufferCount = 2,
	};

	enum eFlushState
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

	enum eResourceType
	{
		eShader,
		eTexture,
		eSDKMesh,
		eCount
	};

	enum eShaderType
	{
		eVertexShader,
		ePixelShader
	};

	enum eBufferType
	{
		eVertexBuffer,
		eIndexBuffer,
		eConstantsBuffer
	};

	static void CreateInstance(void) { if (nullptr == m_sInstance) { m_sInstance = new D3DGraphic(); assert(m_sInstance); } }
	static D3DGraphic* GetInstance(void) { return m_sInstance; }
	static void DestoryInstance(void) { SafeDelete(m_sInstance); }

	void InitD3DEnvironment(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed);

	void CreateTexture2D(__out Ref<ID3D11Texture2D>& resource, DXGI_FORMAT fmt, uint32_t width, uint32_t height,
		uint32_t bindFlags, uint32_t mipLevels = 1U, uint32_t arraySize = 1U, uint32_t cpuFlags = 0U, uint32_t sampleCount = 1U, uint32_t sampleQuality = 0U,
		uint32_t miscFlags = 0U, D3D11_USAGE usage = D3D11_USAGE_DEFAULT, const void* pData = nullptr, uint32_t pitch = 0U, uint32_t slice = 0U);

	void CreateShaderResourceView(__out Ref<ID3D11ShaderResourceView>& srv, char* pFileName);
	void CreateShaderResourceView(__out Ref<ID3D11ShaderResourceView>& srv, Ref<ID3D11Texture2D>& resource, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, D3D11_SRV_DIMENSION dimension = D3D11_SRV_DIMENSION_TEXTURE2D);
	void CreateRenderTargetView(__out Ref<ID3D11RenderTargetView>& rtv, Ref<ID3D11Texture2D>& resource);
	void CreateDepthStencilView(__out Ref<ID3D11DepthStencilView>& dsv, Ref<ID3D11Texture2D>& resource, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, uint32_t width = 0U, uint32_t height = 0U, D3D11_DSV_DIMENSION dimension = D3D11_DSV_DIMENSION_TEXTURE2D);

	inline void CreateVertexBuffer(__out Ref<ID3D11Buffer>& buffer, uint32_t byteWidth, D3D11_USAGE usage, const void* pBuf, uint32_t cpuAccessFlag = 0U)
	{
		CreateBuffer(buffer.GetReference(), D3D11_BIND_VERTEX_BUFFER, byteWidth, usage, pBuf, cpuAccessFlag);
	}

	inline void CreateIndexBuffer(__out Ref<ID3D11Buffer>& buffer, uint32_t byteWidth, D3D11_USAGE usage, const void* pBuf, uint32_t cpuAccessFlag = 0U)
	{
		CreateBuffer(buffer.GetReference(), D3D11_BIND_INDEX_BUFFER, byteWidth, usage, pBuf, cpuAccessFlag);
	}

	inline void CreateConstantBuffer(__out Ref<ID3D11Buffer>& buffer, uint32_t byteWidth, D3D11_USAGE usage, const void* pBuf, uint32_t cpuAccessFlag = 0U)
	{
		CreateBuffer(buffer.GetReference(), D3D11_BIND_CONSTANT_BUFFER, byteWidth, usage, pBuf, cpuAccessFlag);
	}

	void CreateVertexShader(__out Ref<ID3D11VertexShader>& vs, char* pFileName, char* pEntryPoint, const D3D_SHADER_MACRO* pDefines = nullptr);
	void CreatePixelShader(__out Ref<ID3D11PixelShader>& ps, char* pFileName, char* pEntryPoint, const D3D_SHADER_MACRO* pDefines = nullptr);
	void CreateVertexShaderAndInputLayout(__out Ref<ID3D11VertexShader>& vs, __out Ref<ID3D11InputLayout>& layout, D3D11_INPUT_ELEMENT_DESC* pInputElement, uint32_t size, char* pFileName, char* pEntryPoint);

	void CreateInputLayout(__out Ref<ID3D11InputLayout>& inputLayout, D3D11_INPUT_ELEMENT_DESC* pInputElement, uint32_t size, ID3DBlob* pRes);

	void CreateBlendState(__out Ref<ID3D11BlendState>& blendState, D3D11_BLEND_DESC* pBlendDesc);
	void CreateSamplerState(__out Ref<ID3D11SamplerState>& samplerState, const D3D11_SAMPLER_DESC* pSamplerDesc);
	void CreateRasterizerState(__out Ref<ID3D11RasterizerState>& rasterizerState, D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode = D3D11_CULL_BACK, bool cw = false, bool depthClip = true);

	void ClearRenderTarget(const Ref<ID3D11RenderTargetView>& renderTarget, float* pClearColor);
	void ClearDepthStencil(const Ref<ID3D11DepthStencilView>& depthStencil, float depth, uint8_t stencil);

	void ResizeBackBuffer(uint32_t width, uint32_t height);

	inline const Ref<ID3D11RenderTargetView>& DefaultRenderTarget() const { assert(m_DefaultRenderTarget.IsValid()); return m_DefaultRenderTarget; }
	inline const Ref<ID3D11DepthStencilView>& DefaultDepthStencil() const { assert(m_DefaultDepthStencil.IsValid()); return m_DefaultDepthStencil; }

	void SetVertexBuffer(const ID3D11Buffer* pVertexBuffer, uint32_t stride, uint32_t offset, uint32_t index = 0U);
	void SetIndexBuffer(const ID3D11Buffer* pIndexBuffer, DXGI_FORMAT format, uint32_t offset = 0U);

	inline void SetVertexBuffer(const Ref<ID3D11Buffer>& vertexBuffer, uint32_t stride, uint32_t offset, uint32_t index = 0U)
	{
		SetVertexBuffer(vertexBuffer.GetPtr(), stride, offset, index);
	}
	inline void SetIndexBuffer(const Ref<ID3D11Buffer>& indexBuffer, DXGI_FORMAT format, uint32_t offset = 0U)
	{
		SetIndexBuffer(indexBuffer.GetPtr(), format, offset);
	}

	void SetInputLayout(const Ref<ID3D11InputLayout>& inputLayout);
	void SetRenderTarget(const Ref<ID3D11RenderTargetView>& renderTarget, uint32_t slot = 0U);
	void SetDepthStencil(const Ref<ID3D11DepthStencilView>& depthStencil);
	void SetViewports(D3D11_VIEWPORT* pViewports, uint32_t count = 1U);
	void SetRasterizerState(const Ref<ID3D11RasterizerState>& rasterizerState);
	void SetDepthStencilState(const Ref<ID3D11DepthStencilState>& depthStencilState);
	void SetBlendState(const Ref<ID3D11BlendState>& blendState, DirectX::XMFLOAT4, uint32_t mask);
	void SetVertexShader(const Ref<ID3D11VertexShader>& vertexShader);
	void SetPixelShader(const Ref<ID3D11PixelShader>& pixelShader);
	void SetConstantBuffer(Ref<ID3D11Buffer>& constantBuf, uint32_t slot, eShaderType shaderType);

	void UpdateConstantBuffer(Ref<ID3D11Buffer>& constantBuf, const void* pSource, uint32_t size);

	void Draw(uint32_t vertexCount, uint32_t startIndex, D3D_PRIMITIVE_TOPOLOGY prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, D3D_PRIMITIVE_TOPOLOGY prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	static std::string ResourceFilePath(const char* pFileName, eResourceType resType);
	static const char* ResourceFileDirectory(eResourceType resType);
protected:
	D3DGraphic();
	D3DGraphic(const D3DGraphic&) {}
	~D3DGraphic() {}

	void RecreateBackBuffer();

	void FlushPipelineState();

	void CreateBuffer(__out ID3D11Buffer** ppBuffer, D3D11_BIND_FLAG bindFlag, uint32_t byteWidth, D3D11_USAGE usage, const void* pBuf, uint32_t cpuAccessFlag);
	void CompileShaderFile(__out ID3DBlob** ppRes, char* pFileName, char* pEntryPoint, char* pTarget, const D3D_SHADER_MACRO* pDefines);

	friend class D3DModel;
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
		ID3D11Buffer* Buffers[eVertexStreamCount];
		uint32_t      Stride[eVertexStreamCount];
		uint32_t      Offset[eVertexStreamCount];
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
		ID3D11RenderTargetView*  RenderTarget[eRenderTargetCount];
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
	D3D11_VIEWPORT m_DefaultViewport;

	bool m_FlushState[eFSCount];
};
