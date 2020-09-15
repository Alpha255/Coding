#include "Gear/Application.h"
#include <ThirdParty/json/single_include/nlohmann/json.hpp>

NAMESPACE_START(Gear)

Application::Configs::Configs()
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

	assert(configs["Renderer"].is_string());
	assert(configs["RendererNames"].is_object());
	const std::string& renderName = configs["Renderer"];
	RendererName = configs["RendererNames"][renderName];
}

void Application::mountAssetsDirectory(const Configs& configs)
{
	std::string assetDirectory(File::directory(configs.WorkingDirectory) + "\\Assets");
	assert(File::isExists(assetDirectory, true));
	System::setWorkingDirectory(assetDirectory);
	LOG_INFO("Mount working directory to \"%s\"", System::getCurrentWorkingDirectory().c_str());
}

void Application::initialize(const std::string& name, const Configs& configs)
{
	m_Window = std::make_unique<Window>(m_Instance, name, configs.WindowSize, configs.MinWindowSize);
	mountAssetsDirectory(configs);
	onInitialize();
}

void Application::run()
{
	while (true)
	{

	}
}

void Application::finalize()
{
	onFinalize();
}

NAMESPACE_END(Gear)
