#include "ObjectMario.h"
#include "Image.h"

Mario::Mario()
	: Base(eMario)
{
	m_Area.Width = 32U;
	m_Area.Height = 32U;
	m_Area.ImageX = 0U;
	m_Area.ImageY = m_Image->Height() - m_Area.Width;
}