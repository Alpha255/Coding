#pragma once

#include "Common.h"

#if defined UsingimGUI
#include "GUI_imGUI.h"
#elif defined UsingAntTweakBar
#include "GUI_AntTweakBar.h"
#endif

class IApplication
{
public:
	IApplication();
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

	void Startup(LPCWSTR lpTitle, uint32_t width = 800U, uint32_t height = 600U, bool bWindowed = true);
	void Running();
	void ShutDown();
protected:
	void MakeWindow(LPCWSTR lpTitle, uint32_t width, uint32_t height);

	HWND m_hWnd;
	int m_LastMousePos[2];
	uint32_t m_Width;
	uint32_t m_Height;
	bool m_bInited;

#if defined UsingimGUI
	imGUI_D3D11 m_GUI;
#elif defined UsingAntTweakBar
	GUIAntTweakBar m_GUI;
#endif

	typedef IApplication Base;
private:
	class Timer* m_pTimer;
	bool m_bActive;
};
