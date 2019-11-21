#include "Gear/Public/Extension/Application.h"
#include "Applications/Resource.h"
#include "Colorful/D3D11/D3D11Engine.h"
#include "Colorful/Vulkan/VulkanEngine.h"

rEnginePtr g_rEnginePtr = nullptr;
rDevicePtr g_rDevicePtr = nullptr;

namespaceStart(gear)

void application::initialize(const std::string &title, uint32_t extraWindowStyle)
{
	m_Config.load();

	auto renderEngine = m_Config.RenderEngine;
	if (renderEngine == appConfig::eD3D11)
	{
		makeWindow(title, m_Config.WindowWidth, m_Config.WindowHeight, extraWindowStyle, IconDirectX);
		d3d11Engine::instance().initialize(m_WindowHandle, m_Config);

		g_rEnginePtr.reset(&d3d11Engine::instance());
	}
	else if (renderEngine == appConfig::eVulkan)
	{
		makeWindow(title, m_Config.WindowWidth, m_Config.WindowHeight, extraWindowStyle, IconVulkan);
		vkEngine::instance().initialize(m_WindowHandle, m_Config);

		g_rEnginePtr.reset(&vkEngine::instance());
	}

	g_rDevicePtr = g_rEnginePtr->getDevice();

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
		m_bActive = true;
		m_cpuTimer.start();
		break;
	case eAppEvent::eMaximize:
	case eAppEvent::eMinimize:
		m_bNeedResize = true;
		break;
	case eAppEvent::eResizing_Start:
		m_bActive = false;
		m_cpuTimer.stop();
		m_bNeedResize = true;
		break;
	case eAppEvent::eResizing_End:
		m_cpuTimer.start();
		m_bActive = true;
		break;
	case eAppEvent::eQuit:
		m_bActive = false;
		m_cpuTimer.stop();
		break;
	case eAppEvent::eDoubleClickNonClientArea:
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

	math::rect winRect = getWindowRect(m_WindowHandle);
	m_WindowSize = 
	{ 
		winRect.getWidth(), 
		winRect.getHeight() 
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

namespaceEnd(gear)
