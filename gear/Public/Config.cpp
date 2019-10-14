#include "Config.h"
#include "gear/Public/Util.h"
#include "gear/System/File.h"
#include <ThirdParty/json/single_include/nlohmann/json.hpp>

namespaceStart(gear)

void appConfig::load()
{
	char8_t currentPath[MAX_PATH] = {};
	verifyWin(::GetModuleFileNameA(nullptr, currentPath, MAX_PATH) != 0);

	std::string rootPath = file::getDirectory(currentPath);
	std::string configFilePath(rootPath + "\\appConfig.json");
	if (file::isExists(configFilePath))
	{
		nlohmann::json configJson;
		std::ifstream filestream(configFilePath);
		filestream >> configJson;

		auto itWidth = configJson.find("width");
		auto itHeight = configJson.find("height");
		auto itFullScreen = configJson.find("fullscreen");

		if (itWidth != configJson.end())
		{
			m_WindowWidth = itWidth.value();
		}
		if (itHeight != configJson.end())
		{
			m_WindowHeight = itHeight.value();
		}
		if (itFullScreen != configJson.end())
		{
			m_bFullScreen = itFullScreen.value();
		}
	}
	else
	{
		assert(0);
	}
}

namespaceEnd(gear)
