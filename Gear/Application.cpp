#include "Gear/Application.h"
#include "Applications/Resource.h"
#include "Colorful/Vulkan/VulkanEngine.h"
#include "AssetTool/AssetDatabase.h"

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
		m_GfxEngine = std::make_unique<VulkanEngine>();
		assert(m_GfxEngine);
	}
	
	m_GfxEngine->initialize(m_Window->handle(), m_Config);
	m_GfxEngine->registerRenderFrameCallback(std::bind(&Application::renderFrame, this));
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

void Application::loop()
{
	m_CpuTimer.start();

	while (true)
	{
		auto &message = m_Window->message();
		if (message.State == eWindowState::eDestroy)
		{
			break;
		}
		if (message.State == eWindowState::eInactive)
		{
			m_CpuTimer.stop();
			Gear::sleep(100u);
		}
		if (message.State == eWindowState::eResized)
		{
			Logger::instance().log(Logger::eInfo, "Window resized Width = %d, Height = %d.\n", m_Window->width(), m_Window->height());
			m_GfxEngine->handleWindowResize(m_Window->width(), m_Window->height());
		}
		if (message.State == eWindowState::eActive)
		{
			m_CpuTimer.start();

			m_CpuTimer.tick();

			m_Camera.processMessage(message, m_CpuTimer.elapsedTime());

			m_GfxEngine->processMessage(message, m_Window->width(), m_Window->height());

			m_GfxEngine->renderFrame();
		}

		m_Window->update();
	}
}

void Application::finalize()
{
	m_GfxEngine->finalize();
}

namespaceEnd(Gear)
