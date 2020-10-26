#pragma once

#include "Gear/Gear.h"
#include "Colorful/Public/GfxRenderer.h"

NAMESPACE_START(Gear)

class Application
{
public:
	struct Configs
	{
		Configs();

		Math::Vec2 WindowSize;
		Math::Vec2 MinWindowSize;
		bool8_t FullScreen = false;
		bool8_t VSync = false;

		std::string RendererName;
		std::string WorkingDirectory;
	};

	Application()
		: m_Instance(System::getModuleHandle())
	{
	}

	virtual ~Application()
	{
		finalize();
	}

	virtual void onInitialize() {};
	void initialize(const std::string& name, const Configs& configs);

	void run();

	virtual void onResize(uint32_t width, uint32_t height);

	virtual void onFinalize() {};
	void finalize();
protected:
private:
	uint64_t m_Instance = 0u;
	WindowPtr m_Window = nullptr;
	System::DynamicLibraryPtr m_RendererDll = nullptr;
	Gfx::GfxRendererPtr m_Renderer = nullptr;
};

NAMESPACE_END(Gear)