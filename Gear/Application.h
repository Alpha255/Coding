#pragma once

#include "Gear/Window.h"
#include "Gear/System.h"
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
		bool8_t FullScreen;

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

	virtual void onFinalize() {};
	void finalize();
protected:
	void mountAssetsDirectory(const Configs& configs);
private:
	uint64_t m_Instance = 0u;
	WindowPtr m_Window = nullptr;
};

NAMESPACE_END(Gear)