#include "D3D11State.h"
#include "D3D11Engine.h"

void d3d11RasterizerState::create(
	eRPolygonMode polygonMode, 
	eRCullMode cullMode, 
	eRFrontFace frontFace, 
	bool8_t enableDepthBias, 
	float32_t depthBias, 
	float32_t depthBiasClamp, 
	float32_t depthBiasSlope)
{
	assert(!isValid());

	D3D11_RASTERIZER_DESC desc
	{
		D3D11Enum::toPolygonMode(polygonMode),
		D3D11Enum::toCullMode(cullMode),
		frontFace == eCounterclockwise ? true : false,
		enableDepthBias ? (int32_t)depthBias : 0,
		depthBiasClamp,
		depthBiasSlope,
		false,
		false,
		false,
		false
	};

	ID3D11RasterizerState *pRasterizerState = nullptr;
	GfxVerifyD3D(D3D11Engine::instance().getDevice()->CreateRasterizerState(&desc, &pRasterizerState));
	reset(pRasterizerState);
}

void d3d11BlendState::create(
	bool8_t enableLogicOp,
	eRLogicOp logicOp, 
	uint32_t renderTargetCount,
	const GfxBlendStateDesc::ColorBlendState * const clrBlendState)
{
	assert(!isValid());

	(void)enableLogicOp;
	(void)logicOp;

	D3D11_BLEND_DESC desc
	{
		false,
		false
	};

	for (uint32_t i = 0u; i < renderTargetCount; ++i)
	{
		desc.RenderTarget[i] = D3D11_RENDER_TARGET_BLEND_DESC
		{
			clrBlendState[i].Enable,
			D3D11Enum::toBlendFactor(clrBlendState[i].SrcColor),
			D3D11Enum::toBlendFactor(clrBlendState[i].DstColor),
			D3D11Enum::toBlendOp(clrBlendState[i].ColorOp),
			D3D11Enum::toBlendFactor(clrBlendState[i].SrcAlpha),
			D3D11Enum::toBlendFactor(clrBlendState[i].DstColor),
			D3D11Enum::toBlendOp(clrBlendState[i].AlphaOp),
			D3D11Enum::toColorComponentFlags(clrBlendState[i].ColorMask)
		};
	}

	ID3D11BlendState *pBlendState = nullptr;
	GfxVerifyD3D(D3D11Engine::instance().getDevice()->CreateBlendState(&desc, &pBlendState));
	reset(pBlendState);
}

void d3d11DepthStencilState::create(
	bool8_t enableDepth, 
	bool8_t enableDepthWrite, 
	eRCompareOp depthCompareOp, 
	bool8_t enableStencil, 
	uint8_t stencilReadMask, 
	uint8_t stencilWriteMask, 
	const GfxDepthStencilStateDesc::StencilOp &front,
	const GfxDepthStencilStateDesc::StencilOp &back)
{
	assert(!isValid());

	D3D11_DEPTH_STENCIL_DESC desc
	{
		enableDepth,
		enableDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO,
		D3D11Enum::toCompareOp(depthCompareOp),
		enableStencil,
		stencilReadMask,
		stencilWriteMask,
		{
			D3D11Enum::toStencilOp(front.FailOp),
			D3D11Enum::toStencilOp(front.DepthFailOp),
			D3D11Enum::toStencilOp(front.PassOp),
			D3D11Enum::toCompareOp(front.CompareOp),
		},
		{
			D3D11Enum::toStencilOp(back.FailOp),
			D3D11Enum::toStencilOp(back.DepthFailOp),
			D3D11Enum::toStencilOp(back.PassOp),
			D3D11Enum::toCompareOp(back.CompareOp),
		},
	};

	ID3D11DepthStencilState *pDepthStencilState = nullptr;
	GfxVerifyD3D(D3D11Engine::instance().getDevice()->CreateDepthStencilState(&desc, &pDepthStencilState));
	reset(pDepthStencilState);
}
