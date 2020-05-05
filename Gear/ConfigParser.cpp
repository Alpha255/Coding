#include "Gear/Gear.h"
#include <ThirdParty/json/single_include/nlohmann/json.hpp>

namespaceStart(Gear)

void Configurations::load()
{
	std::string configPath(File::directory(getApplicationPath()) + "\\Configurations.json");
	if (File::isExists(configPath))
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
			else if (renderEngine == "d3d12")
			{
				RenderEngine = eD3D12;
			}
			else if (renderEngine == "vulkan")
			{
				RenderEngine = eVulkan;
			}
		}
	}
	else
	{
		/// Try to get configurations from Commandline.
		assert(0);
	}
}

namespaceEnd(Gear)
