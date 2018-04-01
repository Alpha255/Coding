#pragma once

#include "D3DObject.h"

class D3DState
{
public:
	enum eTextureFilter
	{
		ePoint = D3D11_FILTER_MIN_MAG_MIP_POINT,
		eLinear = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		eAnisotropic = D3D11_FILTER_ANISOTROPIC
	};

	enum eTextureAddressMode
	{
		eWrap = D3D11_TEXTURE_ADDRESS_WRAP,
		eMirror = D3D11_TEXTURE_ADDRESS_MIRROR,
		eClamp = D3D11_TEXTURE_ADDRESS_CLAMP,
		eBorder = D3D11_TEXTURE_ADDRESS_BORDER,
		eMirrorOnce = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE
	};

	enum eFillMode
	{
		eWireframe = D3D11_FILL_WIREFRAME,
		eSolid = D3D11_FILL_SOLID
	};

	enum eCullMode
	{
		eNone = D3D11_CULL_NONE,
		eFront = D3D11_CULL_FRONT,
		eBack = D3D11_CULL_BACK
	};

	enum eComparisonFunc
	{
		eNever = D3D11_COMPARISON_NEVER,
		eLess = D3D11_COMPARISON_LESS,
		eEqual = D3D11_COMPARISON_EQUAL,
		eLessEqual = D3D11_COMPARISON_LESS_EQUAL,
		eGreater = D3D11_COMPARISON_GREATER,
		eNotEqual = D3D11_COMPARISON_NOT_EQUAL,
		eGreaterEqual = D3D11_COMPARISON_GREATER_EQUAL,
		eAlways = D3D11_COMPARISON_ALWAYS
	};

	enum eBlend
	{
		eZero = D3D11_BLEND_ZERO,
		eOne = D3D11_BLEND_ONE,
		eFactor = D3D11_BLEND_BLEND_FACTOR,
		eInvFactor = D3D11_BLEND_INV_BLEND_FACTOR,
	};

	enum eBlendColor
	{
		eSrc = D3D11_BLEND_SRC_COLOR,
		eInvSrc = D3D11_BLEND_INV_SRC_COLOR,
		eDest = D3D11_BLEND_DEST_COLOR,
		eInvDest = D3D11_BLEND_INV_DEST_COLOR,
		eSrc1 = D3D11_BLEND_SRC1_COLOR,
		eInvSrc1 = D3D11_BLEND_INV_SRC1_COLOR,
	};

	enum eBlendAlpha
	{
		eSrcAlpha = D3D11_BLEND_SRC_ALPHA,
		eInvSrcAlpha = D3D11_BLEND_INV_SRC_ALPHA,
		eDestAlpha = D3D11_BLEND_DEST_ALPHA,
		eInvDestAlpha = D3D11_BLEND_INV_DEST_ALPHA,
		eSrcAlphaSat = D3D11_BLEND_SRC_ALPHA_SAT,
		eSrc1Alpha = D3D11_BLEND_SRC1_ALPHA,
		eInvSrc1Alpha = D3D11_BLEND_INV_SRC1_ALPHA
	};

	enum eBlendOp
	{
		eAdd = D3D11_BLEND_OP_ADD,
		eSubtract = D3D11_BLEND_OP_SUBTRACT,
		eRevSubtract = D3D11_BLEND_OP_REV_SUBTRACT,
		eMin = D3D11_BLEND_OP_MIN,
		eMax = D3D11_BLEND_OP_MAX
	};

	enum eBlendWriteMask : uint8_t
	{
		eRed = D3D11_COLOR_WRITE_ENABLE_RED,
		eGreen = D3D11_COLOR_WRITE_ENABLE_GREEN,
		eBlue = D3D11_COLOR_WRITE_ENABLE_BLUE,
		eAlpha = D3D11_COLOR_WRITE_ENABLE_ALPHA,
		eAll = D3D11_COLOR_WRITE_ENABLE_ALL
	};
};

class D3DSamplerState : public D3DObject<ID3D11SamplerState>, public D3DState
{
public:
	void Create(uint32_t filter, uint32_t addressMode, float LODBias, uint32_t compFunc, const float *borderClr, float minLOD, float maxLOD);
};

class D3DDepthStencilState : public D3DObject<ID3D11DepthStencilState>, public D3DState
{
public:
	void Create();
};

class D3DBlendState : public D3DObject<ID3D11BlendState>, public D3DState
{
public:
	void Create(
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
		uint8_t writeMask);
};

class D3DRasterizerState : public D3DObject<ID3D11RasterizerState>, public D3DState
{
public:
	void Create(uint32_t fillMode, uint32_t cullMode, bool cw, bool depthClip, bool bScissor);
};

struct StaticD3DState
{
	D3DSamplerState PointSampler;
	D3DSamplerState LinearSampler;
	D3DSamplerState AnisotropicSampler;

	D3DRasterizerState Wireframe;
	D3DRasterizerState Solid;
	D3DRasterizerState WireframeNoneCulling;
	D3DRasterizerState SolidNoneCulling;
};