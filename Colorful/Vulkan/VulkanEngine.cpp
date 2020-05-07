#include "VulkanEngine.h"
#include "VulkanTexture.h"
#include "VulkanShader.h"
#include "VulkanBuffer.h"

void vkEngine::initialize(uint64_t windowHandle, const Configurations &config)
{
#if defined(UsingVkLoader)
	vkLoader::initializeGlobalFunctionTable();
#endif

	m_Instance.create();

#if defined(UsingVkLoader)
	vkLoader::initializeInstanceFunctionTable(m_Instance);
#endif

#if defined(_DEBUG)
	m_DebugUtilsMessenger.create(m_Instance, config.VulkanValidationVerbose);
#endif

	auto physicalDevices = vkPhysicalDevice::enumeratePhysicalDevices(m_Instance);
	assert(physicalDevices.size() == 1u); /// Only allow single video card for now...

	uint32_t graphicsQueueFamilyIndex = std::numeric_limits<uint32_t>().max();
	uint32_t computeQueueFamilyIndex = std::numeric_limits<uint32_t>().max();
	uint32_t transferQueueFamilyIndex = std::numeric_limits<uint32_t>().max();
	uint32_t gpuIndex = m_Device.create(
		physicalDevices,
		graphicsQueueFamilyIndex,
		computeQueueFamilyIndex,
		transferQueueFamilyIndex);
	assert(graphicsQueueFamilyIndex != std::numeric_limits<uint32_t>().max());

	m_GraphicsQueue.create(graphicsQueueFamilyIndex, m_Device);
	if (computeQueueFamilyIndex != std::numeric_limits<uint32_t>().max())
	{
		m_ComputeQueue.create(computeQueueFamilyIndex, m_Device);
	}
	if (transferQueueFamilyIndex != std::numeric_limits<uint32_t>().max())
	{
		m_TransferQueue.create(transferQueueFamilyIndex, m_Device);
	}

	assert(gpuIndex != std::numeric_limits<uint32_t>().max());
	m_PhysicalDevice = physicalDevices[gpuIndex];

	m_Swapchain.create(
		getAppInstance(),
		windowHandle,
		config.WindowWidth,
		config.WindowHeight,
		config.VSync,
		config.FullScreen,
		m_Instance,
		m_PhysicalDevice,
		m_Device);
}

void vkEngine::logError(uint32_t result)
{
	std::string errorMsg;

#define vkResultCaseMessage(enumValue)               \
case enumValue:                                      \
	errorMsg = std::string(enumToString(enumValue)); \
	break;

	switch (result)
	{
		vkResultCaseMessage(VK_SUCCESS);
		vkResultCaseMessage(VK_NOT_READY);
		vkResultCaseMessage(VK_TIMEOUT);
		vkResultCaseMessage(VK_EVENT_SET);
		vkResultCaseMessage(VK_EVENT_RESET);
		vkResultCaseMessage(VK_INCOMPLETE);
		vkResultCaseMessage(VK_ERROR_OUT_OF_HOST_MEMORY);
		vkResultCaseMessage(VK_ERROR_OUT_OF_DEVICE_MEMORY);
		vkResultCaseMessage(VK_ERROR_INITIALIZATION_FAILED);
		vkResultCaseMessage(VK_ERROR_DEVICE_LOST);
		vkResultCaseMessage(VK_ERROR_MEMORY_MAP_FAILED);
		vkResultCaseMessage(VK_ERROR_LAYER_NOT_PRESENT);
		vkResultCaseMessage(VK_ERROR_EXTENSION_NOT_PRESENT);
		vkResultCaseMessage(VK_ERROR_FEATURE_NOT_PRESENT);
		vkResultCaseMessage(VK_ERROR_INCOMPATIBLE_DRIVER);
		vkResultCaseMessage(VK_ERROR_TOO_MANY_OBJECTS);
		vkResultCaseMessage(VK_ERROR_FORMAT_NOT_SUPPORTED);
		vkResultCaseMessage(VK_ERROR_FRAGMENTED_POOL);
		vkResultCaseMessage(VK_ERROR_OUT_OF_POOL_MEMORY);
		vkResultCaseMessage(VK_ERROR_INVALID_EXTERNAL_HANDLE);
		vkResultCaseMessage(VK_ERROR_SURFACE_LOST_KHR);
		vkResultCaseMessage(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
		vkResultCaseMessage(VK_SUBOPTIMAL_KHR);
		vkResultCaseMessage(VK_ERROR_OUT_OF_DATE_KHR);
		vkResultCaseMessage(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
		vkResultCaseMessage(VK_ERROR_VALIDATION_FAILED_EXT);
		vkResultCaseMessage(VK_ERROR_INVALID_SHADER_NV);
		vkResultCaseMessage(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
		vkResultCaseMessage(VK_ERROR_FRAGMENTATION_EXT);
		vkResultCaseMessage(VK_ERROR_NOT_PERMITTED_EXT);
		vkResultCaseMessage(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT);
		default:
			errorMsg = "Unknown error code.";
			assert(0);
			break;
	}

#undef vkResultCaseMessage

	Logger::instance().log(Logger::eError, "Failed to invoke VulkanAPI, VKResult = %s", errorMsg.c_str());
	assert(0);
}

void vkEngine::present()
{
	m_GraphicsQueue.submit(m_Swapchain);
}

void vkEngine::waitDone(vkCommandBuffer *cmdBuffer)
{
	cmdBuffer->waitFence(m_Device);
	if (cmdBuffer->m_State == vkCommandBuffer::ePending)
	{
		cmdBuffer->setState(vkCommandBuffer::eExecutable);
	}
}

void vkEngine::createOpaqueRenderPass()
{
	GfxFrameBufferDesc frameBufferDesc{};
	frameBufferDesc.Width = m_Swapchain.getWidth();
	frameBufferDesc.Height = m_Swapchain.getHeight();
	frameBufferDesc.DepthSurface = createDepthStencilView(frameBufferDesc.Width, frameBufferDesc.Height, eD24_UNorm_S8_UInt);

	m_OpaqueRenderPass = m_Device.createRenderPass(m_Swapchain, frameBufferDesc);
}

VkFormatProperties vkEngine::getFormatProperties(VkFormat format)
{
	VkFormatProperties formatProperties{};
	vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice.Handle, format, &formatProperties);

	return formatProperties;
}

void vkEngine::finalize()
{
	m_Device.waitIdle();

	m_GraphicsQueue.destroy(m_Device);
	m_ComputeQueue.destroy(m_Device);
	m_TransferQueue.destroy(m_Device);

	m_Swapchain.destroy(m_Instance, m_Device);

	m_Device.destroy();

#if defined(DEBUG)
	m_DebugUtilsMessenger.destroy(m_Instance);
#endif

	m_Instance.destroy();

#if defined(UsingVkLoader)
	vkLoader::finalize();
#endif
}

VkPolygonMode vkEngine::enumTranslator::toPolygonMode(eRPolygonMode mode)
{
	switch (mode)
	{
	case eSolid:     return VK_POLYGON_MODE_FILL;
	case eWireframe: return VK_POLYGON_MODE_LINE;
	case ePoint:     return VK_POLYGON_MODE_POINT;
	}
	return VK_POLYGON_MODE_MAX_ENUM;
}

VkCullModeFlags vkEngine::enumTranslator::toCullMode(eRCullMode mode)
{
	switch (mode)
	{
	case eCullNone:      return VK_CULL_MODE_NONE;
	case eCullFrontFace: return VK_CULL_MODE_FRONT_BIT;
	case eCullBackFace:  return VK_CULL_MODE_BACK_BIT;
	///case eCullAll:       return VK_CULL_MODE_FRONT_AND_BACK;
	}
	return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
}

VkFrontFace vkEngine::enumTranslator::toFrontFace(eRFrontFace frontFace)
{
	switch (frontFace)
	{
	case eClockwise:        return VK_FRONT_FACE_CLOCKWISE;
	case eCounterclockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
	}
	return VK_FRONT_FACE_MAX_ENUM;
}

VkLogicOp vkEngine::enumTranslator::toLogicOp(eRLogicOp op)
{
	switch (op)
	{
	case eRLogicOp::eClear:         return VK_LOGIC_OP_CLEAR;
	case eRLogicOp::eAnd:           return VK_LOGIC_OP_AND;
	case eRLogicOp::eAnd_Reverse:   return VK_LOGIC_OP_AND_REVERSE;
	case eRLogicOp::eCopy:          return VK_LOGIC_OP_COPY;
	case eRLogicOp::eAnd_Inverted:  return VK_LOGIC_OP_AND_INVERTED;
	case eRLogicOp::eNo:            return VK_LOGIC_OP_NO_OP;
	case eRLogicOp::eXor:           return VK_LOGIC_OP_XOR;
	case eRLogicOp::eOr:            return VK_LOGIC_OP_OR;
	case eRLogicOp::eNor:           return VK_LOGIC_OP_NOR;
	case eRLogicOp::eEquivalent:    return VK_LOGIC_OP_EQUIVALENT;
	case eRLogicOp::eInvert:        return VK_LOGIC_OP_INVERT;
	case eRLogicOp::eOr_Reverse:    return VK_LOGIC_OP_OR_REVERSE;
	case eRLogicOp::eCopy_Inverted: return VK_LOGIC_OP_COPY_INVERTED;
	case eRLogicOp::eOr_Inverted:   return VK_LOGIC_OP_OR_INVERTED;
	case eRLogicOp::eNand:          return VK_LOGIC_OP_NAND;
	case eRLogicOp::eSet:           return VK_LOGIC_OP_SET;
	}
	return VK_LOGIC_OP_MAX_ENUM;
}

VkBlendOp vkEngine::enumTranslator::toBlendOp(eRBlendOp op)
{
	switch (op)
	{
	case eRBlendOp::eAdd:             return VK_BLEND_OP_ADD;
	case eRBlendOp::eSubtract:        return VK_BLEND_OP_SUBTRACT;
	case eRBlendOp::eReverseSubtract: return VK_BLEND_OP_REVERSE_SUBTRACT;
	case eRBlendOp::eMin:             return VK_BLEND_OP_MIN;
	case eRBlendOp::eMax:             return VK_BLEND_OP_MAX;
	}
	return VK_BLEND_OP_MAX_ENUM;
}

VkBlendFactor vkEngine::enumTranslator::toBlendFactor(eRBlendFactor factor)
{
	switch (factor)
	{
	case eRBlendFactor::eZero:             return VK_BLEND_FACTOR_ZERO;
	case eRBlendFactor::eOne:              return VK_BLEND_FACTOR_ONE;
	case eRBlendFactor::eConstant:         return VK_BLEND_FACTOR_CONSTANT_COLOR;
	case eRBlendFactor::eInverseConstant:  return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
	case eRBlendFactor::eSrcAlpha:         return VK_BLEND_FACTOR_SRC_ALPHA;
	case eRBlendFactor::eInverseSrcAlpha:  return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	case eRBlendFactor::eDstAlpha:         return VK_BLEND_FACTOR_DST_ALPHA;
	case eRBlendFactor::eInverseDstAlpha:  return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
	case eRBlendFactor::eSrcAlphaSaturate: return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
	case eRBlendFactor::eSrc1Alpha:        return VK_BLEND_FACTOR_SRC1_ALPHA;
	case eRBlendFactor::eInverseSrc1Alpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
	case eRBlendFactor::eSrcColor:         return VK_BLEND_FACTOR_SRC_COLOR;
	case eRBlendFactor::eInverseSrcColor:  return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
	case eRBlendFactor::eDstColor:         return VK_BLEND_FACTOR_DST_COLOR;
	case eRBlendFactor::eInverseDstColor:  return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
	case eRBlendFactor::eSrc1Color:        return VK_BLEND_FACTOR_SRC1_COLOR;
	case eRBlendFactor::eInverseSrc1Color: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
	}
	return VK_BLEND_FACTOR_MAX_ENUM;
}

VkColorComponentFlags vkEngine::enumTranslator::toColorComponentFlags(uint32_t colorMask)
{
	if (colorMask == eColorNone)
	{
		return 0u;
	}

	if (colorMask == eColorAll)
	{
		return (VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
	}

	VkColorComponentFlags flags = 0u;
	if (colorMask & eColorRed)
	{
		flags |= VK_COLOR_COMPONENT_R_BIT;
	}
	if (colorMask & eColorGreen)
	{
		flags |= VK_COLOR_COMPONENT_G_BIT;
	}
	if (colorMask & eColorBlue)
	{
		flags |= VK_COLOR_COMPONENT_B_BIT;
	}
	if (colorMask & eColorAlpha)
	{
		flags |= VK_COLOR_COMPONENT_A_BIT;
	}
	return flags;
}

VkCompareOp vkEngine::enumTranslator::toCompareOp(eRCompareOp op)
{
	switch (op)
	{
	case eRCompareOp::eNever:          return VK_COMPARE_OP_NEVER;
	case eRCompareOp::eLess:           return VK_COMPARE_OP_LESS;
	case eRCompareOp::eEqual:          return VK_COMPARE_OP_EQUAL;
	case eRCompareOp::eLessOrEqual:    return VK_COMPARE_OP_LESS_OR_EQUAL;
	case eRCompareOp::eGreater:        return VK_COMPARE_OP_GREATER;
	case eRCompareOp::eNotEqual:       return VK_COMPARE_OP_NOT_EQUAL;
	case eRCompareOp::eGreaterOrEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
	case eRCompareOp::eAlways:         return VK_COMPARE_OP_ALWAYS;
	}
	return VK_COMPARE_OP_MAX_ENUM;
}

VkStencilOp vkEngine::enumTranslator::toStencilOp(eRStencilOp op)
{
	switch (op)
	{
	case eRStencilOp::eKeep:              return VK_STENCIL_OP_KEEP;
	case eRStencilOp::eZero:              return VK_STENCIL_OP_ZERO;
	case eRStencilOp::eReplace:           return VK_STENCIL_OP_REPLACE;
	case eRStencilOp::eIncrementAndClamp: return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
	case eRStencilOp::eDecrementAndClamp: return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
	case eRStencilOp::eInvert:            return VK_STENCIL_OP_INVERT;
	case eRStencilOp::eIncrementAndWrap:  return VK_STENCIL_OP_INCREMENT_AND_WRAP;
	case eRStencilOp::eDecrementAndWrap:  return VK_STENCIL_OP_DECREMENT_AND_WRAP;
	}
	return VK_STENCIL_OP_MAX_ENUM;
}

VkFormat vkEngine::enumTranslator::toFormat(eRFormat format)
{
	switch (format)
	{
	case eUnknownFormat:            return VK_FORMAT_UNDEFINED;
	case eD16_UNorm:                return VK_FORMAT_D16_UNORM;
	case eD24_UNorm_S8_UInt:        return VK_FORMAT_D24_UNORM_S8_UINT;
	case eD32_Float:                return VK_FORMAT_D32_SFLOAT;
	case eRGBA32_Typeless:          return VK_FORMAT_R32G32B32A32_SFLOAT;
	case eRGBA32_Float:             return VK_FORMAT_R32G32B32A32_SFLOAT;
	case eRGBA32_UInt:              return VK_FORMAT_R32G32B32A32_UINT;
	case eRGBA32_SInt:              return VK_FORMAT_R32G32B32A32_SINT;
	case eRGB32_Typeless:           return VK_FORMAT_R32G32B32_SFLOAT;
	case eRGB32_Float:              return VK_FORMAT_R32G32B32_SFLOAT;
	case eRGB32_UInt:               return VK_FORMAT_R32G32B32_UINT;
	case eRGB32_SInt:               return VK_FORMAT_R32G32B32_SINT;
	case eRGBA16_Typeless:          return VK_FORMAT_R16G16B16A16_SFLOAT;
	case eRGBA16_Float:             return VK_FORMAT_R16G16B16A16_SFLOAT;
	case eRGBA16_UNorm:             return VK_FORMAT_R16G16B16A16_UNORM;
	case eRGBA16_UInt:              return VK_FORMAT_R16G16B16A16_UINT;
	case eRGBA16_SNorm:             return VK_FORMAT_R16G16B16A16_SNORM;
	case eRGBA16_SInt:              return VK_FORMAT_R16G16B16A16_SINT;
	case eRG32_Typeless:            return VK_FORMAT_R32G32_SFLOAT;
	case eRG32_Float:               return VK_FORMAT_R32G32_SFLOAT;
	case eRG32_UInt:                return VK_FORMAT_R32G32_UINT;
	case eRG32_SInt:                return VK_FORMAT_R32G32_SINT;
	case eRGBA8_Typeless:           return VK_FORMAT_R8G8B8A8_UNORM;
	case eRGBA8_UNorm:              return VK_FORMAT_R8G8B8A8_UNORM;
	case eRGBA8_UNorm_SRGB:         return VK_FORMAT_R8G8B8A8_SRGB;
	case eRGBA8_UInt:               return VK_FORMAT_R8G8B8A8_UINT;
	case eRGBA8_SNorm:              return VK_FORMAT_R8G8B8A8_SNORM;
	case eRGBA8_SInt:               return VK_FORMAT_R8G8B8A8_SINT;
	case eRG16_Typeless:            return VK_FORMAT_R16G16_SFLOAT;
	case eRG16_Float:               return VK_FORMAT_R16G16_SFLOAT;
	case eRG16_UNorm:               return VK_FORMAT_R16G16_UNORM;
	case eRG16_UInt:                return VK_FORMAT_R16G16_UINT;
	case eRG16_SNorm:               return VK_FORMAT_R16G16_SNORM;
	case eRG16_SInt:                return VK_FORMAT_R16G16_SINT;
	case eR32_Typeless:             return VK_FORMAT_R32_SFLOAT;
	case eR32_Float:                return VK_FORMAT_R32_SFLOAT;
	case eR32_UInt:                 return VK_FORMAT_R32_UINT;
	case eR32_SInt:                 return VK_FORMAT_R32_SINT;
	case eRG8_Typeless:             return VK_FORMAT_R8G8_UNORM;
	case eRG8_UNorm:                return VK_FORMAT_R8G8_UNORM;
	case eRG8_UInt:                 return VK_FORMAT_R8G8_UINT;
	case eRG8_SNorm:                return VK_FORMAT_R8G8_SNORM;
	case eRG8_SInt:                 return VK_FORMAT_R8G8_SINT;
	case eR16_Typeless:             return VK_FORMAT_R16_SFLOAT;
	case eR16_Float:                return VK_FORMAT_R16_SFLOAT;
	case eR16_UNorm:                return VK_FORMAT_R16_UNORM;
	case eR16_UInt:                 return VK_FORMAT_R16_UINT;
	case eR16_SNorm:                return VK_FORMAT_R16_SNORM;
	case eR16_SInt:                 return VK_FORMAT_R16_SINT;
	case eR8_Typeless:              return VK_FORMAT_R8_UNORM;
	case eR8_UNorm:                 return VK_FORMAT_R8_UNORM;
	case eR8_UInt:                  return VK_FORMAT_R8_UINT;
	case eR8_SNorm:                 return VK_FORMAT_R8_SNORM;
	case eR8_SInt:                  return VK_FORMAT_R8_SINT;
	case eBC1_Typeless:             return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
	case eBC1_UNorm:                return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
	case eBC1_UNorm_SRGB:           return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
	case eBC2_Typeless:             return VK_FORMAT_BC2_UNORM_BLOCK;
	case eBC2_UNorm:                return VK_FORMAT_BC2_UNORM_BLOCK;
	case eBC2_UNorm_SRGB:           return VK_FORMAT_BC2_SRGB_BLOCK;
	case eBC3_Typeless:             return VK_FORMAT_BC3_UNORM_BLOCK;
	case eBC3_UNorm:                return VK_FORMAT_BC3_UNORM_BLOCK;
	case eBC3_UNorm_SRGB:           return VK_FORMAT_BC3_SRGB_BLOCK;
	case eBC4_Typeless:             return VK_FORMAT_BC4_UNORM_BLOCK;
	case eBC4_UNorm:                return VK_FORMAT_BC4_UNORM_BLOCK;
	case eBC4_SNorm:                return VK_FORMAT_BC4_SNORM_BLOCK;
	case eBC5_Typeless:             return VK_FORMAT_BC5_UNORM_BLOCK;
	case eBC5_UNorm:                return VK_FORMAT_BC5_UNORM_BLOCK;
	case eBC5_SNorm:                return VK_FORMAT_BC5_SNORM_BLOCK;
	case eB5G6R5_UNorm:             return VK_FORMAT_B5G6R5_UNORM_PACK16;
	case eBGR5A1_UNorm:             return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
	case eBGRA8_UNorm:              return VK_FORMAT_R8G8B8A8_UNORM;
	case eBGRA8_Typeless:           return VK_FORMAT_R8G8B8A8_UNORM;
	case eBGRA8_UNorm_SRGB:         return VK_FORMAT_R8G8B8A8_SRGB;
	case eBC6H_Typeless:            return VK_FORMAT_BC6H_UFLOAT_BLOCK;
	case eBC6H_UF16:                return VK_FORMAT_BC6H_UFLOAT_BLOCK;
	case eBC6H_SF16:                return VK_FORMAT_BC6H_SFLOAT_BLOCK;
	case eBC7_Typeless:             return VK_FORMAT_BC7_UNORM_BLOCK;
	case eBC7_UNorm:                return VK_FORMAT_BC7_UNORM_BLOCK;
	case eBC7_UNorm_SRGB:           return VK_FORMAT_BC7_SRGB_BLOCK;
	case eForce_UInt:               return VK_FORMAT_R8G8B8A8_UINT;
	case eD32_Float_S8X24_Uint:    
	case eR32G8X24_Typeless:       
	case eR32_Float_X8X24_Typeless:
	case eX32_Typeless_G8X24_UInt: 
	case eRGB10A2_Typeless:        
	case eRGB10A2_UNorm:           
	case eRGB10A2_UInt:            
	case eRG11B10_Float:           
	case eR24G8_Typeless:          
	case eR24_UNorm_X8_Typeless:   
	case eX24_Typelesss_G8_UInt:   
	case eA8_UNorm:                
	case eR1_UNorm:                
	case eRGB9E5_SharedXP:         
	case eRG8_BG8_UNorm:           
	case eGR8_GB8_UNorm:           
	case eBGRX8_UNorm:             
	case eRGB10_XR_Bias_A2_UNorm:  
	case eBGRX8_Typeless:          
	case eBGRX8_UNorm_SRGB:        
	case eAYUV:                    
	case eY410:                    
	case eY416:                    
	case eNV12:                    
	case eP010:                    
	case eP016:                    
	case e420_Opaque:              
	case eYUY2:                    
	case eY210:                    
	case eY216:                    
	case eNV11:                    
	case eAI44:                    
	case eIA44:                    
	case eP8:                      
	case eAP8:                     
	case eBGRA4_UNorm:             
	case eP208:                    
	case eV208:                    
	case eV408:
		assert(0);
		break;
	}
	return (VkFormat)-1;
}

VkShaderStageFlagBits vkEngine::enumTranslator::toShaderStage(eRShaderUsage usage)
{
	switch (usage)
	{
	case eVertexShader:   return VK_SHADER_STAGE_VERTEX_BIT;
	case eHullShader:     return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	case eDomainShader:   return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	case eGeometryShader: return VK_SHADER_STAGE_GEOMETRY_BIT;
	case eFragmentShader: return VK_SHADER_STAGE_FRAGMENT_BIT;
	case eComputeShader:  return VK_SHADER_STAGE_COMPUTE_BIT;
	}

	return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
}

VkPrimitiveTopology vkEngine::enumTranslator::toPrimitiveTopology(eRPrimitiveTopology primitiveTopology)
{
	switch (primitiveTopology)
	{
	case ePointList:        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	case eLineList:         return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	case eLineStrip:        return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	case eTriangleList:     return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	case eTriangleStrip:    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	case eLineListAdj:      return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
	case eLineStripAdj:     return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
	case eTriangleListAdj:  return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
	case eTriangleStripAdj: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
	case ePatchList:        return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
	}
	return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
}

VkDescriptorType vkEngine::enumTranslator::toDescriptorType(eRDescriptorType type)
{
	switch (type)
	{
	case eDescriptor_Sampler:                return VK_DESCRIPTOR_TYPE_SAMPLER;
	case eDescriptor_CombinedTextureSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	case eDescriptor_SampledTexture:         return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	case eDescriptor_StorageTexture:         return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	case eDescriptor_UniformBuffer:          return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	case eDescriptor_StorageBuffer:          return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	case eDescriptor_InputAttachment:        return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	}
	return VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

VkFilter vkEngine::enumTranslator::toFilter(eRFilter filter)
{
	switch (filter)
	{
	case eNearest:  return VK_FILTER_NEAREST;
	case eAnisotropic:
	case eLinear:   return VK_FILTER_LINEAR;
	}

	return VK_FILTER_MAX_ENUM;
}

VkSamplerAddressMode vkEngine::enumTranslator::toAddressMode(eRSamplerAddressMode addressMode)
{
	switch (addressMode)
	{
	case eRepeat:            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	case eMirroredRepeat:    return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	case eClampToEdge:       return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	case eClampToBorder:     return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	case eMirrorClampToEdge: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
	}

	return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
}

VkBorderColor vkEngine::enumTranslator::toBorderColor(eRBorderColor borderColor)
{
	switch (borderColor)
	{
	case eFloatTransparentBlack: return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
	case eIntTransparentBlack:   return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
	case eFloatOpaqueBlack:      return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
	case eIntOpaqueBlack:        return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	case eFloatOpaqueWhite:      return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	case eIntOpaqueWhite:        return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
	}

	return VK_BORDER_COLOR_MAX_ENUM;
}
