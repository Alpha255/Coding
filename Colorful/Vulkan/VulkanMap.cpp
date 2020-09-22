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
	switch (format)
	{
	case EFormat::Unknown:                  return VK_FORMAT_UNDEFINED;
	case EFormat::D16_UNorm:                return VK_FORMAT_D16_UNORM;
	case EFormat::D24_UNorm_S8_UInt:        return VK_FORMAT_D24_UNORM_S8_UINT;
	case EFormat::D32_Float:                return VK_FORMAT_D32_SFLOAT;
	case EFormat::RGBA32_Typeless:          return VK_FORMAT_R32G32B32A32_SFLOAT;
	case EFormat::RGBA32_Float:             return VK_FORMAT_R32G32B32A32_SFLOAT;
	case EFormat::RGBA32_UInt:              return VK_FORMAT_R32G32B32A32_UINT;
	case EFormat::RGBA32_SInt:              return VK_FORMAT_R32G32B32A32_SINT;
	case EFormat::RGB32_Typeless:           return VK_FORMAT_R32G32B32_SFLOAT;
	case EFormat::RGB32_Float:              return VK_FORMAT_R32G32B32_SFLOAT;
	case EFormat::RGB32_UInt:               return VK_FORMAT_R32G32B32_UINT;
	case EFormat::RGB32_SInt:               return VK_FORMAT_R32G32B32_SINT;
	case EFormat::RGBA16_Typeless:          return VK_FORMAT_R16G16B16A16_SFLOAT;
	case EFormat::RGBA16_Float:             return VK_FORMAT_R16G16B16A16_SFLOAT;
	case EFormat::RGBA16_UNorm:             return VK_FORMAT_R16G16B16A16_UNORM;
	case EFormat::RGBA16_UInt:              return VK_FORMAT_R16G16B16A16_UINT;
	case EFormat::RGBA16_SNorm:             return VK_FORMAT_R16G16B16A16_SNORM;
	case EFormat::RGBA16_SInt:              return VK_FORMAT_R16G16B16A16_SINT;
	case EFormat::RG32_Typeless:            return VK_FORMAT_R32G32_SFLOAT;
	case EFormat::RG32_Float:               return VK_FORMAT_R32G32_SFLOAT;
	case EFormat::RG32_UInt:                return VK_FORMAT_R32G32_UINT;
	case EFormat::RG32_SInt:                return VK_FORMAT_R32G32_SINT;
	case EFormat::RGBA8_Typeless:           return VK_FORMAT_R8G8B8A8_UNORM;
	case EFormat::RGBA8_UNorm:              return VK_FORMAT_R8G8B8A8_UNORM;
	case EFormat::RGBA8_UNorm_SRGB:         return VK_FORMAT_R8G8B8A8_SRGB;
	case EFormat::RGBA8_UInt:               return VK_FORMAT_R8G8B8A8_UINT;
	case EFormat::RGBA8_SNorm:              return VK_FORMAT_R8G8B8A8_SNORM;
	case EFormat::RGBA8_SInt:               return VK_FORMAT_R8G8B8A8_SINT;
	case EFormat::RG16_Typeless:            return VK_FORMAT_R16G16_SFLOAT;
	case EFormat::RG16_Float:               return VK_FORMAT_R16G16_SFLOAT;
	case EFormat::RG16_UNorm:               return VK_FORMAT_R16G16_UNORM;
	case EFormat::RG16_UInt:                return VK_FORMAT_R16G16_UINT;
	case EFormat::RG16_SNorm:               return VK_FORMAT_R16G16_SNORM;
	case EFormat::RG16_SInt:                return VK_FORMAT_R16G16_SINT;
	case EFormat::R32_Typeless:             return VK_FORMAT_R32_SFLOAT;
	case EFormat::R32_Float:                return VK_FORMAT_R32_SFLOAT;
	case EFormat::R32_UInt:                 return VK_FORMAT_R32_UINT;
	case EFormat::R32_SInt:                 return VK_FORMAT_R32_SINT;
	case EFormat::RG8_Typeless:             return VK_FORMAT_R8G8_UNORM;
	case EFormat::RG8_UNorm:                return VK_FORMAT_R8G8_UNORM;
	case EFormat::RG8_UInt:                 return VK_FORMAT_R8G8_UINT;
	case EFormat::RG8_SNorm:                return VK_FORMAT_R8G8_SNORM;
	case EFormat::RG8_SInt:                 return VK_FORMAT_R8G8_SINT;
	case EFormat::R16_Typeless:             return VK_FORMAT_R16_SFLOAT;
	case EFormat::R16_Float:                return VK_FORMAT_R16_SFLOAT;
	case EFormat::R16_UNorm:                return VK_FORMAT_R16_UNORM;
	case EFormat::R16_UInt:                 return VK_FORMAT_R16_UINT;
	case EFormat::R16_SNorm:                return VK_FORMAT_R16_SNORM;
	case EFormat::R16_SInt:                 return VK_FORMAT_R16_SINT;
	case EFormat::R8_Typeless:              return VK_FORMAT_R8_UNORM;
	case EFormat::R8_UNorm:                 return VK_FORMAT_R8_UNORM;
	case EFormat::R8_UInt:                  return VK_FORMAT_R8_UINT;
	case EFormat::R8_SNorm:                 return VK_FORMAT_R8_SNORM;
	case EFormat::R8_SInt:                  return VK_FORMAT_R8_SINT;
	case EFormat::BC1_Typeless:             return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
	case EFormat::BC1_UNorm:                return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
	case EFormat::BC1_UNorm_SRGB:           return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
	case EFormat::BC2_Typeless:             return VK_FORMAT_BC2_UNORM_BLOCK;
	case EFormat::BC2_UNorm:                return VK_FORMAT_BC2_UNORM_BLOCK;
	case EFormat::BC2_UNorm_SRGB:           return VK_FORMAT_BC2_SRGB_BLOCK;
	case EFormat::BC3_Typeless:             return VK_FORMAT_BC3_UNORM_BLOCK;
	case EFormat::BC3_UNorm:                return VK_FORMAT_BC3_UNORM_BLOCK;
	case EFormat::BC3_UNorm_SRGB:           return VK_FORMAT_BC3_SRGB_BLOCK;
	case EFormat::BC4_Typeless:             return VK_FORMAT_BC4_UNORM_BLOCK;
	case EFormat::BC4_UNorm:                return VK_FORMAT_BC4_UNORM_BLOCK;
	case EFormat::BC4_SNorm:                return VK_FORMAT_BC4_SNORM_BLOCK;
	case EFormat::BC5_Typeless:             return VK_FORMAT_BC5_UNORM_BLOCK;
	case EFormat::BC5_UNorm:                return VK_FORMAT_BC5_UNORM_BLOCK;
	case EFormat::BC5_SNorm:                return VK_FORMAT_BC5_SNORM_BLOCK;
	case EFormat::B5G6R5_UNorm:             return VK_FORMAT_B5G6R5_UNORM_PACK16;
	case EFormat::BGR5A1_UNorm:             return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
	case EFormat::BGRA8_UNorm:              return VK_FORMAT_R8G8B8A8_UNORM;
	case EFormat::BGRA8_Typeless:           return VK_FORMAT_R8G8B8A8_UNORM;
	case EFormat::BGRA8_UNorm_SRGB:         return VK_FORMAT_R8G8B8A8_SRGB;
	case EFormat::BC6H_Typeless:            return VK_FORMAT_BC6H_UFLOAT_BLOCK;
	case EFormat::BC6H_UF16:                return VK_FORMAT_BC6H_UFLOAT_BLOCK;
	case EFormat::BC6H_SF16:                return VK_FORMAT_BC6H_SFLOAT_BLOCK;
	case EFormat::BC7_Typeless:             return VK_FORMAT_BC7_UNORM_BLOCK;
	case EFormat::BC7_UNorm:                return VK_FORMAT_BC7_UNORM_BLOCK;
	case EFormat::BC7_UNorm_SRGB:           return VK_FORMAT_BC7_SRGB_BLOCK;
	case EFormat::Force_UInt:               return VK_FORMAT_R8G8B8A8_UINT;
	case EFormat::D32_Float_S8X24_Uint:
	case EFormat::R32G8X24_Typeless:
	case EFormat::R32_Float_X8X24_Typeless:
	case EFormat::X32_Typeless_G8X24_UInt:
	case EFormat::RGB10A2_Typeless:
	case EFormat::RGB10A2_UNorm:
	case EFormat::RGB10A2_UInt:
	case EFormat::RG11B10_Float:
	case EFormat::R24G8_Typeless:
	case EFormat::R24_UNorm_X8_Typeless:
	case EFormat::X24_Typelesss_G8_UInt:
	case EFormat::A8_UNorm:
	case EFormat::R1_UNorm:
	case EFormat::RGB9E5_SharedXP:
	case EFormat::RG8_BG8_UNorm:
	case EFormat::GR8_GB8_UNorm:
	case EFormat::BGRX8_UNorm:
	case EFormat::RGB10_XR_Bias_A2_UNorm:
	case EFormat::BGRX8_Typeless:
	case EFormat::BGRX8_UNorm_SRGB:
	case EFormat::AYUV:
	case EFormat::Y410:
	case EFormat::Y416:
	case EFormat::NV12:
	case EFormat::P010:
	case EFormat::P016:
	case EFormat::Opaque_420:
	case EFormat::YUY2:
	case EFormat::Y210:
	case EFormat::Y216:
	case EFormat::NV11:
	case EFormat::AI44:
	case EFormat::IA44:
	case EFormat::P8:
	case EFormat::AP8:
	case EFormat::BGRA4_UNorm:
	case EFormat::P208:
	case EFormat::V208:
	case EFormat::V408:
		assert(0);
		break;
	}

	assert(0);
	return (VkFormat)-1;
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

//VkDescriptorType VulkanMap::descriptorType(eRDescriptorType type)
//{
//	switch (type)
//	{
//	case eDescriptor_Sampler:                return VK_DESCRIPTOR_TYPE_SAMPLER;
//	case eDescriptor_CombinedTextureSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//	case eDescriptor_SampledTexture:         return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
//	case eDescriptor_StorageTexture:         return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
//	case eDescriptor_UniformBuffer:          return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//	case eDescriptor_StorageBuffer:          return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//	case eDescriptor_InputAttachment:        return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
//	}
//
//	assert(0);
//	return VK_DESCRIPTOR_TYPE_MAX_ENUM;
//}

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

NAMESPACE_END(Gfx)
