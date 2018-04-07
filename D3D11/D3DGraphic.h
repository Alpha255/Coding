#pragma once

#include "D3DMath.h"

#include <d3d11.h>
#include <d3dcompiler.h>

class D3DGraphic
{
public:
	static void CreateInstance(void) { if (nullptr == m_sInstance) { m_sInstance = new D3DGraphic(); assert(m_sInstance); } }
	static D3DGraphic* GetInstance(void) { return m_sInstance; }
	static void DestoryInstance(void) { SafeDelete(m_sInstance); }

	inline const Ref<ID3D11RenderTargetView> &DefaultRenderTarget() const { assert(m_DefaultRenderTarget.Valid()); return m_DefaultRenderTarget; }
	inline const Ref<ID3D11DepthStencilView> &DefaultDepthStencil() const { assert(m_DefaultDepthStencil.Valid()); return m_DefaultDepthStencil; }

	void SetStreamOut(const Ref<ID3D11Buffer> &rBuf, uint32_t offset = 0U);

	inline void SetUnorderedAccessView(const Ref<ID3D11UnorderedAccessView> &rUAV, uint32_t startSlot = 0U)
	{
		ID3D11UnorderedAccessView *const ppUAV[1]{ rUAV.Ptr() };
		m_IMContent->CSSetUnorderedAccessViews(startSlot, 1U, ppUAV, nullptr);
	}

	inline void ExeCommandlist(uint32_t tx, uint32_t ty, uint32_t tz)
	{
		///FlushState();
		m_IMContent->Dispatch(tx, ty, tz);
	}

	///void GetBackBufferDesc(D3D11_TEXTURE2D_DESC& tex2DDesc);

	///void ResolveSubResource(ID3D11Texture2D* pDstResource, ID3D11Texture2D* pSrcResource, uint32_t dstCount, uint32_t srcCount, DXGI_FORMAT fmt);
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
