#include "Renderer.h"

#include "Image.h"
#include "Object2D.h"

void Renderer::Init(HWND hWnd, uint32_t width, uint32_t height)
{
	assert(hWnd && width && height);

	m_WindowRect.right = width;
	m_WindowRect.bottom = height;

#if defined (UseGDI)
	m_DC = ::GetDC(hWnd);
	assert(m_DC);

	/// Compatiable DC
	m_DCCompatible = ::CreateCompatibleDC(m_DC);
	assert(m_DCCompatible);

	::HBITMAP bitmapCompatible = ::CreateCompatibleBitmap(m_DC, (int32_t)width, (int32_t)height);
	assert(bitmapCompatible);

	::SelectObject(m_DCCompatible, bitmapCompatible);

	/// Agent DC
	m_DCAgent = ::CreateCompatibleDC(m_DC);
	assert(m_DCAgent);

	::HBITMAP bitmapAgent = ::CreateCompatibleBitmap(m_DC, (int32_t)width, (int32_t)height);
	assert(bitmapAgent);

	::SelectObject(m_DCAgent, bitmapAgent);
#elif defined (UseD3D11)
#endif
}

void Renderer::Clear()
{
#ifdef UseGDI
	::FillRect(m_DCCompatible, &m_WindowRect, m_ClearColor);
#else
#endif
}

void Renderer::Flip()
{
#ifdef UseGDI
	::BitBlt(
		m_DC, 
		m_WindowRect.left, 
		m_WindowRect.top, 
		m_WindowRect.right, 
		m_WindowRect.bottom, 
		m_DCCompatible, 
		0, 
		0, 
		SRCCOPY);
#else
#endif
}

void Renderer::DrawObject(const Object2D *pObject)
{
	assert(pObject);

#ifdef UseGDI
	const Object2D::Area &area = pObject->GetArea();
	const Image *pImage = pObject->GetImage();

	::StretchDIBits(m_DCAgent,
		0,
		0,
		area.Width,
		area.Height,
		area.ImageX,
		area.ImageY,
		area.Width,
		area.Height,
		pImage->Data(),
		pImage->BitmapInfo(),
		DIB_RGB_COLORS,
		SRCCOPY);

	static const uint32_t s_AlphaClr = RGB(255, 60, 174);
	::TransparentBlt(
		m_DCCompatible,
		area.Left,
		area.Top,
		area.Width,
		area.Height,
		m_DCAgent,
		0,
		0,
		area.Width,
		area.Height,
		s_AlphaClr);
#else
#endif
}
