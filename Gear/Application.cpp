#include "Gear/Application.h"
#include <ThirdParty/json/single_include/nlohmann/json.hpp>
#include "Colorful/D3D/DXGI_Interface.h"
#include "Colorful/ImGui/ImGui.h"

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

	assert(configs["VSync"].is_boolean());
	VSync = configs["VSync"];

	assert(configs["Renderer"].is_string());
	assert(configs["RendererNames"].is_object());
	const std::string& renderName = configs["Renderer"];
	RendererName = configs["RendererNames"][renderName];
}

void Application::initialize(const std::string& name, const Configs& configs)
{
#if 1
#endif
	m_Window = std::make_unique<Window>(m_Instance, name, configs.WindowSize, configs.MinWindowSize);

	Gfx::AssetTool::instance().initialize(configs.WorkingDirectory);

	m_RendererDll = std::make_shared<System::DynamicLibrary>(std::string(configs.WorkingDirectory + "\\" + configs.RendererName + DLL_POSTFIX));
	Gfx::CreateRendererFunc createRendererfunc = static_cast<Gfx::CreateRendererFunc>(m_RendererDll->getProcAddress(CREATE_RENDERER_FUNC_NAME));
	assert(createRendererfunc);
	Gfx::GRenderer = createRendererfunc();
	assert(Gfx::GRenderer);
	Gfx::GResourceMgr = Gfx::GRenderer->createDevice();
	Gfx::GRenderer->createSwapchain(
		m_Instance,
		m_Window->handle(),
		static_cast<uint32_t>(configs.WindowSize.x),
		static_cast<uint32_t>(configs.WindowSize.y),
		configs.FullScreen,
		configs.VSync);

	///Gfx::ImGuiRendererPtr ImGui = std::make_unique<Gfx::ImGuiRenderer>();

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

	Gfx::GRenderer->finalize();
	Gfx::AssetTool::instance().finalize();
}

NAMESPACE_END(Gear)
