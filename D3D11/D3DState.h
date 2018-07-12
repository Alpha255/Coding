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
		eCullNone = D3D11_CULL_NONE,
		eCullFrontFace = D3D11_CULL_FRONT,
		eCullBackFace = D3D11_CULL_BACK
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
		eColorNone = 0,
		eColorRed = D3D11_COLOR_WRITE_ENABLE_RED,
		eColorGreen = D3D11_COLOR_WRITE_ENABLE_GREEN,
		eColorBlue = D3D11_COLOR_WRITE_ENABLE_BLUE,
		eColorAlpha = D3D11_COLOR_WRITE_ENABLE_ALPHA,
		eColorAll = D3D11_COLOR_WRITE_ENABLE_ALL
	};

	enum eDepthWriteMask
	{
		eDepthMaskZero = D3D11_DEPTH_WRITE_MASK_ZERO,  /// Turn off writes to the depth-stencil buffer.
		eDepthMaskAll = D3D11_DEPTH_WRITE_MASK_ALL     /// Turn on writes to the depth-stencil buffer.
	};

	enum eStencilMask : uint8_t
	{
		eStencilDefaultReadMask = D3D11_DEFAULT_STENCIL_READ_MASK,
		eStencilDefaultWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK
	};

	enum eStencilOp
	{
		eStencilKeep = D3D11_STENCIL_OP_KEEP,
		eStencilZero = D3D11_STENCIL_OP_ZERO,
		eStencilReplace = D3D11_STENCIL_OP_REPLACE,  /// Set the stencil data to the reference value
		eStencilIncrSat = D3D11_STENCIL_OP_INCR_SAT, /// Increment the stencil value by 1, and clamp the result
		eStencilDecrSat = D3D11_STENCIL_OP_DECR_SAT, /// Decrement the stencil value by 1, and clamp the result
		eStencilInvert = D3D11_STENCIL_OP_INVERT,
		eStencilIncr = D3D11_STENCIL_OP_INCR,        /// Increment the stencil value by 1, and wrap the result if necessary
		eStencilDecr = D3D11_STENCIL_OP_DECR         /// Increment the stencil value by 1, and wrap the result if necessary
	};
};

class D3DSamplerState : public D3DObject<ID3D11SamplerState>, public D3DState
{
public:
	void Create(uint32_t filter, uint32_t addressMode, float LODBias, uint32_t compFunc, const float *pBorderClr, float minLOD, float maxLOD);
};

class D3DDepthStencilState : public D3DObject<ID3D11DepthStencilState>, public D3DState
{
public:
	void Create(
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
		uint32_t stencilCompFuncBackFace);
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
		uint8_t renderTargetWriteMask);
};

class D3DRasterizerState : public D3DObject<ID3D11RasterizerState>, public D3DState
{
public:
	void Create(uint32_t fillMode, uint32_t cullMode, bool bFrontCCW, bool bDepthClip, bool bScissor);
};

struct D3DStaticState
{
	static void Initialize();

	static D3DSamplerState PointSampler;
	static D3DSamplerState PointClampSampler;
	static D3DSamplerState LinearSampler;
	static D3DSamplerState AnisotropicSampler;

	static D3DRasterizerState Wireframe;
	static D3DRasterizerState Solid;
	static D3DRasterizerState WireframeNoneCulling;
	static D3DRasterizerState SolidNoneCulling;
	static D3DRasterizerState SolidFrontCCW;

	static D3DDepthStencilState DisableDepthStencil;

	static D3DBlendState NoneBlendState;
	static D3DDepthStencilState NoneDepthStencilState;

	static bool Inited;
};