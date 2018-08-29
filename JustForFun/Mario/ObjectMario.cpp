#include "ObjectMario.h"
#include "Image.h"

Mario::Mario()
	: Base(eMario, 32U, 32U)
{
	m_Area.UVX = 0U;
	m_Area.UVY = m_Image->Height() - m_Area.ObjectWidth;
}