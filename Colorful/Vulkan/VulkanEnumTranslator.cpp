#include "Colorful/Vulkan/VulkanEnumTranslator.h"

NAMESPACE_START(Gfx)

VkPolygonMode VkEnumTranslator::polygonMode(EPolygonMode mode)
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

VkCullModeFlags VkEnumTranslator::cullMode(ECullMode mode)
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

VkFrontFace VkEnumTranslator::frontFace(EFrontFace frontFace)
{
	switch (frontFace)
	{
	case EFrontFace::Counterclockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
	case EFrontFace::Clockwise:        return VK_FRONT_FACE_CLOCKWISE;
	}

	assert(0);
	return VK_FRONT_FACE_MAX_ENUM;
}

VkLogicOp VkEnumTranslator::logicOp(ELogicOp op)
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

VkBlendOp VkEnumTranslator::blendOp(EBlendOp op)
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

VkBlendFactor VkEnumTranslator::blendFactor(EBlendFactor factor)
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

VkColorComponentFlags VkEnumTranslator::colorComponentFlags(uint32_t colorMask)
{
	VkColorComponentFlags flags = 0u;
	if (colorMask == EColorWriteMask::All)
	{
		return VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	}
	if (colorMask & EColorWriteMask::Red)
	{
		flags |= VK_COLOR_COMPONENT_R_BIT;
	}
	if (colorMask & EColorWriteMask::Green)
	{
		flags |= VK_COLOR_COMPONENT_G_BIT;
	}
	if (colorMask & EColorWriteMask::Blue)
	{
		flags |= VK_COLOR_COMPONENT_B_BIT;
	}
	if (colorMask & EColorWriteMask::Alpha)
	{
		flags |= VK_COLOR_COMPONENT_A_BIT;
	}

	return flags;
}

VkCompareOp VkEnumTranslator::compareFunc(ECompareFunc func)
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

VkStencilOp VkEnumTranslator::stencilOp(EStencilOp op)
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

VkFormat VkEnumTranslator::format(EFormat format)
{
	return static_cast<VkFormat>(FormatAttribute::attribute(format).VulkanFormat);
}

VkShaderStageFlagBits VkEnumTranslator::shaderStage(EShaderStage stage)
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

VkPrimitiveTopology VkEnumTranslator::primitiveTopology(EPrimitiveTopology primitiveTopology)
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

VkDescriptorType VkEnumTranslator::descriptorType(EDescriptorType type)
{
	switch (type)
	{
	case EDescriptorType::Sampler:              return VK_DESCRIPTOR_TYPE_SAMPLER;
	case EDescriptorType::CombinedImageSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	case EDescriptorType::SampledImage:		    return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	case EDescriptorType::StorageImage:         return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	case EDescriptorType::UniformTexelBuffer:   return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
	case EDescriptorType::StorageTexelBuffer:   return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
	case EDescriptorType::UniformBuffer:        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	case EDescriptorType::StorageBuffer:        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	case EDescriptorType::UniformBufferDynamic: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	case EDescriptorType::StorageBufferDynamic: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
	case EDescriptorType::InputAttachment:      return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	}

	assert(0);
	return VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

VkFilter VkEnumTranslator::textureFilter(ETextureFilter filter)
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

VkSamplerAddressMode VkEnumTranslator::samplerAddressMode(ESamplerAddressMode addressMode)
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

VkBorderColor VkEnumTranslator::borderColor(EBorderColor borderColor)
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

VkImageType VkEnumTranslator::imageType(ETextureType type)
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

VkImageViewType VkEnumTranslator::imageViewType(ETextureType type)
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

VkDebugReportObjectTypeEXT VkEnumTranslator::debugReportObjectType(EDescriptorType type)
{
	switch (type)
	{
	case EDescriptorType::SampledImage:
	case EDescriptorType::StorageImage: 
	case EDescriptorType::CombinedImageSampler: return VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT;
	case EDescriptorType::InputAttachment:      return VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT;
	case EDescriptorType::UniformTexelBuffer:
	case EDescriptorType::StorageTexelBuffer:
	case EDescriptorType::StorageBuffer:
	case EDescriptorType::UniformBuffer:
	case EDescriptorType::StorageBufferDynamic:
	case EDescriptorType::UniformBufferDynamic: return VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT;
	case EDescriptorType::Sampler:              return VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT;
	}

	assert(0);
	return VkDebugReportObjectTypeEXT::VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT;
}

VkAttachmentLoadOp VkEnumTranslator::attachmentLoadOp(RenderPassDesc::EAttachmentLoadOp op)
{
	switch (op)
	{
	case RenderPassDesc::EAttachmentLoadOp::Load:     return VK_ATTACHMENT_LOAD_OP_LOAD;
	case RenderPassDesc::EAttachmentLoadOp::Clear:    return VK_ATTACHMENT_LOAD_OP_CLEAR;
	case RenderPassDesc::EAttachmentLoadOp::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	}

	return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
}

VkAttachmentStoreOp VkEnumTranslator::attachmentStoreOp(RenderPassDesc::EAttachmentStoreOp op)
{
	switch (op)
	{
	case RenderPassDesc::EAttachmentStoreOp::Store:    return VK_ATTACHMENT_STORE_OP_STORE;
	case RenderPassDesc::EAttachmentStoreOp::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
	}

	return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
}

VkImageLayout VkEnumTranslator::imageLayout(Texture::EImageLayout layout)
{
	switch (layout)
	{
	case Texture::EImageLayout::Undefined:                             
		return VK_IMAGE_LAYOUT_UNDEFINED;
	}

	assert(0);
	return VK_IMAGE_LAYOUT_MAX_ENUM;
}

VkPipelineStageFlags VkEnumTranslator::pipelineStageFlags(uint64_t flags)
{
	VkPipelineStageFlags stageFlags = VK_PIPELINE_STAGE_NONE_KHR;

	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::TopOfPipe)) stageFlags |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::DrawIndirect)) stageFlags |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::VertexInput)) stageFlags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::VertexShader)) stageFlags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::HullShader)) stageFlags |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::DomainShader)) stageFlags |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::GeometryShader)) stageFlags |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::FragmentShader)) stageFlags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::BeforeEarlyZ)) stageFlags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::AfterEarlyZ)) stageFlags |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::ColorAttachmentOutput)) stageFlags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::ComputeShader)) stageFlags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::Transfer)) stageFlags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::BottomOfPipe)) stageFlags |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::Host)) stageFlags |= VK_PIPELINE_STAGE_HOST_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::AllGraphics)) stageFlags |= VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::AllCommands)) stageFlags |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::TransformFeedback)) stageFlags |= VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::ConditionalRendering)) stageFlags |= VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::AcceleartionStructureBuild)) stageFlags |= VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::RayTracingShader)) stageFlags |= VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::ShadingRateImage)) stageFlags |= VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::TaskShader)) stageFlags |= VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::MeshShader)) stageFlags |= VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::FragmentDensityProcess)) stageFlags |= VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EPipelineStageFlags::CommandPreProcess)) stageFlags |= VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV;

	return stageFlags;
}

VkAccessFlags VkEnumTranslator::accessFlags(uint64_t flags)
{
	VkAccessFlags accessFlags = VK_ACCESS_NONE_KHR;

	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::IndirectCommandRead)) accessFlags |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::IndexRead)) accessFlags |= VK_ACCESS_INDEX_READ_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::VertexRead)) accessFlags |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::UniformRead)) accessFlags |= VK_ACCESS_UNIFORM_READ_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::InputAttachmentRead)) accessFlags |= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::ShaderRead)) accessFlags |= VK_ACCESS_SHADER_READ_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::ShaderWrite)) accessFlags |= VK_ACCESS_SHADER_WRITE_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::ColorAttachmentRead)) accessFlags |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::ColorAttachmentWrite)) accessFlags |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::DepthStencilAttachmentRead)) accessFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::DepthStencilAttachmentWrite)) accessFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::TransferRead)) accessFlags |= VK_ACCESS_TRANSFER_READ_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::TransferWrite)) accessFlags |= VK_ACCESS_TRANSFER_WRITE_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::HostRead)) accessFlags |= VK_ACCESS_HOST_READ_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::HostWrite)) accessFlags |= VK_ACCESS_HOST_WRITE_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::MemoryRead)) accessFlags |= VK_ACCESS_MEMORY_READ_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::MemoryWrite)) accessFlags |= VK_ACCESS_MEMORY_WRITE_BIT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::TransformFeedbackWrite)) accessFlags |= VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::TransformFeedbackCounterRead)) accessFlags |= VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::TransformFeedbackCounterWrite)) accessFlags |= VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::ConditionalRenderingRead)) accessFlags |= VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::ColorAttachmentReadNonCoherent)) accessFlags |= VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::AcclerationStructureRead)) accessFlags |= VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::AcclerationStructureWrite)) accessFlags |= VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::ShadingRateImageRead)) accessFlags |= VK_ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::FragmentDensityMapRead)) accessFlags |= VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::CommandPreProcessRead)) accessFlags |= VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_NV;
	if (flags & static_cast<uint64_t>(RenderPassDesc::EAccessFlags::CommandPreProcessWrite)) accessFlags |= VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV;

	return accessFlags;
}

NAMESPACE_END(Gfx)
