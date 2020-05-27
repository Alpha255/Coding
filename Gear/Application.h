#pragma once

#include "Gear/Gear.h"
#include "Gear/Window.h"
#include "Colorful/Public/GfxEngine.h"

namespaceStart(Gear)

class Application
{
public:
	Application() = default;
	virtual ~Application() = default;

	void initialize(const std::string& windowTitle);

	virtual void postInitialize() {}

	virtual void loop();

	virtual void finalize();

	virtual void renderFrame() = 0;
protected:
	void createGfxRenderer();
	void updateFPS();

	Configurations m_Config;
	WindowPtr m_Window = nullptr;
	GfxEnginePtr m_GfxEngine = nullptr;
	DXUTCamera m_Camera;
	CpuTimer m_CpuTimer;
	float32_t m_FPS = 0.0f;
private:
};

namespaceEnd(Gear)
