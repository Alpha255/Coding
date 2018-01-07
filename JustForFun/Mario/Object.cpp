#include "Object.h"

#include "Image.h"
#include "Engine.h"

Object2D::Object2D(eType type)
{
	m_Image = Engine::Instance().GetImage(type);

	m_Area.Width = m_Image->Width();
	m_Area.Height = m_Image->Height();
}
