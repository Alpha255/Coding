#include "ObjectMario.h"
#include "Image.h"

Mario::Mario()
	: Base(eMario)
{
	m_Area.ObjectWidth = 32U;
	m_Area.ObjectHeight = 32U;
	m_Area.UVX = 0U;
	m_Area.UVY = m_Image->Height() - m_Area.ObjectWidth;
}