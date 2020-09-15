#include "Gear/Application.h"
#include <ThirdParty/json/single_include/nlohmann/json.hpp>

NAMESPACE_START(Gear)

Application::Configs::Configs()
{
	Gear::File configFile("Configurations.json");
	nlohmann::json json(std::string(reinterpret_cast<const char8_t*>(configFile.data().get())));
	auto& configs = json["Configs"];
	assert(!configs.is_null());

	assert(configs["WindowSize"].is_array());
	WindowSize = Vec2(configs["WindowSize"][0], configs["WindowSize"][1]);

	assert(configs["MinWindowSize"].is_array());
	MinWindowSize = Vec2(configs["MinWindowSize"][0], configs["MinWindowSize"][1]);

	assert(configs["FullScreen"].is_boolean());
	FullScreen = configs["FullScreen"];

	assert(configs["Renderer"].is_string());
	assert(configs["RendererNames"].is_object());
	const std::string& renderName = configs["Renderer"];
	RendererName = configs["RendererNames"][renderName];
}

void Application::mountAssetsDirectory()
{
}

void Application::initialize(const std::string& name, const Configs& configs)
{
	m_Window = std::make_unique<Window>(m_Instance, name, configs.WindowSize, configs.MinWindowSize);
	mountAssetsDirectory();
	onInitialize();
}

void Application::run()
{
}

void Application::finalize()
{
	onFinalize();
}

NAMESPACE_END(Gear)
