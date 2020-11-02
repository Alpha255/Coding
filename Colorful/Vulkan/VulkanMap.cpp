#include "Colorful/Vulkan/VulkanMap.h"

NAMESPACE_START(Gfx)

VkPolygonMode VulkanMap::polygonMode(EPolygonMode mode)
{
	switch (mode)
	{
	case EPolygonMode::Solid:     return VK_POLYGON_MODE_FILL;
	case EPolygonMode::Wireframe: return VK_POLYGON_MODE_LINE;
	case EPolygonMode::Point:     return VK_POLYGON_MODE_POINT;
	}

	assert(0);
	return VK_POLYGON_MODE_MAX_ENUM;
}

VkCullModeFlags VulkanMap::cullMode(ECullMode mode)
{
	switch (mode)
	{
	case ECullMode::None:      return VK_CULL_MODE_NONE;
	case ECullMode::FrontFace: return VK_CULL_MODE_FRONT_BIT;
	case ECullMode::BackFace:  return VK_CULL_MODE_BACK_BIT;
	}

	assert(0);
	return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
}

VkFrontFace VulkanMap::frontFace(EFrontFace frontFace)
{
	switch (frontFace)
	{
	case EFrontFace::Counterclockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
	case EFrontFace::Clockwise:        return VK_FRONT_FACE_CLOCKWISE;
	}

	assert(0);
	return VK_FRONT_FACE_MAX_ENUM;
}

VkLogicOp VulkanMap::logicOp(ELogicOp op)
{
	switch (op)
	{
	case ELogicOp::Clear:         return VK_LOGIC_OP_CLEAR;
	case ELogicOp::And:           return VK_LOGIC_OP_AND;
	case ELogicOp::And_Reverse:   return VK_LOGIC_OP_AND_REVERSE;
	case ELogicOp::Copy:          return VK_LOGIC_OP_COPY;
	case ELogicOp::And_Inverted:  return VK_LOGIC_OP_AND_INVERTED;
	case ELogicOp::No:            return VK_LOGIC_OP_NO_OP;
	case ELogicOp::Xor:           return VK_LOGIC_OP_XOR;
	case ELogicOp::Or:            return VK_LOGIC_OP_OR;
	case ELogicOp::Nor:           return VK_LOGIC_OP_NOR;
	case ELogicOp::Equivalent:    return VK_LOGIC_OP_EQUIVALENT;
	case ELogicOp::Invert:        return VK_LOGIC_OP_INVERT;
	case ELogicOp::Or_Reverse:    return VK_LOGIC_OP_OR_REVERSE;
	case ELogicOp::Copy_Inverted: return VK_LOGIC_OP_COPY_INVERTED;
	case ELogicOp::Or_Inverted:   return VK_LOGIC_OP_OR_INVERTED;
	case ELogicOp::Nand:          return VK_LOGIC_OP_NAND;
	case ELogicOp::Set:           return VK_LOGIC_OP_SET;
	}

	assert(0);
	return VK_LOGIC_OP_MAX_ENUM;
}

VkBlendOp VulkanMap::blendOp(EBlendOp op)
{
	switch (op)
	{
	case EBlendOp::Add:             return VK_BLEND_OP_ADD;
	case EBlendOp::Subtract:        return VK_BLEND_OP_SUBTRACT;
	case EBlendOp::ReverseSubtract: return VK_BLEND_OP_REVERSE_SUBTRACT;
	case EBlendOp::Min:             return VK_BLEND_OP_MIN;
	case EBlendOp::Max:             return VK_BLEND_OP_MAX;
	}

	assert(0);
	return VK_BLEND_OP_MAX_ENUM;
}

VkBlendFactor VulkanMap::blendFactor(EBlendFactor factor)
{
	switch (factor)
	{
	case EBlendFactor::Zero:             return VK_BLEND_FACTOR_ZERO;
	case EBlendFactor::One:              return VK_BLEND_FACTOR_ONE;
	case EBlendFactor::Constant:         return VK_BLEND_FACTOR_CONSTANT_COLOR;
	case EBlendFactor::InverseConstant:  return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
	case EBlendFactor::SrcAlpha:         return VK_BLEND_FACTOR_SRC_ALPHA;
	case EBlendFactor::InverseSrcAlpha:  return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	case EBlendFactor::DstAlpha:         return VK_BLEND_FACTOR_DST_ALPHA;
	case EBlendFactor::InverseDstAlpha:  return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
	case EBlendFactor::SrcAlphaSaturate: return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
	case EBlendFactor::Src1Alpha:        return VK_BLEND_FACTOR_SRC1_ALPHA;
	case EBlendFactor::InverseSrc1Alpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
	case EBlendFactor::SrcColor:         return VK_BLEND_FACTOR_SRC_COLOR;
	case EBlendFactor::InverseSrcColor:  return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
	case EBlendFactor::DstColor:         return VK_BLEND_FACTOR_DST_COLOR;
	case EBlendFactor::InverseDstColor:  return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
	case EBlendFactor::Src1Color:        return VK_BLEND_FACTOR_SRC1_COLOR;
	case EBlendFactor::InverseSrc1Color: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
	}

	assert(0);
	return VK_BLEND_FACTOR_MAX_ENUM;
}

//VkColorComponentFlags VulkanMap::toColorComponentFlags(uint32_t colorMask)
//{
//	if (colorMask == eColorNone)
//	{
//		return 0u;
//	}
//
//	if (colorMask == eColorAll)
//	{
//		return (VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
//	}
//
//	VkColorComponentFlags flags = 0u;
//	if (colorMask & eColorRed)
//	{
//		flags |= VK_COLOR_COMPONENT_R_BIT;
//	}
//	if (colorMask & eColorGreen)
//	{
//		flags |= VK_COLOR_COMPONENT_G_BIT;
//	}
//	if (colorMask & eColorBlue)
//	{
//		flags |= VK_COLOR_COMPONENT_B_BIT;
//	}
//	if (colorMask & eColorAlpha)
//	{
//		flags |= VK_COLOR_COMPONENT_A_BIT;
//	}
//
//	assert(0);
//	return flags;
//}

VkCompareOp VulkanMap::compareFunc(ECompareFunc func)
{
	switch (func)
	{
	case ECompareFunc::Never:          return VK_COMPARE_OP_NEVER;
	case ECompareFunc::Less:           return VK_COMPARE_OP_LESS;
	case ECompareFunc::Equal:          return VK_COMPARE_OP_EQUAL;
	case ECompareFunc::LessOrEqual:    return VK_COMPARE_OP_LESS_OR_EQUAL;
	case ECompareFunc::Greater:        return VK_COMPARE_OP_GREATER;
	case ECompareFunc::NotEqual:       return VK_COMPARE_OP_NOT_EQUAL;
	case ECompareFunc::GreaterOrEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
	case ECompareFunc::Always:         return VK_COMPARE_OP_ALWAYS;
	}
	
	assert(0);
	return VK_COMPARE_OP_MAX_ENUM;
}

VkStencilOp VulkanMap::stencilOp(EStencilOp op)
{
	switch (op)
	{
	case EStencilOp::Keep:              return VK_STENCIL_OP_KEEP;
	case EStencilOp::Zero:              return VK_STENCIL_OP_ZERO;
	case EStencilOp::Replace:           return VK_STENCIL_OP_REPLACE;
	case EStencilOp::IncrementAndClamp: return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
	case EStencilOp::DecrementAndClamp: return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
	case EStencilOp::Invert:            return VK_STENCIL_OP_INVERT;
	case EStencilOp::IncrementAndWrap:  return VK_STENCIL_OP_INCREMENT_AND_WRAP;
	case EStencilOp::DecrementAndWrap:  return VK_STENCIL_OP_DECREMENT_AND_WRAP;
	}

	assert(0);
	return VK_STENCIL_OP_MAX_ENUM;
}

VkFormat VulkanMap::format(EFormat format)
{
	return static_cast<VkFormat>(FormatAttribute::attribute(format).VulkanFormat);
}

VkShaderStageFlagBits VulkanMap::shaderStage(EShaderStage stage)
{
	switch (stage)
	{
	case EShaderStage::Vertex:   return VK_SHADER_STAGE_VERTEX_BIT;
	case EShaderStage::Hull:     return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	case EShaderStage::Domain:   return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	case EShaderStage::Geometry: return VK_SHADER_STAGE_GEOMETRY_BIT;
	case EShaderStage::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
	case EShaderStage::Compute:  return VK_SHADER_STAGE_COMPUTE_BIT;
	}

	assert(0);
	return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
}

VkPrimitiveTopology VulkanMap::primitiveTopology(EPrimitiveTopology primitiveTopology)
{
	switch (primitiveTopology)
	{
	case EPrimitiveTopology::PointList:        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	case EPrimitiveTopology::LineList:         return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	case EPrimitiveTopology::LineStrip:        return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	case EPrimitiveTopology::TriangleList:     return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	case EPrimitiveTopology::TriangleStrip:    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	case EPrimitiveTopology::LineListAdj:      return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
	case EPrimitiveTopology::LineStripAdj:     return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
	case EPrimitiveTopology::TriangleListAdj:  return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
	case EPrimitiveTopology::TriangleStripAdj: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
	case EPrimitiveTopology::PatchList:        return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
	}

	assert(0);
	return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
}

VkDescriptorType VulkanMap::descriptorType(ShaderResource::EResourceType type)
{
	switch (type)
	{
	case ShaderResource::EResourceType::Sampler:                return VK_DESCRIPTOR_TYPE_SAMPLER;
	case ShaderResource::EResourceType::CombinedTextureSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	case ShaderResource::EResourceType::Texture:				return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	case ShaderResource::EResourceType::UniformBuffer:          return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	case ShaderResource::EResourceType::StorageBuffer:          return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	}

	assert(0);
	return VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

VkFilter VulkanMap::textureFilter(ETextureFilter filter)
{
	switch (filter)
	{
	case ETextureFilter::Nearest:  return VK_FILTER_NEAREST;
	case ETextureFilter::Linear:   return VK_FILTER_LINEAR;
	case ETextureFilter::Anisotropic:
		assert(0);
	}

	assert(0);
	return VK_FILTER_MAX_ENUM;
}

VkSamplerAddressMode VulkanMap::samplerAddressMode(ESamplerAddressMode addressMode)
{
	switch (addressMode)
	{
	case ESamplerAddressMode::Repeat:            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	case ESamplerAddressMode::MirroredRepeat:    return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	case ESamplerAddressMode::ClampToEdge:       return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	case ESamplerAddressMode::ClampToBorder:     return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	case ESamplerAddressMode::MirrorClampToEdge: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
	}

	assert(0);
	return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
}

VkBorderColor VulkanMap::borderColor(EBorderColor borderColor)
{
	switch (borderColor)
	{
	case EBorderColor::FloatTransparentBlack: return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
	case EBorderColor::IntTransparentBlack:   return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
	case EBorderColor::FloatOpaqueBlack:      return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
	case EBorderColor::IntOpaqueBlack:        return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	case EBorderColor::FloatOpaqueWhite:      return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	case EBorderColor::IntOpaqueWhite:        return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
	}

	assert(0);
	return VK_BORDER_COLOR_MAX_ENUM;
}

VkImageType VulkanMap::imageType(ETextureType type)
{
	switch (type)
	{
	case ETextureType::T_1D:
	case ETextureType::T_1D_Array:
		return VK_IMAGE_TYPE_1D;
	case ETextureType::T_2D:
	case ETextureType::T_2D_Array:
	case ETextureType::T_Cube:
	case ETextureType::T_Cube_Array:
		return VK_IMAGE_TYPE_2D;
	case ETextureType::T_3D:
		return VK_IMAGE_TYPE_3D;
	}

	assert(0);
	return VK_IMAGE_TYPE_MAX_ENUM;
}

VkImageViewType VulkanMap::imageViewType(ETextureType type)
{
	switch (type)
	{
	case ETextureType::T_1D:         return VK_IMAGE_VIEW_TYPE_1D;
	case ETextureType::T_1D_Array:   return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
	case ETextureType::T_2D:         return VK_IMAGE_VIEW_TYPE_2D;
	case ETextureType::T_2D_Array:   return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
	case ETextureType::T_Cube:       return VK_IMAGE_VIEW_TYPE_CUBE;
	case ETextureType::T_Cube_Array: return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
	case ETextureType::T_3D:         return VK_IMAGE_VIEW_TYPE_3D;
	}

	assert(0);
	return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
}

VkSampleCountFlagBits VulkanMap::samplerCount(uint32_t sampleCount)
{
	switch (sampleCount)
	{
	case 1u:  return VK_SAMPLE_COUNT_1_BIT;
	case 2u:  return VK_SAMPLE_COUNT_2_BIT;
	case 4u:  return VK_SAMPLE_COUNT_4_BIT;
	case 8u:  return VK_SAMPLE_COUNT_8_BIT;
	case 16u: return VK_SAMPLE_COUNT_16_BIT;
	case 32u: return VK_SAMPLE_COUNT_32_BIT;
	case 64u: return VK_SAMPLE_COUNT_64_BIT;
	}

	return VK_SAMPLE_COUNT_1_BIT;
}

NAMESPACE_END(Gfx)
