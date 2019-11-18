#include "Config.h"
#include "Gear/Public/Extension/File.h"
#include "Gear/Public/Extension/System.h"

namespaceStart(gear)

void appConfig::load()
{
	std::string appPath = getApplicationPath();
	std::string rootPath = file::getFileDirectory(appPath);
	std::string configFilePath(rootPath + "\\appConfig.json");
	if (file::isFileExists(configFilePath))
	{
		std::ifstream filestream(configFilePath);
		filestream >> ConfigJson;

		auto appConfigPtr = ConfigJson.find("appConfig");
		assert(appConfigPtr != ConfigJson.end());

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
