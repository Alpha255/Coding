#pragma once

#include "Timer.h"
#include "Camera.h"
#include "Definitions.h"
#include "resource.h"

#if defined(UsingD3D11)
	#define IconID IconDirectX 
#elif defined(UsingVulkan)
	#define IconID IconVulkan 
#endif

class IApplication
{
public:
	IApplication() = default;
	virtual ~IApplication() = default;

	::LRESULT MessageProcFunc(::HWND hWnd, uint32_t msg, ::WPARAM wParam, ::LPARAM lParam);

	void Startup(const wchar_t *pTitle, uint32_t width = 800U, uint32_t height = 600U, bool bFullScreen = false, uint32_t windowStyle = 0U);
	void Running();
	void ShutDown();

	virtual void SetupRenderEngine() = 0;
	virtual void InitScene() {}
	virtual void RenderScene() = 0;
	virtual void UpdateScene(float, float) {}
protected:
	void MakeWindow(const wchar_t *pTitle, uint32_t width, uint32_t height, uint32_t windowStyle);
	void ResizeWindow(uint32_t, uint32_t);
	void UpdateWindow();
	void RenderToWindow();

	void HandleWindowMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam);
	void HandleInput(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam);
	void UpdateFPS();

	inline void MouseMove(::WPARAM wParam, int32_t x, int32_t y)
	{
		if ((wParam & MK_LBUTTON) != 0)
		{
			m_Camera.Move(x - m_LastMousePos[0], y - m_LastMousePos[1]);
		}

		m_LastMousePos[0] = x;
		m_LastMousePos[1] = y;
	}

	inline void MouseButtonDown(::WPARAM, int32_t x, int32_t y)
	{
		m_LastMousePos[0] = x;
		m_LastMousePos[1] = y;
	}

	void MouseWheel(::WPARAM wParam);

	::HWND m_hWnd = nullptr;
	int32_t m_LastMousePos[2] = {};
	uint32_t m_Width = 0U;
	uint32_t m_Height = 0U;
	float m_FPS = 0.0f;
	uint16_t m_IconID = IconID;
	int16_t m_MouseWheelDelta = 0;
	bool m_bFullScreen = false;
	bool m_bDrawGUI = false;
	bool m_bVSync = false;

	Timer m_Timer;
	Camera m_Camera;
private:
	bool m_bActive = false;
	bool m_bNeedResize = false;
	bool m_bRenderEngineInited = false;
};
