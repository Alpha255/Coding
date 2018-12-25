#include "D3D11State.h"
#include "D3D11Engine.h"

void D3D11SamplerState::Create(uint32_t filter, uint32_t addressMode, float LODBias, uint32_t compFunc, const float *pBorderClr, float minLOD, float maxLOD)
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
	
	if (pBorderClr)
	{
		memcpy(&desc.BorderColor, pBorderClr, sizeof(float) * 4U);
	}

	ID3D11SamplerState *pSamplerState = nullptr;
	HRCheck(D3D11Engine::Instance().GetDevice()->CreateSamplerState(&desc, &pSamplerState));

	MakeObject(pSamplerState);
}

void D3D11DepthStencilState::Create(
	bool bDepthEnable,
	uint32_t depthWriteMask,
	uint32_t compFunc,
	bool bStencilEnable,
	uint8_t stencilReadMask,
	uint8_t stencilWriteMask,
	uint32_t stencilFailOpFrontFace,
	uint32_t stencilDepthFailOpFrontFace,
	uint32_t stencilPassOpFrontFace,
	uint32_t stencilCompFuncFrontFace,
	uint32_t stencilFailOpBackFace,
	uint32_t stencilDepthFailOpBackFace,
	uint32_t stencilPassOpBackFace,
	uint32_t stencilCompFuncBackFace)
{
	assert(!IsValid());

	D3D11_DEPTH_STENCIL_DESC desc = {};
	desc.DepthEnable = bDepthEnable;
	desc.DepthWriteMask = (D3D11_DEPTH_WRITE_MASK)depthWriteMask;
	desc.DepthFunc = (D3D11_COMPARISON_FUNC)compFunc;
	desc.StencilEnable = bStencilEnable;
	desc.StencilReadMask = stencilReadMask;
	desc.StencilWriteMask = stencilWriteMask;
	desc.FrontFace.StencilFailOp = (D3D11_STENCIL_OP)stencilFailOpFrontFace;
	desc.FrontFace.StencilDepthFailOp = (D3D11_STENCIL_OP)stencilDepthFailOpFrontFace;
	desc.FrontFace.StencilPassOp = (D3D11_STENCIL_OP)stencilPassOpFrontFace;
	desc.FrontFace.StencilFunc = (D3D11_COMPARISON_FUNC)stencilCompFuncFrontFace;
	desc.BackFace.StencilFailOp = (D3D11_STENCIL_OP)stencilFailOpBackFace;
	desc.BackFace.StencilDepthFailOp = (D3D11_STENCIL_OP)stencilDepthFailOpBackFace;
	desc.BackFace.StencilPassOp = (D3D11_STENCIL_OP)stencilPassOpBackFace;
	desc.BackFace.StencilFunc = (D3D11_COMPARISON_FUNC)stencilCompFuncBackFace;

	ID3D11DepthStencilState *pDepthStencilState = nullptr;
	HRCheck(D3D11Engine::Instance().GetDevice()->CreateDepthStencilState(&desc, &pDepthStencilState));

	MakeObject(pDepthStencilState);
}

void D3D11BlendState::Create(
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
	uint8_t renderTargetWriteMask)
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

	desc.RenderTarget[surfaceIndex].RenderTargetWriteMask = renderTargetWriteMask;

	ID3D11BlendState *pBlendState = nullptr;
	HRCheck(D3D11Engine::Instance().GetDevice()->CreateBlendState(&desc, &pBlendState));

	MakeObject(pBlendState);
}

void D3D11RasterizerState::Create(uint32_t fillMode, uint32_t cullMode, bool bFrontCCW, bool bDepthClip, bool bScissor)
{
	assert(!IsValid());

	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = (D3D11_FILL_MODE)fillMode;
	desc.CullMode = (D3D11_CULL_MODE)cullMode;
	desc.FrontCounterClockwise = bFrontCCW;
	desc.DepthClipEnable = bDepthClip;
	desc.MultisampleEnable = false;
	desc.ScissorEnable = bScissor;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.AntialiasedLineEnable = false;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;

	ID3D11RasterizerState *pRasterizerState = nullptr;
	HRCheck(D3D11Engine::Instance().GetDevice()->CreateRasterizerState(&desc, &pRasterizerState));

	MakeObject(pRasterizerState);
}

D3D11SamplerState D3D11StaticState::PointSampler;
D3D11SamplerState D3D11StaticState::PointClampSampler;
D3D11SamplerState D3D11StaticState::LinearSampler;
D3D11SamplerState D3D11StaticState::AnisotropicSampler;

D3D11RasterizerState D3D11StaticState::Wireframe;
D3D11RasterizerState D3D11StaticState::Solid;
D3D11RasterizerState D3D11StaticState::WireframeNoneCulling;
D3D11RasterizerState D3D11StaticState::SolidNoneCulling;
D3D11RasterizerState D3D11StaticState::SolidFrontCCW;

D3D11DepthStencilState D3D11StaticState::DisableDepthStencil;

D3D11BlendState D3D11StaticState::NoneBlendState;
D3D11DepthStencilState D3D11StaticState::NoneDepthStencilState;

bool D3D11StaticState::Inited = false;

void D3D11StaticState::Initialize()
{
	if (Inited)
	{
		return;
	}

	PointSampler.Create(ePoint, eWrap, 0.0f, D3DState::eNever, nullptr, 0.0f, FLT_MAX);
	PointClampSampler.Create(ePoint, eClamp, 0.0f, D3DState::eAlways, nullptr, 0.0f, FLT_MAX);
	LinearSampler.Create(eLinear, eWrap, 0.0f, D3DState::eNever, nullptr, 0.0f, FLT_MAX);
	AnisotropicSampler.Create(eAnisotropic, eWrap, 0.0f, D3DState::eNever, nullptr, 0.0f, FLT_MAX);

	Wireframe.Create(D3DState::eWireframe, D3DState::eCullBackFace, false, true, false);
	Solid.Create(D3DState::eSolid, D3DState::eCullBackFace, false, true, false);
	WireframeNoneCulling.Create(D3DState::eWireframe, D3DState::eCullNone, false, false, false);
	SolidNoneCulling.Create(D3DState::eSolid, D3DState::eCullNone, false, true, false);
	SolidFrontCCW.Create(D3DState::eSolid, D3DState::eCullBackFace, true, true, false);

	DisableDepthStencil.Create(
		false, D3DState::eDepthMaskAll, D3DState::eAlways,
		false, 0U, 0U,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eAlways,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eAlways);

	Inited = true;
}