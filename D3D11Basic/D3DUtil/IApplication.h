#pragma once

#include "Common.h"

class IApplication
{
public:
	IApplication(HINSTANCE hInstance, LPCWSTR lpTitle, uint32_t width = 800U, uint32_t height = 600U, bool bWindowed = true);
	virtual ~IApplication();

	virtual void SetupScene() = 0;
	virtual void RenderScene() = 0;
	virtual void UpdateScene(float /*elapsedTime*/, float /*totalTime*/) {}
	
	virtual void ResizeWindow(uint32_t width, uint32_t height);
	virtual	LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	virtual	void MouseButtonDown(WPARAM wParam, int x, int y);
	virtual void MouseButtonUp(WPARAM /*wParam*/, int /*x*/, int /*y*/) {}
	virtual void MouseMove(WPARAM /*wParam*/, int /*x*/, int /*y*/) {}
	virtual void Keyboard(WPARAM /*wParam*/) {}

	int MainLoop();
protected:
	void MakeWindow(HINSTANCE hInstance, LPCWSTR lpTitle, uint32_t width, uint32_t height);

	HWND m_hWnd;
	int m_LastMousePos[2];
	uint32_t m_Size[2];
	bool m_bInited;

	typedef IApplication Base;
private:
	class Timer* m_pTimer;
	bool m_bActive;
};
