#include "D3DEngine.h"
#include "D3DFontFreeType.h"

std::unique_ptr<D3DEngine, std::function<void(D3DEngine *)>> D3DEngine::s_Instance;

void D3DEngine::Initialize(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed)
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
			m_Device.MakeObject(pDevice);
			m_IMContext.MakeObject(pContext);

			m_SwapChain.Create(hWnd, width, height, bWindowed);

			RecreateRenderTargetDepthStencil(width, height);

			D3DStaticState::Initialize();

			m_ContextsMap.emplace(std::make_pair(std::this_thread::get_id(), m_IMContext));

			///D3DFontFreeType::Instance().Initialize(width, height);

			m_Inited = true;

			return;
		}
	}

	assert(!"Failed to initialize D3D device!!!");
}

void D3DEngine::RecreateRenderTargetDepthStencil(uint32_t width, uint32_t height)
{
	assert(m_SwapChain.IsValid());

	ID3D11Texture2D *pTexture2D = nullptr;
	HRCheck(m_SwapChain->GetBuffer(0U, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&pTexture2D)));
	D3DResource surface;
	surface.MakeObject(pTexture2D);
	m_RenderTargetView.Create(surface);

	m_DepthStencilView.Create(eD24_UNorm_S8_UInt, width, height);
}

void D3DEngine::Resize(uint32_t width, uint32_t height)
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

	///D3DFontFreeType::Instance().Resize(width, height);
}

void D3DEngine::RegisterRenderThread(std::thread::id workerThreadID)
{
	uint32_t workerThreadsLimit = std::thread::hardware_concurrency();
	assert(m_ContextsMap.size() < workerThreadsLimit);

	m_ContextsMap.emplace(std::make_pair(workerThreadID, D3DContext()));
	m_ContextsMap[workerThreadID].CreateAsDeferredContext();
}

#if 0
void D3DEngine::DrawTextInPos(const char *pTextContent, uint32_t left, uint32_t top, uint32_t fontSize)
{
	assert(pTextContent);

	D3DFontFreeType::Instance().RenderText(pTextContent, left, top, fontSize);
}
#endif