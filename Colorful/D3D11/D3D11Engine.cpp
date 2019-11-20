#include "D3D11Engine.h"

void d3d11Engine::initialize(uint64_t, const appConfig &)
{
	m_Device = std::make_shared<d3d11Device>();
	m_IMContext = std::make_shared<d3d11Context>();

	d3d11DxgiFactoryPtr dxgiFactory = std::make_shared<d3d11DxgiFactory>();
	IDXGIFactory *pFactory = nullptr;
	rVerifyD3D11(CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void **>(&pFactory)));
	dxgiFactory->reset(pFactory);

	m_Device->create(m_IMContext, dxgiFactory);
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

	logger::instance().log(logger::eError, "Failed to invoke D3D11API, error message: ", errorMsg.c_str());
}
