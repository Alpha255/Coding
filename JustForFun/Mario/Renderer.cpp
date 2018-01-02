#include "Renderer.h"

#include "Image.h"

void Renderer::Init(HWND hWnd, uint32_t width, uint32_t height)
{
	assert(hWnd && width && height);

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

void Renderer::DrawImage(const Image *pImage)
{
	assert(pImage);
}
