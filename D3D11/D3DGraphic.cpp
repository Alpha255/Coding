#include "D3DGraphic.h"

D3DGraphic* D3DGraphic::m_sInstance = nullptr;

void D3DGraphic::SetStreamOut(const Ref<ID3D11Buffer> &rBuf, uint32_t offset)
{
	if (rBuf.Ptr() != m_PipelineState.StreamOut[0] || offset != m_PipelineState.StreamOutOffset[0])
	{
		m_PipelineState.StreamOut[0] = rBuf.Ptr();
		m_PipelineState.StreamOutOffset[0] = offset;

		m_FlushState[eFSStreamOut] = true;
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

void D3DGraphic::DrawAuto(D3D_PRIMITIVE_TOPOLOGY prim)
{
	if (m_PipelineState.PrimitiveTopology != prim)
	{
		m_PipelineState.PrimitiveTopology = prim;
		m_FlushState[eFSPrimitiveTopology] = true;
	}

	///FlushState();

	m_IMContent->DrawAuto();
}