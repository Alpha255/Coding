#pragma once

#include "D3DMath.h"

#include <d3d11.h>
#include <d3dcompiler.h>

class D3DGraphic
{
public:
	enum eResourceType
	{
		eShader,
		eTexture,
		eSDKMesh,
		eTxtMesh,
		eObjMesh,
		eResTypeCount
	};

	enum eShaderType
	{
		eVertexShader,
		eHullShader,
		eDomainShader,
		eGeometryShader,
		ePixelShader,
		eComputeShader,
		eShaderTypeCount
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

	void CreateTexture2D(__out ID3D11Texture2D** ppTex, DXGI_FORMAT fmt, uint32_t width, uint32_t height,
		uint32_t bindFlags, uint32_t mipLevels = 1U, uint32_t arraySize = 1U, uint32_t cpuFlags = 0U, uint32_t sampleCount = 1U, uint32_t sampleQuality = 0U,
		uint32_t miscFlags = 0U, D3D11_USAGE usage = D3D11_USAGE_DEFAULT, const void* pData = nullptr, uint32_t pitch = 0U, uint32_t slice = 0U);

	void CreateShaderResourceView(__out ID3D11ShaderResourceView** ppSRV, const char* pFileName);
	void CreateShaderResourceView(__out ID3D11ShaderResourceView** ppSRV, ID3D11Texture2D* pTex, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, D3D11_SRV_DIMENSION dimension = D3D11_SRV_DIMENSION_TEXTURE2D);
	void CreateUnorderedAccessView(__out ID3D11UnorderedAccessView **ppUAV, ID3D11Resource *pRes, uint32_t numElems, uint32_t firstElem = 0U, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, D3D11_UAV_DIMENSION dimension = D3D11_UAV_DIMENSION_UNKNOWN, uint32_t bufFlag = 0U);
	void CreateRenderTargetView(__out ID3D11RenderTargetView** ppRTV, ID3D11Texture2D* pTex, const D3D11_RENDER_TARGET_VIEW_DESC *pRTVDesc = nullptr);
	void CreateDepthStencilView(__out ID3D11DepthStencilView** ppDSV, ID3D11Texture2D* pTex, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, uint32_t width = 0U, uint32_t height = 0U, D3D11_DSV_DIMENSION dimension = D3D11_DSV_DIMENSION_TEXTURE2D);
	
	inline void CreateUnorderedAccessView(
		__out ID3D11UnorderedAccessView **ppUAV, 
		__out ID3D11Buffer **ppBuf, 
		uint32_t byteWidth, 
		D3D11_USAGE usage, 
		uint32_t miscFlag,
		uint32_t byteStride, 
		uint32_t numElems, 
		uint32_t bindFlags = 0U,
		uint32_t firstElem = 0U, 
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, 
		D3D11_UAV_DIMENSION dimension = D3D11_UAV_DIMENSION_UNKNOWN, 
		uint32_t bufFlag = 0U)
	{
		CreateUnorderedAccessBuffer(ppBuf, byteWidth, usage, miscFlag, byteStride, bindFlags);
		CreateUnorderedAccessView(ppUAV, *ppBuf, numElems, firstElem, format, dimension, bufFlag);
	}

	inline void CreateVertexBuffer(__out ID3D11Buffer** ppBuf, uint32_t byteWidth, D3D11_USAGE usage, const void* pBuf, uint32_t cpuAccessFlag = 0U)
	{
		CreateBuffer(ppBuf, D3D11_BIND_VERTEX_BUFFER, byteWidth, usage, pBuf, cpuAccessFlag);
	}

	inline void CreateIndexBuffer(__out ID3D11Buffer** ppBuf, uint32_t byteWidth, D3D11_USAGE usage, const void* pBuf, uint32_t cpuAccessFlag = 0U)
	{
		CreateBuffer(ppBuf, D3D11_BIND_INDEX_BUFFER, byteWidth, usage, pBuf, cpuAccessFlag);
	}

	inline void CreateConstantBuffer(__out ID3D11Buffer** ppBuf, uint32_t byteWidth, D3D11_USAGE usage, const void* pBuf, uint32_t cpuAccessFlag = 0U)
	{
		CreateBuffer(ppBuf, D3D11_BIND_CONSTANT_BUFFER, byteWidth, usage, pBuf, cpuAccessFlag);
	}

	inline void CreateUnorderedAccessBuffer(__out ID3D11Buffer **ppBuf, uint32_t byteWidth, D3D11_USAGE usage, uint32_t miscFlag, 
		uint32_t byteStride, uint32_t bindFlags = 0U)
	{
		assert(byteStride);
		CreateBuffer(ppBuf, bindFlags | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, byteWidth, usage, nullptr, 0U, miscFlag, byteStride);
	}

	void CreateVertexShader(__out ID3D11VertexShader** ppVS, char* pFileName, char* pEntryPoint, const D3D_SHADER_MACRO* pDefines = nullptr, ID3DInclude* pInclude = nullptr);
	void CreatePixelShader(__out ID3D11PixelShader** ppPS, char* pFileName, char* pEntryPoint, const D3D_SHADER_MACRO* pDefines = nullptr, ID3DInclude* pInclude = nullptr);
	void CreateHullShader(__out ID3D11HullShader **ppHS, char* pFileName, char* pEntryPoint, const D3D_SHADER_MACRO* pDefines = nullptr, ID3DInclude* pInclude = nullptr);
	void CreateDomainShader(__out ID3D11DomainShader **ppDS, char* pFileName, char* pEntryPoint, const D3D_SHADER_MACRO* pDefines = nullptr, ID3DInclude* pInclude = nullptr);
	void CreateGeometryShader(__out ID3D11GeometryShader **ppGS, char *pFileName, char *pEntryPoint, const D3D_SHADER_MACRO *pDefines = nullptr, ID3DInclude *pInclude = nullptr);
	void CreateComputeShader(__out ID3D11ComputeShader **ppCS, char *pFileName, char *pEntryPoint, const D3D_SHADER_MACRO *pDefines = nullptr, ID3DInclude *pInclude = nullptr);
	void CreateVertexShaderAndInputLayout(__out ID3D11VertexShader** ppVS, __out ID3D11InputLayout** ppLayout, D3D11_INPUT_ELEMENT_DESC* pInputElement, uint32_t size, char* pFileName, char* pEntryPoint, const D3D_SHADER_MACRO *pDefines = nullptr, ID3DInclude* pInclude = nullptr);
	///void CreateShader(eShaderType shaderType, struct D3DShaders &targetShaders, char *pFileName, char *pEntryPoint, const D3D_SHADER_MACRO *pDefines, ID3DInclude *pInclude);

	void CreateInputLayout(__out ID3D11InputLayout** ppLayout, D3D11_INPUT_ELEMENT_DESC* pInputElement, uint32_t size, ID3DBlob* pRes);

	void CreateBlendState(__out ID3D11BlendState** ppBlendState, D3D11_BLEND_DESC* pBlendDesc);
	void CreateSamplerState(__out ID3D11SamplerState** ppSamplerState, const D3D11_SAMPLER_DESC* pSamplerDesc);
	void CreateRasterizerState(__out ID3D11RasterizerState** ppRasterizerState, D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode = D3D11_CULL_BACK, bool cw = false, bool depthClip = true, bool bScissor = false);
	void CreateDepthStencilState(__out ID3D11DepthStencilState** ppDepthStencilState, const D3D11_DEPTH_STENCIL_DESC* pDepthStencilStateDesc);

	void ClearRenderTarget(ID3D11RenderTargetView* pRenderTarget, const float* pClearColor = nullptr);
	void ClearDepthStencil(ID3D11DepthStencilView* pDepthStencil, uint32_t clearFlags, float depth, uint8_t stencil);

	void ResizeBackBuffer(uint32_t width, uint32_t height);

	inline ID3D11RenderTargetView* DefaultRenderTarget() const { assert(m_DefaultRenderTarget.Valid()); return m_DefaultRenderTarget.Ptr(); }
	inline ID3D11DepthStencilView* DefaultDepthStencil() const { assert(m_DefaultDepthStencil.Valid()); return m_DefaultDepthStencil.Ptr(); }

	void SetVertexBuffer(const ID3D11Buffer* pVertexBuffer, uint32_t stride, uint32_t offset, uint32_t index = 0U);
	void SetIndexBuffer(const ID3D11Buffer* pIndexBuffer, DXGI_FORMAT format, uint32_t offset = 0U);
	void SetInputLayout(ID3D11InputLayout* pInputLayout);
	void SetRenderTarget(ID3D11RenderTargetView* pRenderTarget, uint32_t slot = 0U);
	void SetDepthStencil(ID3D11DepthStencilView* pDepthStencil);
	void SetViewports(D3D11_VIEWPORT* pViewports, uint32_t count = 1U);
	void SetScissorRects(D3D11_RECT* pRects, uint32_t count = 1U);
	void SetSamplerStates(ID3D11SamplerState * pStates, uint32_t startSlot = 0U, uint32_t count = 1U, eShaderType type = ePixelShader);
	void SetShaderResource(ID3D11ShaderResourceView *pSRV, uint32_t startSlot = 0U, uint32_t count = 1U, eShaderType type = ePixelShader);
	void SetRasterizerState(ID3D11RasterizerState* pRasterizerState);
	void SetDepthStencilState(ID3D11DepthStencilState* pDepthStencilState, uint32_t stencilRef);
	void SetBlendState(ID3D11BlendState* pBlendState, Vec4 blendFactor, uint32_t mask);
	void SetVertexShader(ID3D11VertexShader* pVS);
	void SetHullShader(ID3D11HullShader *pHS);
	void SetDomainShader(ID3D11DomainShader *pDS);
	void SetPixelShader(ID3D11PixelShader* pPS);
	void SetGeometryShader(ID3D11GeometryShader *pGS);
	void SetComputeShader(ID3D11ComputeShader *pCS);
	void SetConstantBuffer(ID3D11Buffer* pConstantBuf, uint32_t slot, eShaderType shaderType);

	void GetBackBufferDesc(D3D11_TEXTURE2D_DESC& tex2DDesc);

	void ResolveSubResource(ID3D11Texture2D* pDstResource, ID3D11Texture2D* pSrcResource, uint32_t dstCount, uint32_t srcCount, DXGI_FORMAT fmt);

	void UpdateBuffer(ID3D11Buffer* pBuffer, const void* pSource, size_t size);

	void Draw(uint32_t vertexCount, uint32_t startIndex, D3D_PRIMITIVE_TOPOLOGY prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, D3D_PRIMITIVE_TOPOLOGY prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void DrawQuad(float x, float y, float width, float height);

	inline ID3D11Device *GetDevice() const
	{
		assert(m_D3DDevice.Valid());
		return m_D3DDevice.Ptr();
	}
	inline ID3D11DeviceContext *GetIMContext() const
	{
		assert(m_D3DContext.Valid());
		return m_D3DContext.Ptr();
	}

	inline void Flip()
	{
		m_SwapChain->Present(0U, 0U);
	}

	void FlushState();

	static std::string ResourceFilePath(const char* pFileName, eResourceType resType);
	static const char* ResourceFileDirectory(eResourceType resType);
protected:
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
		eFSHullShader,
		eFSDomainShader,
		eFSGeometryShader,
		eFSComputeShader,
		eFSInputLayout,
		eFSRenderTarget,
		eFSDepthStencil,
		eFSViewports,
		eFSScissorRect,
		eFSRasterizerState,
		eFSBlendState,
		eFSDepthStencilState,
		eFSSamplerState,
		eFSPrimitiveTopology,
		eFSCount
	};

	D3DGraphic();
	D3DGraphic(const D3DGraphic&) {}
	~D3DGraphic() {}

	void RecreateBackBuffer();

	void CreateBuffer(__out ID3D11Buffer** ppBuffer, uint32_t bindFlag, uint32_t byteWidth, D3D11_USAGE usage, const void* pBuf, uint32_t cpuAccessFlag, uint32_t miscFlag = 0U, uint32_t byteStride = 0U);
	void CompileShaderFile(__out ID3DBlob** ppRes, char* pFileName, char* pEntryPoint, char* pTarget, const D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude);
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

	///struct D3DShaders
	///{
	///	ID3D11VertexShader**      VertexShader = nullptr;
	///	ID3D11HullShader**        HullShader = nullptr;
	///	ID3D11DomainShader**      DomainShader = nullptr;
	///	ID3D11PixelShader**       PixelShader = nullptr;
	///	ID3D11GeometryShader**    GeometryShader = nullptr;
	///	ID3D11ComputeShader**     ComputeShader = nullptr;
	///};

	struct D3DPipelineState
	{
		ID3D11InputLayout*       InputLayout;
		D3DVertexBuffer          VertexBuffer;
		D3DIndexBuffer           IndexBuffer;
		D3D_PRIMITIVE_TOPOLOGY   PrimitiveTopology;
		ID3D11VertexShader*      VertexShader;
		ID3D11HullShader*        HullShader;
		ID3D11DomainShader*      DomainShader;
		ID3D11RasterizerState*   RasterizerState;
		D3D11_VIEWPORT*          Viewports;
		D3D11_RECT*              ScissorRects;
		ID3D11PixelShader*       PixelShader;
		ID3D11GeometryShader*    GeometryShader;
		ID3D11ComputeShader*     ComputeShader;
		ID3D11BlendState*        BlendState;
		ID3D11DepthStencilState* DepthStencilState;
		ID3D11RenderTargetView*  RenderTarget[eRenderTargetCount];
		ID3D11DepthStencilView*  DepthStencil;

		float                    BlendFactor[4];
		uint32_t                 SampleMask;
		uint32_t                 ViewportCount;
		uint32_t                 ScissorRectCount;
		uint32_t                 StencilRef;
	}m_D3DPipelineState;

	DXGI_SWAP_CHAIN_DESC m_SwapChainDesc;

	Ref<IDXGISwapChain> m_SwapChain;
	Ref<ID3D11Device> m_D3DDevice;
	Ref<ID3D11DeviceContext> m_D3DContext;
	Ref<ID3D11RenderTargetView> m_DefaultRenderTarget;
	Ref<ID3D11DepthStencilView> m_DefaultDepthStencil;
	D3D11_VIEWPORT m_DefaultViewport;
	D3D11_RECT m_DefaultScissorRect;

	bool m_FlushState[eFSCount];
};
