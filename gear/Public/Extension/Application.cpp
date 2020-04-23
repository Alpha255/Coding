#include "Gear/Public/Extension/Application.h"
#include "Applications/Resource.h"
#include "Colorful/Public/RAsset.h"
#include "Colorful/D3D/D3D11/D3D11Engine.h"
#include "Colorful/D3D/D3D12/D3D12Engine.h"
#include "Colorful/Vulkan/VulkanEngine.h"

namespaceStart(gear)

void application::initialize(const std::string &title, uint32_t extraWindowStyle)
{
	m_Config.load();

	auto renderEngine = m_Config.RenderEngine;
	assert(renderEngine != appConfig::eUnknown);

	if (renderEngine == appConfig::eD3D11)
	{
		makeWindow(title, m_Config.WindowWidth, m_Config.WindowHeight, extraWindowStyle, IconDirectX);
		d3d11Engine::instance().initialize(m_WindowHandle, m_Config);

		m_Renderer = &d3d11Engine::instance();
	}
	else if (renderEngine == appConfig::eD3D12)
	{
		makeWindow(title, m_Config.WindowWidth, m_Config.WindowHeight, extraWindowStyle, IconDirectX);
		d3d12Engine::instance().initialize(m_WindowHandle, m_Config);

		m_Renderer = &d3d12Engine::instance();
	}
	else if (renderEngine == appConfig::eVulkan)
	{
		makeWindow(title, m_Config.WindowWidth, m_Config.WindowHeight, extraWindowStyle, IconVulkan);
		vkEngine::instance().initialize(m_WindowHandle, m_Config);

		m_Renderer = &vkEngine::instance();
	}

	rAsset::rAssetBucket::instance().initialize(renderEngine);

	eventHandler::instance().setWindowSizeLimitations(math::vec2(640.0f, 480.0f));

	postInitialize();
}

void application::processEvent()
{
	auto event = eventHandler::instance().getAppEvent();
	switch (event)
	{
	case eAppEvent::eInactive:
		m_bActive = false;
		m_cpuTimer.stop();
		break;
	case eAppEvent::eActive:
	case eAppEvent::eRestore:
		m_bActive = true;
		m_cpuTimer.start();
		break;
	case eAppEvent::eMaximize:
	case eAppEvent::eMinimize:
		m_bActive = false;
		m_bNeedResize = true;
		break;
	case eAppEvent::eResizing:
		m_bActive = false;
		m_cpuTimer.stop();
		m_bNeedResize = true;
		break;
	case eAppEvent::eQuit:
		m_bActive = false;
		m_cpuTimer.stop();
		break;
	case eAppEvent::eDoubleClickNonClientArea:
		m_bActive = false;
		m_bNeedResize = true;
		break;
	}

	m_Camera.processEvent();
}

void application::updateWindow()
{
	if (!m_bNeedResize)
	{
		return;
	}

	math::rect windowRect = getWindowRect(m_WindowHandle);
	m_WindowSize = 
	{ 
		windowRect.getWidth(),
		windowRect.getHeight()
	};

	resizeWindow();

	m_bNeedResize = false;
}

void application::updateFPS()
{
	float32_t totalTime = m_cpuTimer.getTotalTime();
	++m_FrameCount;

	float32_t elapsedTime = totalTime - m_LastUpdateTime;
	if (elapsedTime > 1.0f)
	{
		m_FPS = m_FrameCount / elapsedTime;
		m_LastUpdateTime = totalTime;
		m_FrameCount = 0U;
	}
}

void application::loop()
{
	m_cpuTimer.reset();

	eAppEvent event = eAppEvent::eActive;
	while (eAppEvent::eQuit != event)
	{
		if (!eventHandler::instance().dispatchEvent())
		{
			m_cpuTimer.tick();

			if (m_bActive)
			{
				updateWindow();

				renterToWindow();

				updateFPS();
			}
			else
			{
				sleep(100u);
			}
		}

		event = eventHandler::instance().getAppEvent();
	}
}

void application::finalize()
{
	assert(m_Renderer);
	m_Renderer->finalize();
}

namespaceEnd(gear)
