#include "Config.h"
#include "Gear/Public/Extension/File.h"
#include "Gear/Public/Extension/System.h"
#include <ThirdParty/json/single_include/nlohmann/json.hpp>

namespaceStart(gear)

void appConfig::load()
{
	std::string appPath = gear::getApplicationPath();
	std::string rootPath = file::getFileDirectory(appPath);
	std::string configFilePath(rootPath + "\\appConfig.json");
	if (file::isFileExists(configFilePath))
	{
		nlohmann::json configJson;
		std::ifstream filestream(configFilePath);
		filestream >> configJson;

		auto itWidth = configJson.find("width");
		auto itHeight = configJson.find("height");
		auto itFullScreen = configJson.find("fullscreen");
		auto itRenderEngine = configJson.find("renderEngine");

		if (itWidth != configJson.end())
		{
			WindowWidth = itWidth.value();
		}
		if (itHeight != configJson.end())
		{
			WindowHeight = itHeight.value();
		}
		if (itFullScreen != configJson.end())
		{
			FullScreen = itFullScreen.value();
		}
		if (itRenderEngine != configJson.end())
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
