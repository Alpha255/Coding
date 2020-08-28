#pragma once

#include "Gear/Gear.h"
#include "Gear/Window.h"
#include "Colorful/Public/GfxEngine.h"

namespaceStart(Gear)

struct Profile
{
	float32_t FrameTime = 0.0f;
	float32_t FPS = 0.0f;
	float32_t CpuTime = 0.0f;
	float32_t GpuTime = 0.0f;
	uint32_t FrameCount = 0u;
};

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
	void nextFrame(const WindowMessage& message);

	Configurations m_Config;
	WindowPtr m_Window = nullptr;
	Camera m_Camera;
	Profile m_Profile;
private:
};

namespaceEnd(Gear)
