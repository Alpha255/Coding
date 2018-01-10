#include "Renderer.h"

#include "Image.h"
#include "Object.h"

void Renderer::Init(HWND hWnd, uint32_t width, uint32_t height)
{
	assert(hWnd && width && height);

	m_WindowWidth = width;
	m_WindowHeight = height;

#if defined (UseGDI)
	m_DC = ::GetDC(hWnd);
	assert(m_DC);

	m_DCCompatible = ::CreateCompatibleDC(m_DC);
	assert(m_DCCompatible);

	m_BackBuffer = ::CreateCompatibleBitmap(m_DC, (int32_t)width, (int32_t)height);
	assert(m_BackBuffer);

	::SelectObject(m_DCCompatible, m_BackBuffer);
#elif defined (UseD3D11)
#endif
}

void Renderer::Clear()
{
#ifdef UseGDI
	::StretchDIBits(m_DCCompatible, 0, 0, m_WindowWidth, m_WindowHeight, 0, 0, m_WindowWidth, m_WindowHeight, nullptr, nullptr, DIB_RGB_COLORS, SRCCOPY);
#else
#endif
}

void Renderer::Flip()
{
	static const uint32_t s_BackgroundClr = RGB(255, 60, 174);

#ifdef UseGDI
	::TransparentBlt(m_DC, 0, 0, m_WindowWidth, m_WindowHeight, m_DCCompatible, 0, 0, m_WindowWidth, m_WindowHeight, s_BackgroundClr);
#else
#endif
}

void Renderer::DrawObject(const Object2D *pObject)
{
	assert(pObject);

#ifdef UseGDI
	const Object2D::Area &area = pObject->GetArea();
	const Image *pImage = pObject->GetImage();

	::StretchDIBits(m_DCCompatible,
		area.Left,
		area.Top,
		area.Width,
		area.Height,
		0,
		0,
		pImage->Width(),
		pImage->Height(),
		pImage->Data(),
		pImage->BitmapInfo(),
		DIB_RGB_COLORS,
		SRCCOPY);
#else
#endif
}
