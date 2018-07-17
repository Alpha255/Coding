#include "D3DGeometryBuffer.h"

void D3DGeometryBuffer::Init(uint32_t width, uint32_t height)
{
	D3DTexture2D depthTex;
	depthTex.Create(eR24G8_Typeless, width, height, D3DBuffer::eBindAsDepthStencil | D3DBuffer::eBindAsShaderResource, 1U);
	m_SurfaceDepthStencil.CreateAsTexture(D3DView::eTexture2D, depthTex, eD24_UNorm_S8_UInt, 0U, 0U);
	m_SurfaceDepthStencilReadonly.CreateAsTexture(D3DView::eTexture2D, depthTex, eD24_UNorm_S8_UInt, D3DView::eReadonlyDepthStencil, 0U);
	m_ShaderResourceViews[eDepthStencil].CreateAsTexture(D3DView::eTexture2D, depthTex, eR24_UNorm_X8_Typeless, 0U, 1U);

	D3DTexture2D colorTex;
	colorTex.Create(eRGBA8_UNorm, width, height, D3DBuffer::eBindAsRenderTarget | D3DBuffer::eBindAsShaderResource);
	m_SurfaceColorSpecIntensity.CreateAsTexture(D3DView::eTexture2D, colorTex, eRGBA8_UNorm, 0U);
	m_ShaderResourceViews[eColorSpecIntensity].CreateAsTexture(D3DView::eTexture2D, colorTex, eRGBA8_UNorm, 0U, 1U);

	D3DTexture2D normalTex;
	normalTex.Create(eRG11B10_Float, width, height, D3DBuffer::eBindAsRenderTarget | D3DBuffer::eBindAsShaderResource);
	m_SurfaceNormal.CreateAsTexture(D3DView::eTexture2D, normalTex, eRG11B10_Float, 0U);
	m_ShaderResourceViews[eNormal].CreateAsTexture(D3DView::eTexture2D, normalTex, eRG11B10_Float, 0U, 1U);

	D3DTexture2D specPowerTex;
	specPowerTex.Create(eRGBA8_UNorm, width, height, D3DBuffer::eBindAsRenderTarget | D3DBuffer::eBindAsShaderResource);
	m_SurfaceSpecPower.CreateAsTexture(D3DView::eTexture2D, specPowerTex, eRGBA8_UNorm, 0U);
	m_ShaderResourceViews[eSpecPower].CreateAsTexture(D3DView::eTexture2D, specPowerTex, eRGBA8_UNorm, 0U, 1U);

	m_DepthStencilState.Create(
		true, D3DState::eDepthMaskAll, D3DState::eLess,
		true, D3DState::eStencilDefaultReadMask, D3DState::eStencilDefaultWriteMask,
		D3DState::eStencilReplace, D3DState::eStencilReplace, D3DState::eStencilReplace, D3DState::eAlways,
		D3DState::eStencilReplace, D3DState::eStencilReplace, D3DState::eStencilReplace, D3DState::eAlways);
}
