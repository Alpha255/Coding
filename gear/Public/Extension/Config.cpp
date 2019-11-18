#include "Config.h"
#include "Gear/Public/Extension/File.h"
#include "Gear/Public/Extension/System.h"
#include <ThirdParty/json/single_include/nlohmann/json.hpp>

namespaceStart(gear)

void appConfig::load()
{
	std::string configPath(file::getFileDirectory(getApplicationPath()) + "\\appConfig.json");
	if (file::isFileExists(configPath))
	{
		nlohmann::json configJson;
		std::ifstream filestream(configPath);
		filestream >> configJson;
		filestream.close();

		auto appConfigPtr = configJson.find("appConfig");
		assert(appConfigPtr != configJson.end());

		auto itWidth = appConfigPtr->find("width");
		auto itHeight = appConfigPtr->find("height");
		auto itFullScreen = appConfigPtr->find("fullscreen");
		auto itRenderEngine = appConfigPtr->find("renderEngine");

		if (itWidth != appConfigPtr->end())
		{
			WindowWidth = itWidth.value();
		}
		if (itHeight != appConfigPtr->end())
		{
			WindowHeight = itHeight.value();
		}
		if (itFullScreen != appConfigPtr->end())
		{
			FullScreen = itFullScreen.value();
		}
		if (itRenderEngine != appConfigPtr->end())
		{
			std::string renderEngine = itRenderEngine.value();
			if (renderEngine == "d3d11")
			{
				RenderEngine = eD3D11;
			}
			else if (renderEngine == "vulkan")
			{
				RenderEngine = eVulkan;
			}
		}
	}
	else
	{
		/// Try to get configurations from commandline.
		assert(0);
	}
}

namespaceEnd(gear)
