#pragma once

#include "Common.h"

class IApplication
{
public:
	IApplication();
	virtual ~IApplication();

	virtual	LRESULT MsgProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam) = 0;

	virtual void ResizeWindow(uint32_t, uint32_t) {}
	virtual void MouseWheel(WPARAM) {}

	virtual void MouseMove(WPARAM, int32_t x, int32_t y)
	{
		m_LastMousePos[0] = x;
		m_LastMousePos[1] = y;
	}
	virtual	void MouseButtonDown(WPARAM, int32_t x, int32_t y)
	{
		m_LastMousePos[0] = x;
		m_LastMousePos[1] = y;
	}

	virtual void HandleWindowMessage(uint32_t msg, WPARAM wParam, LPARAM lParam);
	virtual void HandleInput(uint32_t msg, WPARAM wParam, LPARAM lParam);

	virtual void RenderToWindow() = 0;

	void Startup(LPCWSTR lpTitle, uint32_t width = 800U, uint32_t height = 600U, bool bWindowed = true);
	void Running();
	void ShutDown() {}
protected:
	void MakeWindow(LPCWSTR lpTitle, uint32_t width, uint32_t height);

	HWND m_hWnd = nullptr;
	int32_t m_LastMousePos[2] = { 0, 0 };
	uint32_t m_Width = 0U;
	uint32_t m_Height = 0U;
	uint16_t m_IconID = 0U;
	bool m_bWindowed = false;

	class Timer* m_pTimer = nullptr;
private:
	bool m_bActive = false;
};
