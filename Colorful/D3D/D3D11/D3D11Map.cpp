#include "Colorful/D3D/D3D11/D3D11Map.h"

NAMESPACE_START(Gfx)

D3D11_FILL_MODE D3D11Map::polygonMode(EPolygonMode mode)
{
	switch (mode)
	{
	case EPolygonMode::Solid:     return D3D11_FILL_SOLID;
	case EPolygonMode::Wireframe: return D3D11_FILL_WIREFRAME;
	case EPolygonMode::Point:     return D3D11_FILL_WIREFRAME;  /// ???
	}

	assert(0);
	return D3D11_FILL_SOLID;
}

D3D11_CULL_MODE D3D11Map::cullMode(ECullMode mode)
{
	switch (mode)
	{
	case ECullMode::None:      return D3D11_CULL_NONE;
	case ECullMode::FrontFace: return D3D11_CULL_FRONT;
	case ECullMode::BackFace:  return D3D11_CULL_BACK;
	}

	assert(0);
	return D3D11_CULL_NONE;
}

D3D11_BLEND_OP D3D11Map::blendOp(EBlendOp op)
{
	switch (op)
	{
	case EBlendOp::Add:             return D3D11_BLEND_OP_ADD;
	case EBlendOp::Subtract:        return D3D11_BLEND_OP_SUBTRACT;
	case EBlendOp::ReverseSubtract: return D3D11_BLEND_OP_REV_SUBTRACT;
	case EBlendOp::Min:             return D3D11_BLEND_OP_MIN;
	case EBlendOp::Max:             return D3D11_BLEND_OP_MAX;
	}

	assert(0);
	return D3D11_BLEND_OP_ADD;
}

D3D11_BLEND D3D11Map::blendFactor(EBlendFactor factor)
{
	switch (factor)
	{
	case EBlendFactor::Zero:             return D3D11_BLEND_ZERO;
	case EBlendFactor::One:              return D3D11_BLEND_ONE;
	case EBlendFactor::Constant:         return D3D11_BLEND_BLEND_FACTOR;
	case EBlendFactor::InverseConstant:  return D3D11_BLEND_INV_BLEND_FACTOR;
	case EBlendFactor::SrcAlpha:         return D3D11_BLEND_SRC_ALPHA;
	case EBlendFactor::InverseSrcAlpha:  return D3D11_BLEND_INV_SRC_ALPHA;
	case EBlendFactor::DstAlpha:         return D3D11_BLEND_DEST_ALPHA;
	case EBlendFactor::InverseDstAlpha:  return D3D11_BLEND_INV_DEST_ALPHA;
	case EBlendFactor::SrcAlphaSaturate: return D3D11_BLEND_SRC_ALPHA_SAT;
	case EBlendFactor::Src1Alpha:        return D3D11_BLEND_SRC1_ALPHA;
	case EBlendFactor::InverseSrc1Alpha: return D3D11_BLEND_INV_SRC1_ALPHA;
	case EBlendFactor::SrcColor:         return D3D11_BLEND_SRC_COLOR;
	case EBlendFactor::InverseSrcColor:  return D3D11_BLEND_INV_SRC_COLOR;
	case EBlendFactor::DstColor:         return D3D11_BLEND_DEST_COLOR;
	case EBlendFactor::InverseDstColor:  return D3D11_BLEND_INV_DEST_COLOR;
	case EBlendFactor::Src1Color:        return D3D11_BLEND_SRC1_COLOR;
	case EBlendFactor::InverseSrc1Color: return D3D11_BLEND_INV_SRC1_COLOR;
	}

	assert(0);
	return D3D11_BLEND_ZERO;
}

//uint8_t D3D11Map::colorComponentFlags(uint32_t colorMask)
//{
//	if (colorMask == eColorNone)
//	{
//		return 0u;
//	}
//
//	if (colorMask == eColorAll)
//	{
//		return D3D11_COLOR_WRITE_ENABLE_ALL;
//	}
//
//	uint8_t flags = 0u;
//	if (colorMask & eColorRed)
//	{
//		flags |= D3D11_COLOR_WRITE_ENABLE_RED;
//	}
//	if (colorMask & eColorGreen)
//	{
//		flags |= D3D11_COLOR_WRITE_ENABLE_GREEN;
//	}
//	if (colorMask & eColorBlue)
//	{
//		flags |= D3D11_COLOR_WRITE_ENABLE_BLUE;
//	}
//	if (colorMask & eColorAlpha)
//	{
//		flags |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
//	}
//	return flags;
//}

D3D11_COMPARISON_FUNC D3D11Map::compareFunc(ECompareFunc func)
{
	switch (func)
	{
	case ECompareFunc::Never:          return D3D11_COMPARISON_NEVER;
	case ECompareFunc::Less:           return D3D11_COMPARISON_LESS;
	case ECompareFunc::Equal:          return D3D11_COMPARISON_EQUAL;
	case ECompareFunc::LessOrEqual:    return D3D11_COMPARISON_LESS_EQUAL;
	case ECompareFunc::Greater:        return D3D11_COMPARISON_GREATER;
	case ECompareFunc::NotEqual:       return D3D11_COMPARISON_NOT_EQUAL;
	case ECompareFunc::GreaterOrEqual: return D3D11_COMPARISON_GREATER_EQUAL;
	case ECompareFunc::Always:         return D3D11_COMPARISON_ALWAYS;
	}

	assert(0);
	return D3D11_COMPARISON_ALWAYS;
}

D3D11_STENCIL_OP D3D11Map::stencilOp(EStencilOp op)
{
	switch (op)
	{
	case EStencilOp::Keep:              return D3D11_STENCIL_OP_KEEP;
	case EStencilOp::Zero:              return D3D11_STENCIL_OP_ZERO;
	case EStencilOp::Replace:           return D3D11_STENCIL_OP_REPLACE;
	case EStencilOp::IncrementAndClamp: return D3D11_STENCIL_OP_INCR_SAT;
	case EStencilOp::DecrementAndClamp: return D3D11_STENCIL_OP_DECR_SAT;
	case EStencilOp::Invert:            return D3D11_STENCIL_OP_INVERT;
	case EStencilOp::IncrementAndWrap:  return D3D11_STENCIL_OP_INCR;
	case EStencilOp::DecrementAndWrap:  return D3D11_STENCIL_OP_DECR;
	}

	assert(0);
	return D3D11_STENCIL_OP_KEEP;
}

DXGI_FORMAT D3D11Map::format(EFormat format)
{
	return static_cast<DXGI_FORMAT>(FormatAttribute::attribute(format).DXGIFromat);
}

D3D11_USAGE D3D11Map::usage(EBufferUsage usage)
{
	switch (usage)
	{
	case EBufferUsage::Default:   return D3D11_USAGE_DEFAULT;
	case EBufferUsage::Immutable: return D3D11_USAGE_IMMUTABLE;
	case EBufferUsage::Dynamic:   return D3D11_USAGE_DYNAMIC;
	case EBufferUsage::Staging:   return D3D11_USAGE_STAGING;
	}

	assert(0);
	return D3D11_USAGE_DEFAULT;
}

D3D11_FILTER D3D11Map::textureFilter(ETextureFilter filter)
{
	switch (filter)
	{
	case ETextureFilter::Nearest:     return D3D11_FILTER_MIN_MAG_MIP_POINT;
	case ETextureFilter::Linear:      return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	case ETextureFilter::Anisotropic: return D3D11_FILTER_ANISOTROPIC;
	}

	assert(0);
	return (D3D11_FILTER)-1;
}

D3D11_TEXTURE_ADDRESS_MODE D3D11Map::samplerAddressMode(ESamplerAddressMode addressMode)
{
	switch (addressMode)
	{
	case ESamplerAddressMode::Repeat:            return D3D11_TEXTURE_ADDRESS_WRAP;
	case ESamplerAddressMode::MirroredRepeat:    return D3D11_TEXTURE_ADDRESS_MIRROR;
	case ESamplerAddressMode::ClampToEdge:       return D3D11_TEXTURE_ADDRESS_CLAMP;
	case ESamplerAddressMode::ClampToBorder:     return D3D11_TEXTURE_ADDRESS_BORDER;
	case ESamplerAddressMode::MirrorClampToEdge: return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
	}

	assert(0);
	return (D3D11_TEXTURE_ADDRESS_MODE)-1;
}

D3D11_PRIMITIVE_TOPOLOGY D3D11Map::primitiveTopology(EPrimitiveTopology primitiveTopology)
{
	switch (primitiveTopology)
	{
	case EPrimitiveTopology::PointList:        return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	case EPrimitiveTopology::LineList:         return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	case EPrimitiveTopology::LineStrip:        return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case EPrimitiveTopology::TriangleList:     return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case EPrimitiveTopology::TriangleStrip:    return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	case EPrimitiveTopology::LineListAdj:      return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
	case EPrimitiveTopology::LineStripAdj:     return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
	case EPrimitiveTopology::TriangleListAdj:  return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
	case EPrimitiveTopology::TriangleStripAdj: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
	case EPrimitiveTopology::PatchList: 
		assert(0);
		return D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
	}

	assert(0);
	return (D3D11_PRIMITIVE_TOPOLOGY)-1;
}

uint32_t D3D11Map::bindFlags(uint32_t bindFlags)
{
	uint32_t flags = 0u;

	if (bindFlags & EBufferBindFlags::VertexBuffer)
	{
		flags |= D3D11_BIND_VERTEX_BUFFER;
	}
	if (bindFlags & EBufferBindFlags::IndexBuffer)
	{
		flags |= D3D11_BIND_INDEX_BUFFER;
	}
	if (bindFlags & EBufferBindFlags::UniformBuffer)
	{
		flags |= D3D11_BIND_CONSTANT_BUFFER;
	}
	if (bindFlags & EBufferBindFlags::ShaderResource)
	{
		flags |= D3D11_BIND_SHADER_RESOURCE;
	}
	if (bindFlags & EBufferBindFlags::StreamOutput)
	{
		flags |= D3D11_BIND_STREAM_OUTPUT;
	}
	if (bindFlags & EBufferBindFlags::UnorderedAccess)
	{
		flags |= D3D11_BIND_UNORDERED_ACCESS;
	}
	if (bindFlags & EBufferBindFlags::IndirectBuffer)
	{
		assert(0);
	}

	return flags;
}

NAMESPACE_END(Gfx)
