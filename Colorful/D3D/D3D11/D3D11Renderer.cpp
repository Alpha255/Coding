#if 0
#include "Colorful/D3D/D3D11/D3D11Engine.h"
#include "AssetTool/AssetDatabase.h"
#include "D3D11Renderer.h"

void D3D11Engine::initialize(uint64_t windowHandle, const Configurations& config)
{
	m_Device.create(m_IMContext, factory);

	m_Swapchain.create(
		windowHandle,
		config.WindowWidth,
		config.WindowHeight,
		config.VSync,
		config.FullScreen,
		config.D3DTripleBuffer,
		factory,
		m_Device);

	GfxFactory::initialize();
	D3D11BufferPool::initialize(m_Device, m_IMContext);
	m_ImGuiRenderer = std::make_unique<ImGuiRenderer>();
}

D3D11GraphicsPipelinePtr D3D11Engine::getOrCreateGfxPipeline(const GfxPipelineState* state)
{
	for (auto& pair : m_GfxPipelines)
	{
		if (pair.first == state)
		{
			return pair.second;
		}
	}

	auto gfxPipeline = std::make_shared<D3D11GraphicsPipeline>(m_Device, state);
	m_GfxPipelines.emplace_back(std::make_pair(state, gfxPipeline));
	return gfxPipeline;
}

void D3D11Engine::prepareForDraw()
{
	assert(m_CurGfxPipeline);
}

void D3D11Engine::finalize()
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

#endif

#include "Colorful/D3D/D3D11/D3D11Renderer.h"

NAMESPACE_START(Gfx)

void D3D11Renderer::createDevice()
{
	DXGIFactoryPtr Factory = std::make_unique<DXGIFactory>();
	DXGIAdapterListPtr AdapterList  = std::make_unique<DXGIAdapterList>(Factory->get(), Factory->get6());

	m_Device = std::make_unique<D3D11Device>(AdapterList->get1(), AdapterList->get4());
}

void D3D11Renderer::createSwapchain(uint64_t instance, uint64_t windowHandle, uint32_t width, uint32_t height, bool8_t fullscreen, bool8_t VSync)
{
	(void)instance;
	(void)windowHandle;
	(void)width;
	(void)height;
	(void)fullscreen;
	(void)VSync;
}

void D3D11Renderer::toggleFullScreen(bool8_t fullscreen)
{
	(void)fullscreen;
}

void D3D11Renderer::toggleVSync(bool8_t VSync)
{
	(void)VSync;
}

void D3D11Renderer::finalize()
{
}

extern "C"
{
	EXPORT_API void createRenderer(GfxRendererPtr& ptr)
	{
		return ptr.reset(new D3D11Renderer());
	}
}

NAMESPACE_END(Gfx)
