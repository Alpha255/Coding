#include "D3DState.h"
#include "D3DEngine.h"

void D3DSamplerState::Create(uint32_t filter, uint32_t addressMode, float LODBias, uint32_t compFunc, const float *borderClr, float minLOD, float maxLOD)
{
	assert(!IsValid());

	D3D11_SAMPLER_DESC desc = {};
	desc.Filter = (D3D11_FILTER)filter;
	desc.AddressU = desc.AddressV = desc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)addressMode;
	desc.MipLODBias = LODBias;
	desc.MaxAnisotropy = 0U;
	desc.MinLOD = minLOD;
	desc.MaxLOD = maxLOD;
	desc.ComparisonFunc = (D3D11_COMPARISON_FUNC)compFunc;
	memcpy(&desc.BorderColor, borderClr, sizeof(float) * 4U);

	ID3D11SamplerState *pSamplerState = nullptr;
	HRCheck(D3DEngine::Instance().GetDevice()->CreateSamplerState(&desc, &pSamplerState));

	MakeObject(pSamplerState);
}

void D3DBlendState::Create(
	bool bAlphaToCoverage,
	bool bIndependentBlend,
	uint32_t surfaceIndex,
	bool bBlend,
	uint32_t srcColor,
	uint32_t dstColor,
	uint32_t colorOp,
	uint32_t srcAlpha,
	uint32_t dstAlpha,
	uint32_t alphaOp,
	uint8_t writeMask)
{
	assert(!IsValid() && surfaceIndex < 8U);

	D3D11_BLEND_DESC desc = {};
	desc.AlphaToCoverageEnable = bAlphaToCoverage;
	desc.IndependentBlendEnable = bIndependentBlend;
	desc.RenderTarget[surfaceIndex].BlendEnable = bBlend;
	desc.RenderTarget[surfaceIndex].SrcBlend = (D3D11_BLEND)srcColor;
	desc.RenderTarget[surfaceIndex].DestBlend = (D3D11_BLEND)dstColor;
	desc.RenderTarget[surfaceIndex].BlendOp = (D3D11_BLEND_OP)colorOp;

	desc.RenderTarget[surfaceIndex].SrcBlendAlpha = (D3D11_BLEND)srcAlpha;
	desc.RenderTarget[surfaceIndex].DestBlendAlpha = (D3D11_BLEND)dstAlpha;
	desc.RenderTarget[surfaceIndex].BlendOpAlpha = (D3D11_BLEND_OP)alphaOp;

	desc.RenderTarget[surfaceIndex].RenderTargetWriteMask = writeMask;

	ID3D11BlendState *pBlendState = nullptr;
	HRCheck(D3DEngine::Instance().GetDevice()->CreateBlendState(&desc, &pBlendState));

	MakeObject(pBlendState);
}

void D3DRasterizerState::Create(uint32_t fillMode, uint32_t cullMode, bool cw, bool depthClip, bool bScissor)
{
	assert(!IsValid());

	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = (D3D11_FILL_MODE)fillMode;
	desc.CullMode = (D3D11_CULL_MODE)cullMode;
	desc.FrontCounterClockwise = cw;
	desc.DepthClipEnable = depthClip;
	desc.MultisampleEnable = false;
	desc.ScissorEnable = bScissor;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.AntialiasedLineEnable = false;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;

	ID3D11RasterizerState *pRasterizerState = nullptr;
	HRCheck(D3DEngine::Instance().GetDevice()->CreateRasterizerState(&desc, &pRasterizerState));

	MakeObject(pRasterizerState);
}