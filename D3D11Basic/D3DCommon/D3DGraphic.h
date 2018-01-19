#pragma once

#include "D3DMath.h"

#include <d3d11.h>
#include <d3dcompiler.h>

class D3DGraphic
{
public:
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

	static void CreateInstance(void) { if (nullptr == m_sInstance) { m_sInstance = new D3DGraphic(); assert(m_sInstance); } }
	static D3DGraphic* GetInstance(void) { return m_sInstance; }
	static void DestoryInstance(void) { SafeDelete(m_sInstance); }

	void InitD3DEnvironment(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed);

	void CreateTexture1D(__out Ref<ID3D11Texture1D> &rTex,
		DXGI_FORMAT fmt,
		uint32_t width,
		uint32_t bindFlags,
		uint32_t mipLevels = 1U,
		uint32_t arraySize = 1U,
		uint32_t cpuFlags = 0U,
		uint32_t miscFlags = 0U,
		D3D11_USAGE usage = D3D11_USAGE_DEFAULT,
		const void *pData = nullptr,
		uint32_t memPitch = 0U);

	void CreateTexture2D(__out Ref<ID3D11Texture2D> &rTex, 
		DXGI_FORMAT fmt, 
		uint32_t width, 
		uint32_t height, 
		uint32_t bindFlags, 
		uint32_t mipLevels = 1U, 
		uint32_t arraySize = 1U, 
		uint32_t cpuFlags = 0U, 
		uint32_t miscFlags = 0U, 
		D3D11_USAGE usage = D3D11_USAGE_DEFAULT, 
		const void *pData = nullptr,
		uint32_t memPitch = 0U);

	void CreateShaderResourceView(__out Ref<ID3D11ShaderResourceView> &rSRV, const char *pFileName);

	void CreateShaderResourceView(__out Ref<ID3D11ShaderResourceView> &rSRV, 
		ID3D11Resource *pSrc, 
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, 
		D3D11_SRV_DIMENSION dimension = D3D11_SRV_DIMENSION_TEXTURE2D, 
		uint32_t firstElem = 0U, 
		uint32_t numElems = 0U);

	void CreateUnorderedAccessView(__out Ref<ID3D11UnorderedAccessView> &rUAV, 
		ID3D11Resource *pSrc, 
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, 
		D3D11_UAV_DIMENSION dimension = D3D11_UAV_DIMENSION_UNKNOWN, 
		uint32_t firstElem = 0U, 
		uint32_t numElems = 0U, 
		uint32_t flags = 0U);

	void CreateRenderTargetView(__out Ref<ID3D11RenderTargetView> &rRTV, 
		ID3D11Resource *pSrc, 
		const D3D11_RENDER_TARGET_VIEW_DESC *pRTVDesc);

	void CreateDepthStencilView(__out Ref<ID3D11DepthStencilView> &rDSV, 
		ID3D11Resource* pSrc,
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, 
		uint32_t width = 0U, 
		uint32_t height = 0U,
		D3D11_DSV_DIMENSION dimension = D3D11_DSV_DIMENSION_TEXTURE2D);

	inline void CreateVertexBuffer(__out Ref<ID3D11Buffer> &rBuf, 
		uint32_t byteWidth, 
		D3D11_USAGE usage, 
		const void *pData, 
		uint32_t cpuAccessFlags = 0U, 
		uint32_t bindFlags = 0U)
	{
		CreateBuffer(rBuf, D3D11_BIND_VERTEX_BUFFER | bindFlags, byteWidth, usage, pData, cpuAccessFlags, 0U, 0U, 0U);
	}

	inline void CreateIndexBuffer(__out Ref<ID3D11Buffer> &rBuf, 
		uint32_t byteWidth, 
		D3D11_USAGE usage, 
		const void *pData, 
		uint32_t cpuAccessFlags = 0U)
	{
		CreateBuffer(rBuf, D3D11_BIND_INDEX_BUFFER, byteWidth, usage, pData, cpuAccessFlags, 0U, 0U, 0U);
	}

	inline void CreateConstantBuffer(__out Ref<ID3D11Buffer> &rBuf, 
		uint32_t byteWidth, 
		D3D11_USAGE usage, 
		const void *pData, 
		uint32_t cpuAccessFlags = 0U)
	{
		CreateBuffer(rBuf, D3D11_BIND_CONSTANT_BUFFER, byteWidth, usage, pData, cpuAccessFlags, 0U, 0U, 0U);
	}

	inline void CreateUnorderedAccessBuffer(__out Ref<ID3D11Buffer> &rBuf, 
		uint32_t byteWidth, 
		D3D11_USAGE usage, 
		uint32_t miscFlags, 
		uint32_t byteStride, 
		uint32_t bindFlags = 0U)
	{
		assert(byteStride);
		CreateBuffer(rBuf, bindFlags | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, byteWidth, usage, nullptr, 0U, miscFlags, byteStride, 0U);
	}

	inline void CreateReadOnlyBuffer(__out Ref<ID3D11Buffer> &rBuf, 
		uint32_t byteWidth, 
		const void *pData, 
		uint32_t cpuAccessFlags = 0U)
	{
		CreateBuffer(rBuf, 0U, byteWidth, D3D11_USAGE_STAGING, pData, cpuAccessFlags, 0U, 0U, 0U);
	}

	inline void CreateVertexShader(__out Ref<ID3D11VertexShader> &rVS,
		const char *pFileName,
		const char *pEntryPoint,
		const D3D_SHADER_MACRO *pMacros = nullptr,
		ID3DInclude *pInclude = nullptr)
	{
		const D3DShaders &&shaders = CreateShader(eVertexShader, pFileName, pEntryPoint, pMacros, pInclude);
		rVS = shaders.VertexShader;
	}

	inline void CreatePixelShader(__out Ref<ID3D11PixelShader> &rPS,
		const char *pFileName,
		const char *pEntryPoint,
		const D3D_SHADER_MACRO *pMacros = nullptr,
		ID3DInclude *pInclude = nullptr)
	{
		const D3DShaders &&shaders = CreateShader(ePixelShader, pFileName, pEntryPoint, pMacros, pInclude);
		rPS = shaders.PixelShader;
	}

	inline void CreateHullShader(__out Ref<ID3D11HullShader> &rHS,
		const char *pFileName,
		const char *pEntryPoint,
		const D3D_SHADER_MACRO *pMacros = nullptr,
		ID3DInclude *pInclude = nullptr)
	{
		const D3DShaders &&shaders = CreateShader(eHullShader, pFileName, pEntryPoint, pMacros, pInclude);
		rHS = shaders.HullShader;
	}

	inline void CreateDomainShader(__out Ref<ID3D11DomainShader> &rDS,
		const char *pFileName,
		const char *pEntryPoint,
		const D3D_SHADER_MACRO *pMacros = nullptr,
		ID3DInclude *pInclude = nullptr)
	{
		const D3DShaders &&shaders = CreateShader(eDomainShader, pFileName, pEntryPoint, pMacros, pInclude);
		rDS = shaders.DomainShader;
	}

	inline void CreateGeometryShader(__out Ref<ID3D11GeometryShader> &rGS,
		const char *pFileName,
		const char *pEntryPoint,
		const D3D_SHADER_MACRO *pMacros = nullptr,
		ID3DInclude *pInclude = nullptr)
	{
		const D3DShaders &&shaders = CreateShader(eGeometryShader, pFileName, pEntryPoint, pMacros, pInclude);
		rGS = shaders.GeometryShader;
	}

	inline void CreateComputeShader(__out Ref<ID3D11ComputeShader> &rCS,
		const char *pFileName,
		const char *pEntryPoint,
		const D3D_SHADER_MACRO *pMacros = nullptr,
		ID3DInclude *pInclude = nullptr)
	{
		const D3DShaders &&shaders = CreateShader(eComputeShader, pFileName, pEntryPoint, pMacros, pInclude);
		rCS = shaders.ComputeShader;
	}

	inline void CreateInputLayout(__out Ref<ID3D11InputLayout> &rLayout,
		const D3D11_INPUT_ELEMENT_DESC *pInputElement,
		uint32_t size,
		ID3DBlob *pBlob)
	{
		assert(!rLayout.Valid() && pInputElement && (size > 0U) && pBlob);

		HRCheck(m_D3DDevice->CreateInputLayout(pInputElement, size, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), rLayout.Reference()));
	}

	inline void CreateVertexShaderAndInputLayout(
		__out Ref<ID3D11VertexShader> &rVS, 
		__out Ref<ID3D11InputLayout> &rLayout, 
		const D3D11_INPUT_ELEMENT_DESC *pInputElement, 
		uint32_t size, 
		const char *pFileName, 
		const char *pEntryPoint, 
		const D3D_SHADER_MACRO *pMacros = nullptr, 
		ID3DInclude *pInclude = nullptr)
	{
		assert(!rVS.Valid() && !rLayout.Valid());

		Ref<ID3DBlob> blob;
		CompileShaderFile(blob, pFileName, pEntryPoint, "vs_5_0", pMacros, pInclude);
		HRCheck(m_D3DDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, rVS.Reference()));
		CreateInputLayout(rLayout, pInputElement, size, blob.Ptr());
	}

	inline void CreateBlendState(__out Ref<ID3D11BlendState> &rBlendState, 
		const D3D11_BLEND_DESC *pBlendDesc)
	{
		assert(pBlendDesc && !rBlendState.Valid());

		HRCheck(m_D3DDevice->CreateBlendState(pBlendDesc, rBlendState.Reference()));
	}

	inline void CreateSamplerState(__out Ref<ID3D11SamplerState> &rSamplerState, 
		const D3D11_SAMPLER_DESC *pSamplerDesc)
	{
		assert(pSamplerDesc && !rSamplerState.Valid());

		HRCheck(m_D3DDevice->CreateSamplerState(pSamplerDesc, rSamplerState.Reference()));
	}

	inline void CreateDepthStencilState(__out Ref<ID3D11DepthStencilState> &rDepthStencilState,
		const D3D11_DEPTH_STENCIL_DESC *pDepthStencilStateDesc)
	{
		assert(pDepthStencilStateDesc && !rDepthStencilState.Valid());

		HRCheck(m_D3DDevice->CreateDepthStencilState(pDepthStencilStateDesc, rDepthStencilState.Reference()));
	}

	void CreateRasterizerState(__out Ref<ID3D11RasterizerState> &rRasterizerState, 
		D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID,
		D3D11_CULL_MODE cullMode = D3D11_CULL_BACK, 
		bool cw = false, 
		bool depthClip = true, 
		bool bScissor = false);

	void CreateRandomTexture1D(__out Ref<ID3D11ShaderResourceView> &rSRV);

	void CreateSamplerState(__out Ref<ID3D11SamplerState> &rSamplerState, 
		D3D11_FILTER filter, 
		D3D11_TEXTURE_ADDRESS_MODE addressMode,
		float LODBias = 0.0f,
		D3D11_COMPARISON_FUNC = D3D11_COMPARISON_NEVER,
		const Vec4 &borderClr = { 0.0f, 0.0f, 0.0f, 0.0f },
		float minLOD = 0.0f,
		float maxLOD = D3D11_FLOAT32_MAX);

	void CreateBlendState(__out Ref<ID3D11BlendState> &rBlendState,
		bool bBlend,
		bool bAlphaToCoverage,
		D3D11_BLEND srcColor,
		D3D11_BLEND dstColor,
		D3D11_BLEND_OP colorOp,
		D3D11_BLEND srcAlpha, 
		D3D11_BLEND dstAlpha,
		D3D11_BLEND_OP alphaOp,
		D3D11_COLOR_WRITE_ENABLE writeMask);


	inline void ClearRenderTarget(const Ref<ID3D11RenderTargetView> &rRenderTarget, const float *pClearColor = nullptr)
	{
		assert(rRenderTarget.Valid());

		pClearColor = (nullptr == pClearColor ? reinterpret_cast<const float*>(&Color::DarkBlue) : pClearColor);

		m_IMContent->ClearRenderTargetView(rRenderTarget.Ptr(), pClearColor);
	}

	inline void ClearDepthStencil(const Ref<ID3D11DepthStencilView> &rDepthStencil, uint32_t clearFlags, float depth, uint8_t stencil)
	{
		assert(rDepthStencil.Valid());

		m_IMContent->ClearDepthStencilView(rDepthStencil.Ptr(), clearFlags, depth, stencil);
	}

	inline const Ref<ID3D11RenderTargetView> &DefaultRenderTarget() const { assert(m_DefaultRenderTarget.Valid()); return m_DefaultRenderTarget; }
	inline const Ref<ID3D11DepthStencilView> &DefaultDepthStencil() const { assert(m_DefaultDepthStencil.Valid()); return m_DefaultDepthStencil; }

	void ResizeBackBuffer(uint32_t width, uint32_t height);

	void SetRenderTarget(const Ref<ID3D11RenderTargetView> &rRenderTarget, uint32_t slot = 0U);
	void SetDepthStencil(const Ref<ID3D11DepthStencilView> &rDepthStencilView);

	void SetInputLayout(const Ref<ID3D11InputLayout> &rInputLayout);
	void SetVertexBuffer(const Ref<ID3D11Buffer> &rVertexBuffer, uint32_t stride, uint32_t offset, uint32_t index = 0U);
	void SetIndexBuffer(const Ref<ID3D11Buffer> &rIndexBuffer, DXGI_FORMAT format, uint32_t offset = 0U);

	void SetViewports(const D3D11_VIEWPORT *pViewports, uint32_t count = 1U);
	void SetScissorRects(const D3D11_RECT *pRects, uint32_t count = 1U);

	void SetSamplerStates(const Ref<ID3D11SamplerState> &rSamplerStates, uint32_t startSlot, eShaderType shaderType);
	void SetRasterizerState(const Ref<ID3D11RasterizerState> &rRasterizerState);
	void SetDepthStencilState(const Ref<ID3D11DepthStencilState> &rDepthStencilState, uint32_t stencilRef);
	void SetBlendState(const Ref<ID3D11BlendState> &rBlendState, Vec4 blendFactor, uint32_t mask);

	void SetVertexShader(const Ref<ID3D11VertexShader> &rVS);
	void SetHullShader(const Ref<ID3D11HullShader> &rHS);
	void SetDomainShader(const Ref<ID3D11DomainShader> &rDS);
	void SetPixelShader(const Ref<ID3D11PixelShader> &rPS);
	void SetGeometryShader(const Ref<ID3D11GeometryShader> &rGS);
	void SetComputeShader(const Ref<ID3D11ComputeShader> &rCS);
	
	void SetConstantBuffer(const Ref<ID3D11Buffer> &rConstantBuf, uint32_t slot, eShaderType shaderType);
	
	void SetShaderResource(const Ref<ID3D11ShaderResourceView> &rSRV, uint32_t startSlot, eShaderType);

	void SetStreamOut(const Ref<ID3D11Buffer> &rBuf, uint32_t offset = 0U);

	inline void SetUnorderedAccessView(const Ref<ID3D11UnorderedAccessView> &rUAV, uint32_t startSlot = 0U)
	{
		ID3D11UnorderedAccessView *const ppUAV[1]{ rUAV.Ptr() };
		m_IMContent->CSSetUnorderedAccessViews(startSlot, 1U, ppUAV, nullptr);
	}

	inline void ExeCommandlist(uint32_t tx, uint32_t ty, uint32_t tz)
	{
		FlushState();
		m_IMContent->Dispatch(tx, ty, tz);
	}

	///void GetBackBufferDesc(D3D11_TEXTURE2D_DESC& tex2DDesc);

	///void ResolveSubResource(ID3D11Texture2D* pDstResource, ID3D11Texture2D* pSrcResource, uint32_t dstCount, uint32_t srcCount, DXGI_FORMAT fmt);

	void UpdateBuffer(Ref<ID3D11Buffer> &rBuffer, const void *pSource, size_t size);
	inline void UpdateBuffer(ID3D11Resource *pRes, uint32_t dstSubRes, const ::RECT *pRect, const void *pSrcData, uint32_t srcRowPitch, uint32_t srcDepthPitch)
	{
		D3D11_BOX box = { 0U };
		if (pRect)
		{
			box.left = (uint32_t)pRect->left;
			box.right = (uint32_t)pRect->right;
			box.top = pRect->top;
			box.bottom = pRect->bottom;
			box.front = 0U;
			box.back = 1U;
		}

		m_IMContent->UpdateSubresource(pRes, dstSubRes, pRect ? &box : nullptr, pSrcData, srcRowPitch, srcDepthPitch);
	}

	void CopyBuffer(ID3D11Resource *pSrc, ID3D11Resource *pDst, void *pDstMem, size_t memSize, ::RECT &rect);

	void Draw(uint32_t vertexCount, uint32_t startIndex, D3D_PRIMITIVE_TOPOLOGY prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, D3D_PRIMITIVE_TOPOLOGY prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void DrawQuad(float x, float y, float width, float height);
	void DrawAuto(D3D_PRIMITIVE_TOPOLOGY prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	inline ID3D11Device *GetDevice() const
	{
		assert(m_D3DDevice.Valid());
		return m_D3DDevice.Ptr();
	}
	inline ID3D11DeviceContext *GetIMContext() const
	{
		assert(m_IMContent.Valid());
		return m_IMContent.Ptr();
	}

	inline void Flip()
	{
		m_SwapChain->Present(0U, 0U);
	}

	void FlushState();
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
		eFSStreamOut,
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

	struct D3DShaders
	{
		Ref<ID3D11VertexShader> VertexShader;
		Ref<ID3D11HullShader> HullShader;
		Ref<ID3D11DomainShader> DomainShader;
		Ref<ID3D11PixelShader> PixelShader;
		Ref<ID3D11GeometryShader> GeometryShader;
		Ref<ID3D11ComputeShader> ComputeShader;
	};

	D3DGraphic() = default;
	D3DGraphic(const D3DGraphic &) = delete;
	void operator=(const D3DGraphic &) = delete;
	~D3DGraphic() = default;

	void RecreateBackBuffer();

	void CreateBuffer(__out Ref<ID3D11Buffer> &rBuf, 
		uint32_t bindFlags, 
		uint32_t byteWidth, 
		D3D11_USAGE usage, 
		const void *pData, 
		uint32_t cpuAccessFlags, 
		uint32_t miscFlags, 
		uint32_t byteStride, 
		uint32_t memPitch);

	void CompileShaderFile(__out Ref<ID3DBlob> &rBlob, 
		const char *pFileName, 
		const char *pEntryPoint, 
		const char *pTarget, 
		const D3D_SHADER_MACRO* pDefines, 
		ID3DInclude* pInclude);

	D3DShaders CreateShader(
		eShaderType shaderType,
		const char *pFileName,
		const char *pEntryPoint,
		const D3D_SHADER_MACRO *pMacros,
		ID3DInclude *pInclude);
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
		ID3D11HullShader*        HullShader;
		ID3D11DomainShader*      DomainShader;
		ID3D11RasterizerState*   RasterizerState;
		const D3D11_RECT*        ScissorRects;
		ID3D11PixelShader*       PixelShader;
		ID3D11GeometryShader*    GeometryShader;
		ID3D11ComputeShader*     ComputeShader;
		ID3D11Buffer*            StreamOut[1];
		ID3D11BlendState*        BlendState;
		ID3D11DepthStencilState* DepthStencilState;
		ID3D11RenderTargetView*  RenderTarget[eRenderTargetCount];
		ID3D11DepthStencilView*  DepthStencil;

		float                    BlendFactor[4];
		uint32_t                 SampleMask;
		uint32_t                 ViewportCount;
		uint32_t                 ScissorRectCount;
		uint32_t                 StencilRef;
		uint32_t                 StreamOutOffset[1];
	}m_PipelineState = { 0 };

	DXGI_SWAP_CHAIN_DESC m_SwapChainDesc = { 0 };

	Ref<IDXGISwapChain> m_SwapChain;
	Ref<ID3D11Device> m_D3DDevice;
	Ref<ID3D11DeviceContext> m_IMContent;
	Ref<ID3D11RenderTargetView> m_DefaultRenderTarget;
	Ref<ID3D11DepthStencilView> m_DefaultDepthStencil;
	D3D11_VIEWPORT m_DefaultViewport = { 0 };
	D3D11_RECT m_DefaultScissorRect = { 0 };

	bool m_FlushState[eFSCount] = { 0 };
};
