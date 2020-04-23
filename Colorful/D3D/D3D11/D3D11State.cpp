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
		d3d11Engine::enumTranslator::toPolygonMode(polygonMode),
		d3d11Engine::enumTranslator::toCullMode(cullMode),
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
	rVerifyD3D11(d3d11Engine::instance().getDevice()->CreateRasterizerState(&desc, &pRasterizerState));
	reset(pRasterizerState);
}

void d3d11BlendState::create(
	bool8_t enableLogicOp,
	eRLogicOp logicOp, 
	uint32_t renderTargetCount,
	const rColorBlendStateDesc * const clrBlendState)
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
			d3d11Engine::enumTranslator::toBlendFactor(clrBlendState[i].SrcColor),
			d3d11Engine::enumTranslator::toBlendFactor(clrBlendState[i].DstColor),
			d3d11Engine::enumTranslator::toBlendOp(clrBlendState[i].ColorOp),
			d3d11Engine::enumTranslator::toBlendFactor(clrBlendState[i].SrcAlpha),
			d3d11Engine::enumTranslator::toBlendFactor(clrBlendState[i].DstColor),
			d3d11Engine::enumTranslator::toBlendOp(clrBlendState[i].AlphaOp),
			d3d11Engine::enumTranslator::toColorComponentFlags(clrBlendState[i].ColorMask)
		};
	}

	ID3D11BlendState *pBlendState = nullptr;
	rVerifyD3D11(d3d11Engine::instance().getDevice()->CreateBlendState(&desc, &pBlendState));
	reset(pBlendState);
}

void d3d11DepthStencilState::create(
	bool8_t enableDepth, 
	bool8_t enableDepthWrite, 
	eRCompareOp depthCompareOp, 
	bool8_t enableStencil, 
	uint8_t stencilReadMask, 
	uint8_t stencilWriteMask, 
	const rStencilOpDesc &front, 
	const rStencilOpDesc &back)
{
	assert(!isValid());

	D3D11_DEPTH_STENCIL_DESC desc
	{
		enableDepth,
		enableDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO,
		d3d11Engine::enumTranslator::toCompareOp(depthCompareOp),
		enableStencil,
		stencilReadMask,
		stencilWriteMask,
		{
			d3d11Engine::enumTranslator::toStencilOp(front.FailOp),
			d3d11Engine::enumTranslator::toStencilOp(front.DepthFailOp),
			d3d11Engine::enumTranslator::toStencilOp(front.PassOp),
			d3d11Engine::enumTranslator::toCompareOp(front.CompareOp),
		},
		{
			d3d11Engine::enumTranslator::toStencilOp(back.FailOp),
			d3d11Engine::enumTranslator::toStencilOp(back.DepthFailOp),
			d3d11Engine::enumTranslator::toStencilOp(back.PassOp),
			d3d11Engine::enumTranslator::toCompareOp(back.CompareOp),
		},
	};

	ID3D11DepthStencilState *pDepthStencilState = nullptr;
	rVerifyD3D11(d3d11Engine::instance().getDevice()->CreateDepthStencilState(&desc, &pDepthStencilState));
	reset(pDepthStencilState);
}
