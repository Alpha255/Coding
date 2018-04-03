#pragma once

#include "IApplication.h"

class D3DApp : public IApplication
{
public:
	D3DApp();
	virtual ~D3DApp();

	virtual	LRESULT MsgProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam);

	virtual void ResizeWindow(uint32_t width, uint32_t height);

	virtual void MouseWheel(WPARAM wParam);
	virtual void MouseMove(WPARAM wParam, int32_t x, int32_t y);

	virtual void RenderToWindow() final;
	virtual void InitRenderer() final;

	virtual void PreInit() {}
	virtual void Update(float, float) {};
	virtual void RenderScene() {};
	
	void Frame();
protected:
	class Camera* m_Camera = nullptr;
	class D3DEngine *s_Renderer = nullptr;

	typedef D3DApp Base;
private:
	int32_t m_MouseWheelDelta = 0;

#if defined UsingimGUI
	imGUI_D3D11 m_GUI;
#elif defined UsingAntTweakBar
	GUIAntTweakBar m_GUI;
#endif

	bool m_bInited = false;
};