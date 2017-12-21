#pragma once

#include "IApplication.h"
#include "resource.h"

#define UsingimGUI
///#define UsingAntTweakBar

#if defined UsingimGUI
#include "GUI_imGUI.h"
#elif defined UsingAntTweakBar
#include "GUI_AntTweakBar.h"
#endif

class D3DApplication : public IApplication
{
public:
	D3DApplication();
	virtual ~D3DApplication();

	virtual	LRESULT MsgProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam);

	virtual void ResizeWindow(uint32_t width, uint32_t height);

	virtual void MouseWheel(WPARAM wParam);
	virtual void MouseMove(WPARAM wParam, int32_t x, int32_t y);

	virtual void RenderToWindow() final;

	virtual void SetupScene() {}
	virtual void UpdateScene(float, float);
	virtual void RenderScene() {}
protected:
	class Camera* m_Camera = nullptr;

	typedef D3DApplication Base;
private:
	int32_t m_MouseWheelDelta = 0;

#if defined UsingimGUI
	imGUI_D3D11 m_GUI;
#elif defined UsingAntTweakBar
	GUIAntTweakBar m_GUI;
#endif

	bool m_bInited = false;
};