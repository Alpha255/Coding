#include "Gear/Application.h"
#include <ThirdParty/json/single_include/nlohmann/json.hpp>
#include "Colorful/D3D/DXGI_Interface.h"

NAMESPACE_START(Gear)

static Gfx::GfxRenderer* GRenderer = nullptr;

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

void Application::initializeRenderer(const Configs& configs)
{
	m_DynamicLib = std::make_shared<System::DynamicLibrary>(std::string(configs.WorkingDirectory + "\\" + configs.RendererName + DLL_POSTFIX));
	Gfx::CreateRendererFunc func = static_cast<Gfx::CreateRendererFunc>(m_DynamicLib->getProcAddress(CREATE_RENDERER_FUNC_NAME));
	assert(func);
	func(m_Renderer);

	m_Renderer->createDevice();

	GRenderer = m_Renderer.get();
}

void Application::initialize(const std::string& name, const Configs& configs)
{
#if 1
#endif

	m_Window = std::make_unique<Window>(m_Instance, name, configs.WindowSize, configs.MinWindowSize);
	initializeRenderer(configs);
	mountAssetsDirectory(configs);
	onInitialize();
}

void Application::run()
{
	while (m_Window->state() != Window::EState::Destroyed)
	{
		m_Window->update();

		if (m_Window->isActive())
		{
		}
		else
		{
			System::sleep(1u);
		}
	}
}

void Application::onResize(uint32_t width, uint32_t height)
{
	(VOID)(width);
	(VOID)(height);
}

void Application::finalize()
{
	onFinalize();

	m_Renderer->finalize();
}

NAMESPACE_END(Gear)
