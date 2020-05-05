#pragma once

#include "Colorful/D3D/DxgiObject.h"

class d3d11RasterizerState : public D3DObject<ID3D11RasterizerState>
{
public:
	void create(eRPolygonMode polygonMode,
		eRCullMode cullMode,
		eRFrontFace frontFace,
		bool8_t enableDepthBias,
		float32_t depthBias,
		float32_t depthBiasClamp,
		float32_t depthBiasSlope);
};

class d3d11BlendState : public D3DObject<ID3D11BlendState>
{
public:
	void create(
		bool8_t enableLogicOp,
		eRLogicOp logicOp,
		uint32_t renderTargetCount,
		const GfxBlendStateDesc::ColorBlendState * const clrBlendState);
};

class d3d11DepthStencilState : public D3DObject<ID3D11DepthStencilState>
{
public:
	void create(
		bool8_t enableDepth,
		bool8_t enableDepthWrite,
		eRCompareOp depthCompareOp,
		bool8_t enableStencil,
		uint8_t stencilReadMask,
		uint8_t stencilWriteMask,
		const GfxDepthStencilStateDesc::StencilOp &front,
		const GfxDepthStencilStateDesc::StencilOp &back);
};
