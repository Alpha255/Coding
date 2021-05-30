#pragma once

#include "Gear/Gear.h"
#include <ThirdParty/json/single_include/nlohmann/json.hpp>

NAMESPACE_START(Gear)

struct Configurations
{
	Math::Vec2 WindowSize;
	Math::Vec2 MinWindowSize;
	bool8_t FullScreen = false;
	bool8_t VSync = false;

	std::string RendererName;
	std::string WorkingDirectory;

	Configurations()
	{
		WorkingDirectory = System::getCurrentDirectory();

		File configFile(WorkingDirectory + "\\Configurations.json");
		nlohmann::json json = nlohmann::json::parse(std::string(reinterpret_cast<const char8_t*>(configFile.data().get())));
		auto& configs = json["Configs"];
		assert(!configs.is_null());

		assert(configs["WindowSize"].is_array());
		WindowSize = Vec2(configs["WindowSize"][0], configs["WindowSize"][1]);

		assert(configs["MinWindowSize"].is_array());
		MinWindowSize = Vec2(configs["MinWindowSize"][0], configs["MinWindowSize"][1]);

		assert(configs["FullScreen"].is_boolean());
		FullScreen = configs["FullScreen"];

		assert(configs["VSync"].is_boolean());
		VSync = configs["VSync"];

		assert(configs["Renderer"].is_string());
		assert(configs["RendererNames"].is_object());
		const std::string& renderName = configs["Renderer"];
		RendererName = configs["RendererNames"][renderName];
	}
};

NAMESPACE_END(Gear)