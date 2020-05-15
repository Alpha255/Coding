#include "D3D12Engine.h"

void d3d12Engine::initialize(uint64_t windowHandle, const Configurations &config)
{
	uint32_t flags = 0u;
	ID3D12Debug *d3d12DebugPtr = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&d3d12DebugPtr))) && d3d12DebugPtr)
	{
		flags |= DXGI_CREATE_FACTORY_DEBUG;
		d3d12DebugPtr->EnableDebugLayer();
	}

	DXGIFactory7 tempDxgiFactory;
	IDXGIFactory7 *pFactory = nullptr;
	rVerifyD3D12(CreateDXGIFactory2(flags, __uuidof(IDXGIFactory7), reinterpret_cast<void **>(&pFactory)));
	tempDxgiFactory.reset(pFactory);

	m_Device.create(tempDxgiFactory);

	m_Swapchain.create(
		windowHandle,
		config.WindowWidth,
		config.WindowHeight,
		config.VSync,
		config.FullScreen,
		config.D3DTripleBuffer,
		tempDxgiFactory,
		m_Device);

	safeRelease(d3d12DebugPtr);
}

void d3d12Engine::logError(uint32_t result)
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

	Logger::instance().log(Logger::eError, "Failed to invoke D3D12API, error message: %s", errorMsg.c_str());
	assert(0);
}

void d3d12Engine::finalize()
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
