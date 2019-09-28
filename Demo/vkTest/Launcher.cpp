#include "Launcher.h"

std::unordered_map<std::string, std::unique_ptr<application>> launcher::s_Apps;

#define registerMetaClass(ClassName) s_Apps.insert(std::make_pair(#ClassName, std::make_unique<ClassName>()))

void launcher::registerMetaClasses()
{
	registerMetaClass(vkTest);
}

void launcher::launch(const std::string &appName)
{
	auto pApp = s_Apps.find(appName);
	assert(pApp != s_Apps.cend());

	pApp->second->initialize(pApp->first, 1280u, 720u, false, 0u);
	pApp->second->loop();
	pApp->second->finalize();
}

int32_t WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int32_t)
{
	launcher localLauncer;

	localLauncer.registerMetaClasses();

	localLauncer.launch("vkTest");

	return 0;
}