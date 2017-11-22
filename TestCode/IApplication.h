#pragma once

#include <Windows.h>
#include <stdint.h>

class IApplication
{
public:
	IApplication();
	~IApplication() = default;

	void RenderScene();
	
	void ResizeWindow(uint32_t width, uint32_t height);

	void Startup(LPCWSTR lpTitle, uint32_t width = 800U, uint32_t height = 600U, bool bWindowed = true);
	void Running();
	void ShutDown();
protected:
	void MakeWindow(LPCWSTR lpTitle, uint32_t width, uint32_t height);

	HWND m_hWnd;
	uint32_t m_Width;
	uint32_t m_Height;
private:
};
