#include "Object2D.h"

#include "Image.h"
#include "Engine.h"

Object2D::Object2D(eType type)
{
	m_Image = Engine::Instance().GetImage(type);
	assert(m_Image);
}
