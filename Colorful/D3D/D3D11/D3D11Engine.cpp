#include "D3D11Engine.h"

void d3d11Engine::initialize(uint64_t windowHandle, const appConfig &config)
{
	uint32_t flags = 0u;
#if defined(_DEBUG)
	flags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	dxgiFactory7 tempDxgiFactory;
	IDXGIFactory7 *pFactory = nullptr;
	rVerifyD3D11(CreateDXGIFactory2(flags, __uuidof(IDXGIFactory7), reinterpret_cast<void **>(&pFactory)));
	tempDxgiFactory.reset(pFactory);

	m_Device.create(m_IMContext, tempDxgiFactory);

	m_Swapchain.create(
		windowHandle,
		config.WindowWidth,
		config.WindowHeight,
		config.VSync,
		config.FullScreen,
		config.D3DTripleBuffer,
		tempDxgiFactory,
		m_Device);
}

void d3d11Engine::logError(uint32_t result) const
{
	std::string errorMsg;
	switch (result)
	{
	case D3D11_ERROR_FILE_NOT_FOUND:
		errorMsg = "The file was not found.";
		break;
	case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
		errorMsg = "There are too many unique instances of a particular type of state object.";
		break;
	case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS:
		errorMsg = "There are too many unique instances of a particular type of view object.";
		break;
	case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD:
		errorMsg = "The first call to ID3D11DeviceContext::Map after either "
			"ID3D11Device::CreateDeferredContext or ID3D11DeviceContext::FinishCommandList per Resource "
			"was not D3D11_MAP_WRITE_DISCARD.";
		break;
	case DXGI_ERROR_INVALID_CALL:
		errorMsg = "The method call is invalid. For example, a method's parameter may not be a valid pointer.";
		break;
	case DXGI_ERROR_WAS_STILL_DRAWING:
		errorMsg = "The previous blit operation that is transferring information to or from this surface is incomplete.";
		break;
	case E_FAIL:
		errorMsg = "Attempted to create a device with the debug layer enabled and the layer is not installed.";
		break;
	case E_INVALIDARG:
		errorMsg = "An invalid parameter was passed to the returning function.";
		break;
	case E_OUTOFMEMORY:
		errorMsg = "Direct3D could not allocate sufficient memory to complete the call.";
		break;
	case E_NOTIMPL:
		errorMsg = "The method call isn't implemented with the passed parameter combination.";
		break;
	case S_FALSE:
		errorMsg = "Alternate success value, indicating a successful but nonstandard completion "
			"(the precise meaning depends on context).";
		break;
	case S_OK:
		errorMsg = "Successed.";
		break;
	default:
		errorMsg = "Unknown error code.";
		assert(0);
		break;
	}

	logger::instance().log(logger::eError, "Failed to invoke D3D11API, error message: %s", errorMsg.c_str());
	assert(0);
}

void d3d11Engine::finalize()
{
#if 0
	ID3D11Debug *debugLayerPtr = nullptr;
	m_Device->QueryInterface(__uuidof(ID3D11Debug), (void**)(&debugLayerPtr));
	if (debugLayerPtr)
	{
		debugLayerPtr->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		safeRelease(debugLayerPtr);
	}
#endif
}

D3D11_FILL_MODE d3d11Engine::enumTranslator::toPolygonMode(eRPolygonMode mode)
{
	switch (mode)
	{
	case eSolid:     return D3D11_FILL_SOLID;
	case eWireframe: return D3D11_FILL_WIREFRAME;
	case ePoint:     return D3D11_FILL_WIREFRAME;  /// ???
	}
	return (D3D11_FILL_MODE)-1;
}

D3D11_CULL_MODE d3d11Engine::enumTranslator::toCullMode(eRCullMode mode)
{
	switch (mode)
	{
	case eCullNone:      return D3D11_CULL_NONE;
	case eCullFrontFace: return D3D11_CULL_FRONT;
	case eCullBackFace:  return D3D11_CULL_BACK;
	}
	return (D3D11_CULL_MODE)-1;
}

D3D11_BLEND_OP d3d11Engine::enumTranslator::toBlendOp(eRBlendOp op)
{
	switch (op)
	{
	case eRBlendOp::eAdd:             return D3D11_BLEND_OP_ADD;
	case eRBlendOp::eSubtract:        return D3D11_BLEND_OP_SUBTRACT;
	case eRBlendOp::eReverseSubtract: return D3D11_BLEND_OP_REV_SUBTRACT;
	case eRBlendOp::eMin:             return D3D11_BLEND_OP_MIN;
	case eRBlendOp::eMax:             return D3D11_BLEND_OP_MAX;
	}
	return (D3D11_BLEND_OP)-1;
}

D3D11_BLEND d3d11Engine::enumTranslator::toBlendFactor(eRBlendFactor factor)
{
	switch (factor)
	{
	case eRBlendFactor::eZero:             return D3D11_BLEND_ZERO;
	case eRBlendFactor::eOne:              return D3D11_BLEND_ONE;
	case eRBlendFactor::eConstant:         return D3D11_BLEND_BLEND_FACTOR;
	case eRBlendFactor::eInverseConstant:  return D3D11_BLEND_INV_BLEND_FACTOR;
	case eRBlendFactor::eSrcAlpha:         return D3D11_BLEND_SRC_ALPHA;
	case eRBlendFactor::eInverseSrcAlpha:  return D3D11_BLEND_INV_SRC_ALPHA;
	case eRBlendFactor::eDstAlpha:         return D3D11_BLEND_DEST_ALPHA;
	case eRBlendFactor::eInverseDstAlpha:  return D3D11_BLEND_INV_DEST_ALPHA;
	case eRBlendFactor::eSrcAlphaSaturate: return D3D11_BLEND_SRC_ALPHA_SAT;
	case eRBlendFactor::eSrc1Alpha:        return D3D11_BLEND_SRC1_ALPHA;
	case eRBlendFactor::eInverseSrc1Alpha: return D3D11_BLEND_INV_SRC1_ALPHA;
	case eRBlendFactor::eSrcColor:         return D3D11_BLEND_SRC_COLOR;
	case eRBlendFactor::eInverseSrcColor:  return D3D11_BLEND_INV_SRC_COLOR;
	case eRBlendFactor::eDstColor:         return D3D11_BLEND_DEST_COLOR;
	case eRBlendFactor::eInverseDstColor:  return D3D11_BLEND_INV_DEST_COLOR;
	case eRBlendFactor::eSrc1Color:        return D3D11_BLEND_SRC1_COLOR;
	case eRBlendFactor::eInverseSrc1Color: return D3D11_BLEND_INV_SRC1_COLOR;
	}
	return (D3D11_BLEND)-1;
}

uint8_t d3d11Engine::enumTranslator::toColorComponentFlags(uint32_t colorMask)
{
	if (colorMask == eColorNone)
	{
		return 0u;
	}

	if (colorMask == eColorAll)
	{
		return D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	uint8_t flags = 0u;
	if (colorMask & eColorRed)
	{
		flags |= D3D11_COLOR_WRITE_ENABLE_RED;
	}
	if (colorMask & eColorGreen)
	{
		flags |= D3D11_COLOR_WRITE_ENABLE_GREEN;
	}
	if (colorMask & eColorBlue)
	{
		flags |= D3D11_COLOR_WRITE_ENABLE_BLUE;
	}
	if (colorMask & eColorAlpha)
	{
		flags |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
	}
	return flags;
}

D3D11_COMPARISON_FUNC d3d11Engine::enumTranslator::toCompareOp(eRCompareOp op)
{
	switch (op)
	{
	case eRCompareOp::eNever:          return D3D11_COMPARISON_NEVER;
	case eRCompareOp::eLess:           return D3D11_COMPARISON_LESS;
	case eRCompareOp::eEqual:          return D3D11_COMPARISON_EQUAL;
	case eRCompareOp::eLessOrEqual:    return D3D11_COMPARISON_LESS_EQUAL;
	case eRCompareOp::eGreater:        return D3D11_COMPARISON_GREATER;
	case eRCompareOp::eNotEqual:       return D3D11_COMPARISON_NOT_EQUAL;
	case eRCompareOp::eGreaterOrEqual: return D3D11_COMPARISON_GREATER_EQUAL;
	case eRCompareOp::eAlways:         return D3D11_COMPARISON_ALWAYS;
	}
	return (D3D11_COMPARISON_FUNC)-1;
}

D3D11_STENCIL_OP d3d11Engine::enumTranslator::toStencilOp(eRStencilOp op)
{
	switch (op)
	{
	case eRStencilOp::eKeep:              return D3D11_STENCIL_OP_KEEP;
	case eRStencilOp::eZero:              return D3D11_STENCIL_OP_ZERO;
	case eRStencilOp::eReplace:           return D3D11_STENCIL_OP_REPLACE;
	case eRStencilOp::eIncrementAndClamp: return D3D11_STENCIL_OP_INCR_SAT;
	case eRStencilOp::eDecrementAndClamp: return D3D11_STENCIL_OP_DECR_SAT;
	case eRStencilOp::eInvert:            return D3D11_STENCIL_OP_INVERT;
	case eRStencilOp::eIncrementAndWrap:  return D3D11_STENCIL_OP_INCR;
	case eRStencilOp::eDecrementAndWrap:  return D3D11_STENCIL_OP_DECR;
	}
	return (D3D11_STENCIL_OP)-1;
}
