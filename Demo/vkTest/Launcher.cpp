#include "Launcher.h"

std::unordered_map<std::string, std::unique_ptr<application>> launcher::s_Apps;

#define registerMetaClass(ClassName) s_Apps.insert(std::make_pair(#ClassName, std::make_unique<ClassName>()))

void launcher::registerMetaClasses()
{
	///registerMetaClass(vkTest);
	registerMetaClass(vkMultiThread);
}

void launcher::launch()
{
	auto pApp = s_Apps.find(m_Config.getAppName());
	assert(pApp != s_Apps.cend());

	pApp->second->initialize(pApp->first, m_Config.getWindowWidth(), m_Config.getWindowHeight(), false, 0u);
	pApp->second->loop();
	pApp->second->finalize();
}

int32_t WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int32_t)
{
	launcher localLauncer;

	localLauncer.loadConfig();

	localLauncer.registerMetaClasses();

	localLauncer.launch();

	return 0;
}