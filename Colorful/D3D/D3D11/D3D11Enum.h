#pragma once

#include "Colorful/Public/GfxDefinitions.h"

class D3D11Enum
{
public:
	static D3D11_FILL_MODE toPolygonMode(eRPolygonMode mode);
	static D3D11_CULL_MODE toCullMode(eRCullMode mode);
	static D3D11_BLEND_OP toBlendOp(eRBlendOp op);
	static D3D11_BLEND toBlendFactor(eRBlendFactor factor);
	static uint8_t toColorComponentFlags(uint32_t colorMask);
	static D3D11_COMPARISON_FUNC toCompareOp(eRCompareOp op);
	static D3D11_STENCIL_OP toStencilOp(eRStencilOp op);
	static DXGI_FORMAT toFormat(eRFormat format);
	static D3D11_USAGE toUsage(eRBufferUsage usage);
	static D3D11_FILTER toFilter(eRFilter filter);
	static D3D11_TEXTURE_ADDRESS_MODE toAddressMode(eRSamplerAddressMode addressMode);
	static D3D11_PRIMITIVE_TOPOLOGY toPrimitiveTopology(eRPrimitiveTopology primitiveTopology);
};
