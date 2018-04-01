#include "D3DGraphic.h"
#include "Utility.h"

#include <fstream>
#include <string>
#include <DirectXTK/Inc/DDSTextureLoader.h>

D3DGraphic* D3DGraphic::m_sInstance = nullptr;

void D3DGraphic::SetVertexBuffer(const Ref<ID3D11Buffer> &rVertexBuffer, uint32_t stride, uint32_t offset, uint32_t index)
{
	assert(index <= eVertexStreamCount);

	if (m_PipelineState.VertexBuffer.Buffers[index] != rVertexBuffer.Ptr() ||
		m_PipelineState.VertexBuffer.Stride[index] != stride ||
		m_PipelineState.VertexBuffer.Offset[index] != offset)
	{
		m_PipelineState.VertexBuffer.Buffers[index] = rVertexBuffer.Ptr();
		m_PipelineState.VertexBuffer.Stride[index] = stride;
		m_PipelineState.VertexBuffer.Offset[index] = offset;

		m_FlushState[eFSVertexBuffer] = true;
	}
}

void D3DGraphic::SetIndexBuffer(const Ref<ID3D11Buffer> &rIndexBuffer, DXGI_FORMAT format, uint32_t offset)
{
	if (m_PipelineState.IndexBuffer.Buffers != rIndexBuffer.Ptr() ||
		m_PipelineState.IndexBuffer.Format != format ||
		m_PipelineState.IndexBuffer.Offset != offset)
	{
		m_PipelineState.IndexBuffer.Buffers = rIndexBuffer.Ptr();
		m_PipelineState.IndexBuffer.Format = format;
		m_PipelineState.IndexBuffer.Offset = offset;

		m_FlushState[eFSIndexBuffer] = true;
	}
}

void D3DGraphic::SetInputLayout(const Ref<ID3D11InputLayout> &rInputLayout)
{
	ID3D11InputLayout *const pInputLayout = rInputLayout.Ptr();
	if (m_PipelineState.InputLayout != pInputLayout)
	{
		m_PipelineState.InputLayout = pInputLayout;

		m_FlushState[eFSInputLayout] = true;
	}
}

void D3DGraphic::SetRenderTarget(const Ref<ID3D11RenderTargetView> &rRenderTarget, uint32_t slot)
{
	assert(slot <= eRenderTargetCount);

	ID3D11RenderTargetView *const pRenderTarget = rRenderTarget.Ptr();
	if (m_PipelineState.RenderTarget[slot] != pRenderTarget)
	{
		m_PipelineState.RenderTarget[slot] = pRenderTarget;

		m_FlushState[eFSRenderTarget] = true;
	}
}

void D3DGraphic::SetDepthStencil(const Ref<ID3D11DepthStencilView> &rDepthStencilView)
{
	ID3D11DepthStencilView *const pDepthStencil = rDepthStencilView.Ptr();
	if (m_PipelineState.DepthStencil != pDepthStencil)
	{
		m_PipelineState.DepthStencil = pDepthStencil;

		m_FlushState[eFSDepthStencil] = true;
	}
}

void D3DGraphic::SetViewports(const D3D11_VIEWPORT *pViewports, uint32_t count)
{
	assert(count <= 1U && "Unsupport yet!!!");

	if (m_PipelineState.ViewportCount != count)
	{
		m_PipelineState.ViewportCount = count;
	}

	memcpy(&m_DefaultViewport, pViewports, sizeof(D3D11_VIEWPORT));

	m_FlushState[eFSViewports] = true;
}

void D3DGraphic::SetScissorRects(const D3D11_RECT *pRects, uint32_t count)
{
	assert(count <= 1U && "Unsupport yet!!!");

	if (nullptr == m_PipelineState.ScissorRects)
	{
		///m_PipelineState.ScissorRects = new D3D11_RECT();
		///assert(0);
	}

	if (m_PipelineState.ScissorRectCount != count ||
		m_PipelineState.ScissorRects != pRects)
	{
		m_PipelineState.ScissorRectCount = count;

		memcpy(&m_DefaultScissorRect, pRects, sizeof(D3D11_RECT));

		m_FlushState[eFSScissorRect] = true;
	}
}

void D3DGraphic::SetSamplerStates(const Ref<ID3D11SamplerState> &rSamplerStates, uint32_t startSlot, eShaderType shaderType)
{
	ID3D11SamplerState *samplers[1]{ rSamplerStates.Ptr() };
	switch (shaderType)
	{
	case eVertexShader:
		m_IMContent->VSSetSamplers(startSlot, 1U, samplers);
		break;
	case eHullShader:
		m_IMContent->HSSetSamplers(startSlot, 1U, samplers);
		break;
	case eDomainShader:
		m_IMContent->DSSetSamplers(startSlot, 1U, samplers);
		break;
	case eGeometryShader:
		m_IMContent->GSSetSamplers(startSlot, 1U, samplers);
		break;
	case ePixelShader:
		m_IMContent->PSSetSamplers(startSlot, 1U, samplers);
		break;
	case eComputeShader:
		m_IMContent->CSSetSamplers(startSlot, 1U, samplers);
		break;
	}
}

void D3DGraphic::SetShaderResource(const Ref<ID3D11ShaderResourceView> &rSRV, uint32_t startSlot, eShaderType type)
{
	ID3D11ShaderResourceView *const ppSRV[1]{ rSRV.Ptr() };
	switch (type)
	{
	case eVertexShader:
		m_IMContent->VSSetShaderResources(startSlot, 1U, ppSRV);
		break;
	case eHullShader:
		m_IMContent->HSSetShaderResources(startSlot, 1U, ppSRV);
		break;
	case eDomainShader:
		m_IMContent->DSSetShaderResources(startSlot, 1U, ppSRV);
		break;
	case eGeometryShader:
		m_IMContent->GSSetShaderResources(startSlot, 1U, ppSRV);
		break;
	case ePixelShader:
		m_IMContent->PSSetShaderResources(startSlot, 1U, ppSRV);
		break;
	case eComputeShader:
		m_IMContent->CSSetShaderResources(startSlot, 1U, ppSRV);
		break;
	}
}

void D3DGraphic::SetStreamOut(const Ref<ID3D11Buffer> &rBuf, uint32_t offset)
{
	if (rBuf.Ptr() != m_PipelineState.StreamOut[0] || offset != m_PipelineState.StreamOutOffset[0])
	{
		m_PipelineState.StreamOut[0] = rBuf.Ptr();
		m_PipelineState.StreamOutOffset[0] = offset;

		m_FlushState[eFSStreamOut] = true;
	}
}

void D3DGraphic::SetRasterizerState(const Ref<ID3D11RasterizerState> &rRasterizerState)
{
	ID3D11RasterizerState *const pRasterizerState = rRasterizerState.Ptr();
	if (m_PipelineState.RasterizerState != pRasterizerState)
	{
		m_PipelineState.RasterizerState = pRasterizerState;

		m_FlushState[eFSRasterizerState] = true;
	}
}

void D3DGraphic::SetDepthStencilState(const Ref<ID3D11DepthStencilState> &rDepthStencilState, uint32_t stencilRef)
{
	ID3D11DepthStencilState *const pDepthStencilState = rDepthStencilState.Ptr();
	if (m_PipelineState.DepthStencilState != pDepthStencilState || m_PipelineState.StencilRef != stencilRef)
	{
		m_PipelineState.DepthStencilState = pDepthStencilState;
		m_PipelineState.StencilRef = stencilRef;

		m_FlushState[eFSDepthStencilState] = true;
	}
}

void D3DGraphic::SetBlendState(const Ref<ID3D11BlendState> &rBlendState, Vec4 blendFactor, uint32_t mask)
{
	ID3D11BlendState *const pBlendState = rBlendState.Ptr();
	if (m_PipelineState.BlendState != pBlendState ||
		m_PipelineState.SampleMask != mask)
	{
		m_PipelineState.BlendState = pBlendState;
		memcpy(m_PipelineState.BlendFactor, &blendFactor, sizeof(Vec4));
		m_PipelineState.SampleMask = mask;

		m_FlushState[eFSBlendState] = true;
	}
}

void D3DGraphic::SetVertexShader(const Ref<ID3D11VertexShader> &rVS)
{
	assert(rVS.Valid());

	ID3D11VertexShader *const pVertexShader = rVS.Ptr();
	if (m_PipelineState.VertexShader != pVertexShader)
	{
		m_PipelineState.VertexShader = pVertexShader;

		m_FlushState[eFSVertexShader] = true;
	}
}

void D3DGraphic::SetHullShader(const Ref<ID3D11HullShader> &rHS)
{
	ID3D11HullShader *const pHullShader = rHS.Ptr();
	if (m_PipelineState.HullShader != pHullShader)
	{
		m_PipelineState.HullShader = pHullShader;

		m_FlushState[eFSHullShader] = true;
	}
}

void D3DGraphic::SetDomainShader(const Ref<ID3D11DomainShader> &rDS)
{
	ID3D11DomainShader *const pDomainShader = rDS.Ptr();
	if (m_PipelineState.DomainShader != pDomainShader)
	{
		m_PipelineState.DomainShader = pDomainShader;

		m_FlushState[eFSDomainShader] = true;
	}
}

void D3DGraphic::SetPixelShader(const Ref<ID3D11PixelShader> &rPS)
{
	ID3D11PixelShader *const pPixelShader = rPS.Ptr();
	if (m_PipelineState.PixelShader != pPixelShader)
	{
		m_PipelineState.PixelShader = pPixelShader;

		m_FlushState[eFSPixelShader] = true;
	}
}

void D3DGraphic::SetGeometryShader(const Ref<ID3D11GeometryShader> &rGS)
{
	ID3D11GeometryShader *const pGeometryShader = rGS.Ptr();
	if (m_PipelineState.GeometryShader != pGeometryShader)
	{
		m_PipelineState.GeometryShader = pGeometryShader;

		m_FlushState[eFSGeometryShader] = true;
	}
}

void D3DGraphic::SetComputeShader(const Ref<ID3D11ComputeShader> &rCS)
{
	ID3D11ComputeShader *const pComputeShader = rCS.Ptr();
	if (m_PipelineState.ComputeShader != pComputeShader)
	{
		m_PipelineState.ComputeShader = pComputeShader;

		m_FlushState[eFSComputeShader] = true;
	}
}

void D3DGraphic::SetConstantBuffer(const Ref<ID3D11Buffer> &rConstantBuf, uint32_t slot, eShaderType shaderType)
{
	ID3D11Buffer *const ppBuf[1] = { rConstantBuf.Ptr() };
	switch (shaderType)
	{
	case eVertexShader:
		m_IMContent->VSSetConstantBuffers(slot, 1U, ppBuf);
		break;
	case eHullShader:
		m_IMContent->HSSetConstantBuffers(slot, 1U, ppBuf);
		break;
	case eDomainShader:
		m_IMContent->DSSetConstantBuffers(slot, 1U, ppBuf);
		break;
	case eGeometryShader:
		m_IMContent->GSSetConstantBuffers(slot, 1U, ppBuf);
		break;
	case ePixelShader:
		m_IMContent->PSSetConstantBuffers(slot, 1U, ppBuf);
		break;
	case eComputeShader:
		m_IMContent->CSSetConstantBuffers(slot, 1U, ppBuf);
		break;
	}
}

///void D3DGraphic::GetBackBufferDesc(D3D11_TEXTURE2D_DESC& tex2DDesc)
///{
///	assert(m_SwapChain.Valid());
///
///	Ref<ID3D11Texture2D> backBuffer;
///	HRCheck(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.Reference()));
///
///	backBuffer->GetDesc(&tex2DDesc);
///}

///void D3DGraphic::ResolveSubResource(ID3D11Texture2D* pDstResource, ID3D11Texture2D* pSrcResource, uint32_t dstCount, uint32_t srcCount, DXGI_FORMAT fmt)
///{
///	assert(pDstResource && pSrcResource);
///
///	m_IMContent->ResolveSubresource(pDstResource, dstCount, pSrcResource, srcCount, fmt);
///}

void D3DGraphic::CopyBuffer(ID3D11Resource *pSrc, ID3D11Resource *pDst, void *pDstMem, size_t memSize, ::RECT &rect)
{
	assert(pSrc && pDst && memSize);

	D3D11_BOX box = { 0U };
	box.left = (uint32_t)rect.left;
	box.right = (uint32_t)rect.right;
	box.top = rect.top;
	box.bottom = rect.bottom;
	box.front = 0U;
	box.back = 1U;

	D3D11_MAPPED_SUBRESOURCE mappedSrc = { 0U };

	m_IMContent->CopySubresourceRegion(pSrc, 0U, 0U, 0U, 0U, pDst, 0U, &box);
	HRCheck(m_IMContent->Map(pSrc, 0U, D3D11_MAP_READ, 0U, &mappedSrc));
	memcpy(pDstMem, mappedSrc.pData, memSize);
	m_IMContent->Unmap(pSrc, 0U);
}

void D3DGraphic::Draw(uint32_t vertexCount, uint32_t startVertex, D3D_PRIMITIVE_TOPOLOGY prim)
{
	if (m_PipelineState.PrimitiveTopology != prim)
	{
		m_PipelineState.PrimitiveTopology = prim;
		m_FlushState[eFSPrimitiveTopology] = true;
	}

	FlushState();

	m_IMContent->Draw(vertexCount, startVertex);
}

void D3DGraphic::DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, D3D_PRIMITIVE_TOPOLOGY prim)
{
	if (m_PipelineState.PrimitiveTopology != prim)
	{
		m_PipelineState.PrimitiveTopology = prim;
		m_FlushState[eFSPrimitiveTopology] = true;
	}

	FlushState();

	m_IMContent->DrawIndexed(indexCount, startIndex, offset);
}

void D3DGraphic::DrawAuto(D3D_PRIMITIVE_TOPOLOGY prim)
{
	if (m_PipelineState.PrimitiveTopology != prim)
	{
		m_PipelineState.PrimitiveTopology = prim;
		m_FlushState[eFSPrimitiveTopology] = true;
	}

	FlushState();

	m_IMContent->DrawAuto();
}

void D3DGraphic::FlushState()
{
	if (!m_IMContent.Valid())
	{
		return;
	}

	/// Input Assembler
	if (m_FlushState[eFSInputLayout])
	{
		m_IMContent->IASetInputLayout(m_PipelineState.InputLayout);
		m_FlushState[eFSInputLayout] = false;
	}

	if (m_FlushState[eFSStreamOut])
	{
		m_IMContent->SOSetTargets(1U, m_PipelineState.StreamOut, m_PipelineState.StreamOutOffset);
	}

	if (m_FlushState[eFSVertexBuffer])
	{
		m_IMContent->IASetVertexBuffers(0, eVertexStreamCount, m_PipelineState.VertexBuffer.Buffers,
			m_PipelineState.VertexBuffer.Stride, m_PipelineState.VertexBuffer.Offset);
		m_FlushState[eFSVertexBuffer] = false;
	}

	if (m_FlushState[eFSIndexBuffer])
	{
		m_IMContent->IASetIndexBuffer(m_PipelineState.IndexBuffer.Buffers, m_PipelineState.IndexBuffer.Format,
			m_PipelineState.IndexBuffer.Offset);
		m_FlushState[eFSIndexBuffer] = false;
	}

	if (m_FlushState[eFSPrimitiveTopology])
	{
		m_IMContent->IASetPrimitiveTopology(m_PipelineState.PrimitiveTopology);
		m_FlushState[eFSPrimitiveTopology] = false;
	}

	/// VS->HS->DS->GS
	if (m_FlushState[eFSVertexShader])
	{
		m_IMContent->VSSetShader(m_PipelineState.VertexShader, nullptr, 0U);
		m_FlushState[eFSVertexShader] = false;
	}

	if (m_FlushState[eFSHullShader])
	{
		m_IMContent->HSSetShader(m_PipelineState.HullShader, nullptr, 0U);
		m_FlushState[eFSHullShader] = false;
	}

	if (m_FlushState[eFSDomainShader])
	{
		m_IMContent->DSSetShader(m_PipelineState.DomainShader, nullptr, 0U);
		m_FlushState[eFSDomainShader] = false;
	}

	if (m_FlushState[eFSGeometryShader])
	{
		m_IMContent->GSSetShader(m_PipelineState.GeometryShader, nullptr, 0U);
		m_FlushState[eFSGeometryShader] = false;
	}

	/// Rasterizer
	if (m_FlushState[eFSRasterizerState])
	{
		m_IMContent->RSSetState(m_PipelineState.RasterizerState);
		m_FlushState[eFSRasterizerState] = false;
	}

	if (m_FlushState[eFSViewports])
	{
		m_IMContent->RSSetViewports(m_PipelineState.ViewportCount, &m_DefaultViewport);
		m_FlushState[eFSViewports] = false;
	}

	if (m_FlushState[eFSScissorRect])
	{
		m_IMContent->RSSetScissorRects(m_PipelineState.ScissorRectCount, /*m_PipelineState.ScissorRects*/&m_DefaultScissorRect);
		m_FlushState[eFSScissorRect] = false;
	}

	/// Pixel Shader
	if (m_FlushState[eFSPixelShader])
	{
		m_IMContent->PSSetShader(m_PipelineState.PixelShader, nullptr, 0U);
		m_FlushState[eFSPixelShader] = false;
	}

	if (m_FlushState[eFSComputeShader])
	{
		m_IMContent->CSSetShader(m_PipelineState.ComputeShader, nullptr, 0U);
		m_FlushState[eFSComputeShader] = false;
	}

	/// Output Merge
	if (m_FlushState[eFSBlendState])
	{
		m_IMContent->OMSetBlendState(m_PipelineState.BlendState, m_PipelineState.BlendFactor, m_PipelineState.SampleMask);
		m_FlushState[eFSBlendState] = false;
	}

	if (m_FlushState[eFSDepthStencilState])
	{
		m_IMContent->OMSetDepthStencilState(m_PipelineState.DepthStencilState, m_PipelineState.StencilRef);
		m_FlushState[eFSDepthStencilState] = false;
	}

	if (m_FlushState[eFSRenderTarget] || m_FlushState[eFSDepthStencil] || eBackBufferCount > 1U)
	{
		m_IMContent->OMSetRenderTargets(eRenderTargetCount, m_PipelineState.RenderTarget, m_PipelineState.DepthStencil);
		m_FlushState[eFSRenderTarget] = m_FlushState[eFSDepthStencil] = false;
	}
}