#pragma once

#define UsingD3D11
///#define UsingVulkan

#ifdef UsingD3D11
#include "D3DEngine.h"
#include "D3DGUI_imGui.h"
typedef D3DEngine Renderer;
typedef D3DGUI_imGui GUI_ImGui;
#elif defined(UsingVulkan)
#include "VulkanEngine.h"
#include "D3DGUI_imGui.h"
typedef VkEngine Renderer;
typedef D3DGUI_imGui GUI_ImGui;
#else
#error "Undefine Renderer"
#endif

#include "Common.h"

class IApplication
{
public:
	IApplication();
	virtual ~IApplication() = default;

	virtual	::LRESULT MsgProc(::HWND hWnd, uint32_t msg, ::WPARAM wParam, ::LPARAM lParam) = 0;

	virtual void ResizeWindow(uint32_t, uint32_t) {}
	virtual void MouseWheel(::WPARAM) {}

	virtual void MouseMove(::WPARAM, int32_t x, int32_t y)
	{
		m_LastMousePos[0] = x;
		m_LastMousePos[1] = y;
	}
	virtual	void MouseButtonDown(::WPARAM, int32_t x, int32_t y)
	{
		m_LastMousePos[0] = x;
		m_LastMousePos[1] = y;
	}

	virtual void HandleWindowMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam);
	virtual void HandleInput(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam);

	virtual void RenderToWindow() = 0;

	virtual void InitRenderer() = 0;

	void UpdateWindow();
	void Startup(const wchar_t *pTitle, uint32_t width = 800U, uint32_t height = 600U, bool bWindowed = true, uint32_t windowStyle = 0U);
	void Running();
	void ShutDown() {}
protected:
	void MakeWindow(const wchar_t *pTitle, uint32_t width, uint32_t height, uint32_t windowStyle);

	::HWND m_hWnd = nullptr;
	int32_t m_LastMousePos[2] = {};
	uint32_t m_Width = 0U;
	uint32_t m_Height = 0U;
	uint16_t m_IconID = 0U;
	bool m_bWindowed = false;

	std::unique_ptr<class Timer> m_Timer = nullptr;
private:
	bool m_bActive = false;
	bool m_bNeedResize = false;
};
