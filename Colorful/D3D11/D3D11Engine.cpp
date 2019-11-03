#include "D3D11Engine.h"

std::unique_ptr<D3D11Engine, std::function<void(D3D11Engine *)>> D3D11Engine::s_Instance;

void D3D11Engine::Initialize(::HWND hWnd, uint32_t width, uint32_t height, bool bFullScreen)
{
	assert(hWnd && !m_Inited);

	uint32_t flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	std::array<D3D_DRIVER_TYPE, 3> driverTypes =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	std::array<D3D_FEATURE_LEVEL, 3> featureLevels = 
	{ 
		D3D_FEATURE_LEVEL_11_0, 
		D3D_FEATURE_LEVEL_10_1, 
		D3D_FEATURE_LEVEL_10_0 
	};

	ID3D11Device *pDevice = nullptr;
	ID3D11DeviceContext *pContext = nullptr;
	for (size_t index = 0; index < driverTypes.size(); ++index)
	{
		if (SUCCEEDED(D3D11CreateDevice(
			nullptr, 
			driverTypes[index], 
			nullptr, 
			flags, 
			&featureLevels[0], 
			(uint32_t)featureLevels.size(), 
			D3D11_SDK_VERSION, 
			&pDevice,
			nullptr, 
			&pContext)))
		{
			m_Device.Reset(pDevice);
			m_IMContext.Reset(pContext);

			m_SwapChain.Create(hWnd, width, height, bFullScreen);

			RecreateRenderTargetDepthStencil(width, height);

			D3D11StaticState::Initialize();

			m_Inited = true;

			return;
		}
	}

	assert(!"Failed to initialize D3D device!!!");
}

void D3D11Engine::RecreateRenderTargetDepthStencil(uint32_t width, uint32_t height)
{
	assert(m_SwapChain.IsValid());

	ID3D11Texture2D *pTexture2D = nullptr;
	Check(m_SwapChain->GetBuffer(0U, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&pTexture2D)));
	D3D11Resource surface;
	surface.Reset(pTexture2D);
	m_RenderTargetView.Create(surface);

	m_DepthStencilView.Create(eD24_UNorm_S8_UInt, width, height);
}

void D3D11Engine::Resize(uint32_t width, uint32_t height)
{
	assert((width > 0U) && (height > 0U));

	if (width == m_SwapChain.Width() && height == m_SwapChain.Height())
	{
		return;
	}

	m_RenderTargetView.Reset();

	m_DepthStencilView.Reset();

	m_SwapChain.Resize(width, height);

	RecreateRenderTargetDepthStencil(width, height);
}

void d3d11Engine::initialize(::HWND, uint32_t, uint32_t, bool8_t)
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

	gear::log(gear::eError, "Failed to invoke D3D11API, error message: ", errorMsg.c_str());
}
