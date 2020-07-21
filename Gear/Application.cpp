#include "Gear/Application.h"
#include "Applications/Resource.h"
#include "Colorful/Vulkan/VulkanEngine.h"
#include "AssetTool/AssetDatabase.h"

GfxEnginePtr g_GfxEngine = nullptr;

namespaceStart(Gear)

void Application::createGfxRenderer()
{
	if (m_Config.RenderEngine == Configurations::eD3D11)
	{
		assert(0);
	}
	else if (m_Config.RenderEngine == Configurations::eD3D12)
	{
		assert(0);
	}
	else if (m_Config.RenderEngine == Configurations::eVulkan)
	{
		g_GfxEngine = std::make_unique<VulkanEngine>();
		assert(g_GfxEngine);
	}
	
	g_GfxEngine->initialize(m_Window->handle(), m_Config);
	g_GfxEngine->registerRenderFrameCallback(std::bind(&Application::renderFrame, this));
}

void Application::initialize(const std::string& windowTitle)
{
	m_Config.load();

	m_Window = std::make_unique<Window>(windowTitle, m_Config.WindowWidth, m_Config.WindowHeight);
	m_Window->setMinSize(640u, 480u);

	AssetTool::AssetDatabase::instance().initialize();

	createGfxRenderer();

	postInitialize();
}

void Application::nextFrame(const WindowMessage& message)
{
	static uint32_t FrameCounter = 0u;
	static float32_t DeltaTime = 0.0f;
	static CpuTimer Timer;

	Timer.start();

	g_GfxEngine->processMessage(message, m_Window->width(), m_Window->height());

	g_GfxEngine->renderFrame();

	++FrameCounter;

	Timer.stop();

	auto frameTime = Timer.elapsedTime();

	if (!g_GfxEngine->isFocusOnUI())
	{
		m_Camera.processMessage(message, m_Profile.FPS == 0.0f ? 0.0f : 1.0f / m_Profile.FPS);
	}

	DeltaTime += frameTime;
	if (DeltaTime > 1.0f)
	{
		m_Profile.FPS = FrameCounter / DeltaTime;
		m_Profile.FrameTime = 1000.0f / m_Profile.FPS;
		DeltaTime = 0.0f;
		FrameCounter = 0u;
	}
}

void Application::loop()
{
	while (true)
	{
		auto &message = m_Window->message();
		if (message.State == eWindowState::eDestroy)
		{
			break;
		}
		else if (message.State == eWindowState::eInactive || message.Minimized)
		{
			sleep(100u);
		}
		else if (message.State == eWindowState::eResized)
		{
			m_Camera.handleWindowResize(m_Window->width(), m_Window->height());
			g_GfxEngine->handleWindowResize(m_Window->width(), m_Window->height());
		}
		else if (message.State == eWindowState::eActive)
		{
			nextFrame(message);
		}

		m_Window->update();
	}
}

void Application::finalize()
{
	g_GfxEngine->finalize();
}

namespaceEnd(Gear)
