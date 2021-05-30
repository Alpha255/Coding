#pragma once

#include "Gear/Gear.h"
#include "Gear/Configurations.hpp"
#include "Colorful/Gfx/GfxRenderer.h"
#include "Colorful/Gfx/AssetTool/AssetTool.h"
#include "Colorful/Gfx/ImGui/ImGui.h"

NAMESPACE_START(Gfx)
	GfxRendererPtr GRenderer;
NAMESPACE_END(Gfx)

class Rockcat
{
public:
	template<class Sample>
	void runSample(const std::string& sampleName)
	{
		uint64_t Instance = System::getModuleHandle();
		assert(Instance);

		Configurations DefaultConfigs;

		WindowPtr MainWindow = std::make_unique<Window>(Instance, sampleName, DefaultConfigs.WindowSize, DefaultConfigs.MinWindowSize);

		System::DynamicLibrary RendererDll(std::string(DefaultConfigs.WorkingDirectory + "\\" + DefaultConfigs.RendererName + DLL_POSTFIX));
		Gfx::CreateRendererFunc CreateRendererfunc = static_cast<Gfx::CreateRendererFunc>(RendererDll.getProcAddress(CREATE_RENDERER_FUNC_NAME));
		assert(CreateRendererfunc);

		CreateRendererfunc(Gfx::GRenderer);
		assert(Gfx::GRenderer);

		Gfx::GRenderer->initialize(
			MainWindow->handle(),
			static_cast<uint32_t>(DefaultConfigs.WindowSize.x),
			static_cast<uint32_t>(DefaultConfigs.WindowSize.y),
			DefaultConfigs.FullScreen,
			DefaultConfigs.VSync);

		Gfx::AssetTool::instance().initialize(DefaultConfigs.WorkingDirectory, Gfx::GRenderer->device());

		auto ImGui = create_unique<Gfx::ImGuiRenderer>();

		auto SamplePtr = std::make_unique<Sample>();
		SamplePtr->initialize();
		while (MainWindow->state() != Window::EState::Destroyed)
		{
			MainWindow->update();

			if (MainWindow->isActive())
			{
				SamplePtr->renderScene();

				ImGui->beginFrame(MainWindow->width(), MainWindow->height());
				SamplePtr->renderGUI();
				ImGui->endFrame();
			}
			else
			{
				System::sleep(1u);
			}
		}
		SamplePtr.reset();

		ImGui.reset();
		Gfx::AssetTool::instance().finalize();
		Gfx::GRenderer->finalize();
		Gfx::GRenderer.reset();
	}
};
DECLARE_UNIQUE_PTR(Rockcat)

#if defined(PLATFORM_WIN32)
	#define RUN_APPLICATION(Application)                               \
		int32_t WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int32_t) \
		{                                                              \
			Rockcat().runSample<Application>(#Application);            \
			return 0;                                                  \
		}
#else
	#error Unknown platform!
#endif