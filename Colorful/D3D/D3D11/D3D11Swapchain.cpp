#include "D3D11Swapchain.h"
#include "D3D11Engine.h"

void D3D11Swapchain::create(
	uint64_t windowHandle, 
	uint32_t width, 
	uint32_t height, 
	bool8_t vSync, 
	bool8_t fullscreen, 
	bool8_t tripleBuffer,
	const DXGIFactory7 &inDxgiFactory,
	D3D11Device &device)
{
	assert(!isValid() && windowHandle && inDxgiFactory.isValid() && device.isValid());
	/// Microsoft DirectX Graphics Infrastructure (DXGI)
	/// IDXGISwapChain1: Provides presentation capabilities that are enhanced from IDXGISwapChain. 
	/// These presentation capabilities consist of specifying dirty rectangles and scroll rectangle to optimize the presentation.

	/// IDXGISwapChain2: Extends IDXGISwapChain1 with methods to support swap back buffer scaling and lower-latency swap chains.

	/// IDXGISwapChain3: Extends IDXGISwapChain2 with methods to support getting the index of the swap chain's current back buffer and support for color space.

	m_bVSync = vSync;
	m_bFullScreen = fullscreen;
	m_bTripleBuffer = tripleBuffer;

	Math::Vec2 desktopWindowSize = getDesktopWindowSize();

	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

	auto DXGIAdapter = device.dxgiAdapter();
	assert(DXGIAdapter.isValid());
	std::vector<DXGIOutput6> dxgiOutputs;
	uint32_t dxgiOutputIndex = 0u;
	while (true)
	{
		IDXGIOutput *pDxgiOutput = nullptr;
		if (DXGIAdapter->EnumOutputs(dxgiOutputIndex++, &pDxgiOutput) == DXGI_ERROR_NOT_FOUND)
		{
			break;
		}

		dxgiOutputs.emplace_back();
		dxgiOutputs[dxgiOutputs.size() - 1u].reset(static_cast<IDXGIOutput6 *>(pDxgiOutput));
	}

	std::vector<DXGI_MODE_DESC1> dxgiModeDescs;
	for (uint32_t i = 0u; i < dxgiOutputs.size(); ++i)
	{
		uint32_t count = 0u;
		GfxVerifyD3D(dxgiOutputs[i]->GetDisplayModeList1(format, DXGI_ENUM_MODES_INTERLACED, &count, nullptr));

		std::vector<DXGI_MODE_DESC1> tempDxgiModeDescs(count);
		GfxVerifyD3D(dxgiOutputs[i]->GetDisplayModeList1(format, DXGI_ENUM_MODES_INTERLACED, &count, tempDxgiModeDescs.data()));

		dxgiModeDescs.insert(dxgiModeDescs.end(), tempDxgiModeDescs.cbegin(), tempDxgiModeDescs.cend());
	}

	uint32_t tempWidth = 0u; uint32_t dxgiModeIndex = std::numeric_limits<uint32_t>().max(); float32_t tempRefreshRate = 0.0f;
	for (uint32_t i = 0u; i < dxgiModeDescs.size(); ++i)
	{
		if ((uint32_t)desktopWindowSize.x == dxgiModeDescs[i].Width &&
			(uint32_t)desktopWindowSize.y == dxgiModeDescs[i].Height)
		{
			if (dxgiModeDescs[i].Width >= tempWidth)
			{
				float32_t refreshRate = dxgiModeDescs[i].RefreshRate.Numerator / (float32_t)dxgiModeDescs[i].RefreshRate.Denominator;
				if (refreshRate >= tempRefreshRate)
				{
					tempWidth = dxgiModeDescs[i].Width;
					tempRefreshRate = refreshRate;
					dxgiModeIndex = i;
				}
			}
		}
	}
	assert(dxgiModeIndex != std::numeric_limits<uint32_t>().max());

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc
	{
		dxgiModeDescs[dxgiModeIndex].RefreshRate,
		dxgiModeDescs[dxgiModeIndex].ScanlineOrdering,
		dxgiModeDescs[dxgiModeIndex].Scaling,
		!m_bFullScreen
	};

	DXGI_SWAP_CHAIN_DESC1 desc
	{
		m_bFullScreen ? dxgiModeDescs[dxgiModeIndex].Width : width,
		m_bFullScreen ? dxgiModeDescs[dxgiModeIndex].Height : height,
		format,
		false,
		{
			1u,
			0u,
		},
		DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER,
		tripleBuffer ? 3u : 2u,
		DXGI_SCALING_NONE,
		DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
		DXGI_ALPHA_MODE_UNSPECIFIED,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	};

	IDXGISwapChain1 *pSwapchain = nullptr;
	GfxVerifyD3D(inDxgiFactory->CreateSwapChainForHwnd(
		device.get(),
		(::HWND)windowHandle,
		&desc,
		&fullscreenDesc,
		nullptr,
		&pSwapchain));
	reset(pSwapchain);
}

void D3D11Swapchain::resize(uint32_t width, uint32_t height)
{
	GfxVerifyD3D(m_Object->ResizeBuffers(
		m_bTripleBuffer ? 3u : 2u,
		width, 
		height,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	));
}
