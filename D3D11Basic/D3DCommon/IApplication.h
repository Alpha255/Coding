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
	virtual void UpdateScene(float elapsedTime, float totalTime);
	
	virtual void ResizeWindow(uint32_t width, uint32_t height);
	virtual	LRESULT MsgProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam);

	virtual	void MouseButtonDown(WPARAM wParam, int32_t x, int32_t y);
	virtual void MouseMove(WPARAM wParam, int32_t x, int32_t y);
	virtual void MouseWheel(WPARAM wParam);

	void Startup(LPCWSTR lpTitle, uint32_t width = 800U, uint32_t height = 600U, bool bWindowed = true);
	void Running();
	void ShutDown();
protected:
	void MakeWindow(LPCWSTR lpTitle, uint32_t width, uint32_t height);

	HWND m_hWnd = nullptr;
	int32_t m_LastMousePos[2] = { 0, 0 };
	int32_t m_MouseWheelDelta = 0;
	uint32_t m_Width = 0U;
	uint32_t m_Height = 0U;
	bool m_bInited = false;

#if defined UsingimGUI
	imGUI_D3D11 m_GUI;
#elif defined UsingAntTweakBar
	GUIAntTweakBar m_GUI;
#endif

	class Camera* m_Camera = nullptr;

	typedef IApplication Base;
private:
	class Timer* m_pTimer = nullptr;
	bool m_bActive = false;
};
