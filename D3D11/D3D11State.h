#pragma once

#include "D3D11Object.h"

class D3D11SamplerState : public D3DObject<ID3D11SamplerState>
{
public:
	void Create(uint32_t filter, uint32_t addressMode, float LODBias, eRComparisonFunc compFunc, const float *pBorderClr, float minLOD, float maxLOD);
};

class D3D11DepthStencilState : public D3DObject<ID3D11DepthStencilState>
{
public:
	void Create(
		bool bDepthEnable,
		uint32_t depthWriteMask,
		eRComparisonFunc compFunc,
		bool bStencilEnable,
		uint8_t stencilReadMask,
		uint8_t stencilWriteMask,
		eRStencilOp stencilFailOpFrontFace,
		eRStencilOp stencilDepthFailOpFrontFace,
		eRStencilOp stencilPassOpFrontFace,
		eRComparisonFunc stencilCompFuncFrontFace,
		eRStencilOp stencilFailOpBackFace,
		eRStencilOp stencilDepthFailOpBackFace,
		eRStencilOp stencilPassOpBackFace,
		eRComparisonFunc stencilCompFuncBackFace);
};

class D3D11BlendState : public D3DObject<ID3D11BlendState>
{
public:
	void Create(
		bool bAlphaToCoverage,
		bool bIndependentBlend,
		uint32_t surfaceIndex,
		bool bBlend,
		eRBlend srcColor,
		eRBlend dstColor,
		eRBlendOp colorOp,
		eRBlend srcAlpha,
		eRBlend dstAlpha,
		eRBlendOp alphaOp,
		uint8_t renderTargetWriteMask);
};

class D3D11RasterizerState : public D3DObject<ID3D11RasterizerState>
{
public:
	void Create(uint32_t fillMode, uint32_t cullMode, bool bFrontCCW, bool bDepthClip, bool bScissor);
};

struct D3D11StaticState
{
	static void Initialize();

	static D3D11SamplerState PointSampler;
	static D3D11SamplerState PointClampSampler;
	static D3D11SamplerState LinearSampler;
	static D3D11SamplerState AnisotropicSampler;

	static D3D11RasterizerState Wireframe;
	static D3D11RasterizerState Solid;
	static D3D11RasterizerState WireframeNoneCulling;
	static D3D11RasterizerState SolidNoneCulling;
	static D3D11RasterizerState SolidFrontCCW;

	static D3D11DepthStencilState DisableDepthStencil;

	static D3D11BlendState NoneBlendState;
	static D3D11DepthStencilState NoneDepthStencilState;

	static bool Inited;
};