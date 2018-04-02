#include "D3DGraphic.h"
#include "Utility.h"

#include <fstream>
#include <string>
#include <DirectXTK/Inc/DDSTextureLoader.h>

D3DGraphic* D3DGraphic::m_sInstance = nullptr;

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