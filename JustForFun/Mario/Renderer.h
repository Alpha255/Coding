#pragma once

#include "Common.h"

#define UseGDI
///#define UseD3D11

class Renderer
{
public:
	Renderer() = default;
	~Renderer()
	{
#ifdef UseGDI
		::DeleteObject(m_BackBuffer);
		::DeleteDC(m_DCCompatible);
		::DeleteDC(m_DC);
#endif
	}
	Renderer(const Renderer &) = delete;
	void operator= (const Renderer &) = delete;

	void Init(HWND hWnd, uint32_t width, uint32_t height);

	void Clear();

	void Flip();

	void DrawObject(const class Object2D *pObject);
protected:
private:
#ifdef UseGDI
	::HDC m_DC = nullptr;
	::HDC m_DCCompatible = nullptr;
	::HBITMAP m_BackBuffer = nullptr;
#endif

	uint32_t m_WindowWidth = 0U;
	uint32_t m_WindowHeight = 0U;
};
