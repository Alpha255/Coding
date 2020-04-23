#pragma once

#include "Colorful/D3D/DxgiObject.h"

class d3d11RasterizerState : public d3dObject<ID3D11RasterizerState>
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

class d3d11BlendState : public d3dObject<ID3D11BlendState>
{
public:
	void create(
		bool8_t enableLogicOp,
		eRLogicOp logicOp,
		uint32_t renderTargetCount,
		const rColorBlendStateDesc * const clrBlendState);
};

class d3d11DepthStencilState : public d3dObject<ID3D11DepthStencilState>
{
public:
	void create(
		bool8_t enableDepth,
		bool8_t enableDepthWrite,
		eRCompareOp depthCompareOp,
		bool8_t enableStencil,
		uint8_t stencilReadMask,
		uint8_t stencilWriteMask,
		const rStencilOpDesc &front,
		const rStencilOpDesc &back);
};
