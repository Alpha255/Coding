#pragma once

#include "gear/gear.h"
#include "gear/Public/Config.h"

namespaceStart(gear)

class application
{
public:
	application() = default;
	virtual ~application() = default;

	void initialize(const std::string &title, uint32_t extraWindowStyle);

	virtual void postInitialize() {}

	virtual void loop();

	virtual void finalize() {}

	virtual void resizeWindow() {}

	virtual void updateWindow();

	virtual void renterToWindow() {}

	virtual void handleMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam);

	virtual void handleInput(uint32_t, ::WPARAM, ::LPARAM) {}

	static ::LRESULT messageProc(::HWND hWnd, uint32_t msg, ::WPARAM wParam, ::LPARAM lParam);
protected:
	void makeWindow(const std::string &title, uint32_t width, uint32_t height, uint32_t extraWindowStyle);
	void updateFPS();

	::HWND m_hWnd = nullptr;

	vec2 m_WindowSize = { 0.0f, 0.0f };

	float32_t m_FPS = 0.0f;
	uint16_t m_IconID = UINT16_MAX;
	bool8_t m_bActive = false;

	timer m_Timer;
	dxutCamera m_Camera;
	appConfig m_Config;
private:
	bool8_t m_bNeedResize = false;
	uint32_t m_FrameCount = 0U;
	float32_t m_LastUpdateTime = 0.0f;
};

namespaceEnd(gear)
