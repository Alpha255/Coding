#pragma once

#include "Common.h"

#define UseGDI
///#define UseD3D11

class Renderer
{
public:
	Renderer()
	{
#ifdef UseGDI
		uint32_t backgroundClr = RGB(107, 136, 255);
		m_ClearColor = ::CreateSolidBrush(backgroundClr);
		assert(m_ClearColor);
#endif
	}

	~Renderer()
	{
#ifdef UseGDI
		::DeleteDC(m_DCAgent);
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
	::HDC m_DCAgent = nullptr;
	::HBRUSH m_ClearColor = nullptr;
#endif

	::RECT m_WindowRect = { 0L, 0L, 0L, 0L };
};
