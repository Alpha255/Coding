#include "Colorful/D3D/D3D11/D3D11RenderState.h"
#include "Colorful/D3D/D3D11/D3D11Map.h"

NAMESPACE_START(Gfx)

D3D11Sampler::D3D11Sampler(ID3D11Device* device, const SamplerDesc& desc)
{
	assert(device);

	Vec4 borderColor{};
	switch (desc.BorderColor)  /// ???? 
	{
	case EBorderColor::FloatTransparentBlack:
	case EBorderColor::IntTransparentBlack:
		break;
	case EBorderColor::FloatOpaqueBlack:
	case EBorderColor::IntOpaqueBlack:
		borderColor = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		break;
	case EBorderColor::FloatOpaqueWhite:
	case EBorderColor::IntOpaqueWhite:
		borderColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		break;
	}

	D3D11_SAMPLER_DESC samplerDesc
	{
		D3D11Map::textureFilter(desc.MinMagFilter),
		D3D11Map::samplerAddressMode(desc.AddressModeU),
		D3D11Map::samplerAddressMode(desc.AddressModeV),
		D3D11Map::samplerAddressMode(desc.AddressModeW),
		desc.MipLodBias,
		desc.MaxAnisotropy,
		D3D11Map::compareFunc(desc.CompareOp),
		{ 
			borderColor.x, 
			borderColor.y, 
			borderColor.z, 
			borderColor.w
		},
		desc.MinLod,
		desc.MaxLod
	};
	VERIFY_D3D(device->CreateSamplerState(&samplerDesc, reference()));
}

NAMESPACE_END(Gfx)

#if 0
#include "Colorful/D3D/D3D11/D3D11Engine.h"

D3D11RasterizerState::D3D11RasterizerState(const D3D11Device& device, const GfxRasterizerStateDesc& gfxDesc)
{
	assert(device.isValid());

	D3D11_RASTERIZER_DESC desc
	{
		D3D11Enum::toPolygonMode(gfxDesc.PolygonMode),
		D3D11Enum::toCullMode(gfxDesc.CullMode),
		gfxDesc.FrontFace == eCounterclockwise ? true : false,
		gfxDesc.EnableDepthBias ? (int32_t)gfxDesc.DepthBias : 0,
		gfxDesc.DepthBiasClamp,
		gfxDesc.DepthBiasSlope,
		false,
		false,
		false,
		false
	};

	ID3D11RasterizerState* rasterizerState = nullptr;
	GfxVerifyD3D(device->CreateRasterizerState(&desc, &rasterizerState));
	reset(rasterizerState);
}

D3D11BlendState::D3D11BlendState(const D3D11Device& device, const GfxBlendStateDesc& gfxDesc)
{
	assert(device.isValid());

	D3D11_BLEND_DESC desc
	{
		false,  /// AlphaToCoverageEnable
		false   /// IndependentBlendEnable
	};

	for (uint32_t i = 0u; i < eMaxRenderTargets; ++i)
	{
		desc.RenderTarget[i] = D3D11_RENDER_TARGET_BLEND_DESC
		{
			gfxDesc.ColorBlendStates[i].Enable,
			D3D11Enum::toBlendFactor(gfxDesc.ColorBlendStates[i].SrcColor),
			D3D11Enum::toBlendFactor(gfxDesc.ColorBlendStates[i].DstColor),
			D3D11Enum::toBlendOp(gfxDesc.ColorBlendStates[i].ColorOp),
			D3D11Enum::toBlendFactor(gfxDesc.ColorBlendStates[i].SrcAlpha),
			D3D11Enum::toBlendFactor(gfxDesc.ColorBlendStates[i].DstColor),
			D3D11Enum::toBlendOp(gfxDesc.ColorBlendStates[i].AlphaOp),
			D3D11Enum::toColorComponentFlags(gfxDesc.ColorBlendStates[i].ColorMask)
		};
	}

	ID3D11BlendState* blendState = nullptr;
	GfxVerifyD3D(device->CreateBlendState(&desc, &blendState));
	reset(blendState);
}

D3D11DepthStencilState::D3D11DepthStencilState(const D3D11Device& device, const GfxDepthStencilStateDesc& gfxDesc)
{
	assert(device.isValid());

	D3D11_DEPTH_STENCIL_DESC desc
	{
		gfxDesc.EnableDepth,
		gfxDesc.EnableDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO,
		D3D11Enum::toCompareOp(gfxDesc.DepthCompareOp),
		gfxDesc.EnableStencil,
		gfxDesc.StencilReadMask,
		gfxDesc.StencilWriteMask,
		{
			D3D11Enum::toStencilOp(gfxDesc.FrontFace.FailOp),
			D3D11Enum::toStencilOp(gfxDesc.FrontFace.DepthFailOp),
			D3D11Enum::toStencilOp(gfxDesc.FrontFace.PassOp),
			D3D11Enum::toCompareOp(gfxDesc.FrontFace.CompareOp),
		},
		{
			D3D11Enum::toStencilOp(gfxDesc.BackFace.FailOp),
			D3D11Enum::toStencilOp(gfxDesc.BackFace.DepthFailOp),
			D3D11Enum::toStencilOp(gfxDesc.BackFace.PassOp),
			D3D11Enum::toCompareOp(gfxDesc.BackFace.CompareOp),
		},
	};

	ID3D11DepthStencilState* depthStencilState = nullptr;
	GfxVerifyD3D(device->CreateDepthStencilState(&desc, &depthStencilState));
	reset(depthStencilState);
}
#endif