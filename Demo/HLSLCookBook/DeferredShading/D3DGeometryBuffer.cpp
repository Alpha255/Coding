#include "D3DGeometryBuffer.h"
#include "D3DEngine.h"

void D3DGeometryBuffer::Init(uint32_t width, uint32_t height, bool bReinit)
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

	if (bReinit)
	{
		return;
	}

	m_DepthStencilState.Create(
		true, D3DState::eDepthMaskAll, D3DState::eLess,
		true, D3DState::eStencilDefaultReadMask, D3DState::eStencilDefaultWriteMask,
		D3DState::eStencilReplace, D3DState::eStencilReplace, D3DState::eStencilReplace, D3DState::eAlways,
		D3DState::eStencilReplace, D3DState::eStencilReplace, D3DState::eStencilReplace, D3DState::eAlways);

	m_VertexShader.Create("DeferredShading\\VisualizeGBuffer.hlsl", "VSMain");
	m_PixelShader.Create("DeferredShading\\VisualizeGBuffer.hlsl", "PSMain");

	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite);
}

void D3DGeometryBuffer::Resize(uint32_t width, uint32_t height)
{
	m_SurfaceColorSpecIntensity.Reset();
	m_SurfaceNormal.Reset();
	m_SurfaceSpecPower.Reset();

	m_SurfaceDepthStencil.Reset();
	m_SurfaceDepthStencilReadonly.Reset();

	for (uint32_t i = 0U; i < eBufferTypeCount; ++i)
	{
		m_ShaderResourceViews[i].Reset();
	}

	Init(width, height, true);
}

void D3DGeometryBuffer::Bind()
{
	D3DEngine::Instance().ClearDepthStencilView(m_SurfaceDepthStencil);

	D3DEngine::Instance().ClearRenderTargetView(m_SurfaceColorSpecIntensity, Color::Black);
	D3DEngine::Instance().ClearRenderTargetView(m_SurfaceNormal, Color::Black);
	D3DEngine::Instance().ClearRenderTargetView(m_SurfaceSpecPower, Color::Black);

	D3DEngine::Instance().SetRenderTargetView(m_SurfaceColorSpecIntensity, 0U);
	D3DEngine::Instance().SetRenderTargetView(m_SurfaceNormal, 1U);
	D3DEngine::Instance().SetRenderTargetView(m_SurfaceSpecPower, 2U);

	D3DEngine::Instance().SetDepthStencilView(m_SurfaceDepthStencil);

	D3DEngine::Instance().SetDepthStencilState(m_DepthStencilState, 0x01U);
}

void D3DGeometryBuffer::UnBind()
{
	D3DRenderTargetView EmptyRTV;
	D3DEngine::Instance().SetRenderTargetView(EmptyRTV, 0U);
	D3DEngine::Instance().SetRenderTargetView(EmptyRTV, 1U);
	D3DEngine::Instance().SetRenderTargetView(EmptyRTV, 2U);

	D3DEngine::Instance().SetDepthStencilView(m_SurfaceDepthStencilReadonly);

	D3DShaderResourceView EmptySRV;
	for (uint32_t i = 0U; i < eBufferTypeCount; ++i)
	{
		D3DEngine::Instance().SetShaderResourceView(EmptySRV, i, D3DShader::ePixelShader);
	}

	D3DEngine::Instance().ForceCommitState();
}

void D3DGeometryBuffer::VisulizeGBuffer(bool bVisulize, const Vec4 &camPerspective)
{
	if (bVisulize)
	{
		D3DDepthStencilView EmptyDSV;
		D3DEngine::Instance().SetDepthStencilView(EmptyDSV);

		D3DInputLayout EmptyLayout;
		D3DBuffer EmptyVertexBuffer;
		D3DEngine::Instance().SetInputLayout(EmptyLayout);
		D3DEngine::Instance().SetVertexBuffer(EmptyVertexBuffer, 0U, 0U, 0U);

		D3DEngine::Instance().SetVertexShader(m_VertexShader);
		D3DEngine::Instance().SetPixelShader(m_PixelShader);

		ConstantBufferPS CBufferPS;
		CBufferPS.Perspective = camPerspective;
		m_CBufferPS.Update(&CBufferPS, sizeof(ConstantBufferPS));
		D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

		D3DEngine::Instance().SetSamplerState(D3DStaticState::PointSampler, 0U, D3DShader::ePixelShader);

		for (uint32_t i = 0U; i < eBufferTypeCount; ++i)
		{
			D3DEngine::Instance().SetShaderResourceView(m_ShaderResourceViews[i], i, D3DShader::ePixelShader);
		}

		D3DEngine::Instance().Draw(16U, 0U, eTriangleStrip);
	}

	D3DShaderResourceView EmptySRV;
	for (uint32_t i = 0U; i < eBufferTypeCount; ++i)
	{
		D3DEngine::Instance().SetShaderResourceView(EmptySRV, i, D3DShader::ePixelShader);
	}
}