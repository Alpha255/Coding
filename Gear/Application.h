#pragma once

#include "Gear/Window.h"
#include "Gear/System.h"
#include "Colorful/Public/GfxRenderer.hpp"

NAMESPACE_START(Gear)

class Application
{
public:
	struct Configs
	{
	public:
		Configs();

		Math::Vec2 WindowSize;
		Math::Vec2 MinWindowSize;

		std::string RendererName;
	protected:
		void bindAttributes();
	private:
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

	Gfx::GfxRenderer& Renderer();
protected:
	void mountAssetsDirectory();
private:
	uint64_t m_Instance = 0u;
	WindowPtr m_Window = nullptr;
};

NAMESPACE_END(Gear)