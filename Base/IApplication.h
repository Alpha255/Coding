#pragma once

#include "Timer.h"

class IApplication
{
public:
	IApplication() = default;

	virtual ~IApplication() = default;

	virtual void Initialize(const std::string &title, uint32_t width, uint32_t height, bool bFullScreen, uint32_t extraWindowStyle);

	virtual void Loop();

	virtual void Finalize() {};

	virtual void ResizeWindow(uint32_t, uint32_t) {}

	virtual void UpdateWindow();

	virtual void RenterToWindow() = 0;

	virtual void HandleWindowMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam);

	virtual void HandleInput(uint32_t, ::WPARAM, ::LPARAM) {};

	static ::LRESULT MessageProcFunc(::HWND hWnd, uint32_t msg, ::WPARAM wParam, ::LPARAM lParam);
protected:
	typedef IApplication Base;

	void MakeWindow(const std::string &title, uint32_t width, uint32_t height, uint32_t extraWindowStyle);
	void UpdateFPS();

	::HWND m_hWnd = nullptr;

	std::pair<uint32_t, uint32_t> m_WindowSize = { 0U, 0U };

	float m_FPS = 0.0f;
	uint16_t m_IconID = UINT16_MAX;
	bool m_bFullScreen = false;

	Timer m_Timer = {};
private:
	bool m_bActive = false;
	bool m_bNeedResize = false;
	static IApplication *s_This;
};
