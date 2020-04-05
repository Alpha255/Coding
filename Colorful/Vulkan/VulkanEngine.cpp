#include "VulkanEngine.h"

void vkEngine::initialize(uint64_t windowHandle, const appConfig &config)
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

	uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
	uint32_t computeQueueFamilyIndex = UINT32_MAX;
	uint32_t transferQueueFamilyIndex = UINT32_MAX;
	uint32_t gpuIndex = m_Device.create(
		physicalDevices,
		graphicsQueueFamilyIndex,
		computeQueueFamilyIndex,
		transferQueueFamilyIndex);
	assert(graphicsQueueFamilyIndex != UINT32_MAX);

	m_GraphicsQueue.create(graphicsQueueFamilyIndex, m_Device);
	if (computeQueueFamilyIndex != UINT32_MAX)
	{
		m_ComputeQueue.create(computeQueueFamilyIndex, m_Device);
	}
	if (transferQueueFamilyIndex != UINT32_MAX)
	{
		m_TransferQueue.create(transferQueueFamilyIndex, m_Device);
	}

	assert(gpuIndex != UINT32_MAX);
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

void vkEngine::logError(uint32_t result) const
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

	logger::instance().log(logger::eError, "Faile to invoke VulkanAPI, VKResult = %s", errorMsg.c_str());
	assert(0);
}

void vkEngine::finalize()
{
	m_Device.waitIdle();

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

VkLogicOp vkEngine::enumTranslator::toLogicOp(eLogicOp op)
{
	switch (op)
	{
	case eLogicOp::eClear:         return VK_LOGIC_OP_CLEAR;
	case eLogicOp::eAnd:           return VK_LOGIC_OP_AND;
	case eLogicOp::eAnd_Reverse:   return VK_LOGIC_OP_AND_REVERSE;
	case eLogicOp::eCopy:          return VK_LOGIC_OP_COPY;
	case eLogicOp::eAnd_Inverted:  return VK_LOGIC_OP_AND_INVERTED;
	case eLogicOp::eNo:            return VK_LOGIC_OP_NO_OP;
	case eLogicOp::eXor:           return VK_LOGIC_OP_XOR;
	case eLogicOp::eOr:            return VK_LOGIC_OP_OR;
	case eLogicOp::eNor:           return VK_LOGIC_OP_NOR;
	case eLogicOp::eEquivalent:    return VK_LOGIC_OP_EQUIVALENT;
	case eLogicOp::eInvert:        return VK_LOGIC_OP_INVERT;
	case eLogicOp::eOr_Reverse:    return VK_LOGIC_OP_OR_REVERSE;
	case eLogicOp::eCopy_Inverted: return VK_LOGIC_OP_COPY_INVERTED;
	case eLogicOp::eOr_Inverted:   return VK_LOGIC_OP_OR_INVERTED;
	case eLogicOp::eNand:          return VK_LOGIC_OP_NAND;
	case eLogicOp::eSet:           return VK_LOGIC_OP_SET;
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
