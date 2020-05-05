#pragma once

#include "Gear/gear.h"
#include "Colorful/Public/ImGui/ImGui.h"

namespaceStart(Gear)

class Application
{
public:
	Application() = default;
	virtual ~Application() = default;

	void initialize(const std::string &title, uint32_t extraWindowStyle);

	virtual void postInitialize() {}

	virtual void loop();

	virtual void finalize();

	virtual void updateWindow();

	virtual void renterToWindow() {}

	virtual void processEvent();

	inline virtual void resizeWindow()
	{
		assert(m_Renderer);
		m_Renderer->handleWindowResize((uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y, m_Config);
	}
protected:
	void makeWindow(const std::string &title, uint32_t width, uint32_t height, uint32_t extraWindowStyle, uint16_t iconID);
	void updateFPS();

	uint64_t m_WindowHandle = 0u;

	Math::Vec2 m_WindowSize = { 0.0f, 0.0f };

	float32_t m_FPS = 0.0f;
	bool8_t m_bActive = false;

	CpuTimer m_cpuTimer;
	dxutCamera m_Camera;
	Configurations m_Config;
	rEngine *m_Renderer = nullptr;
	ImGuiRenderer m_GuiRender;
private:
	bool8_t m_bNeedResize = false;
	uint32_t m_FrameCount = 0U;
	float32_t m_LastUpdateTime = 0.0f;
};

namespaceEnd(Gear)
