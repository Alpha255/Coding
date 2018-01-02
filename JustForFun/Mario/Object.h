#pragma once

#include "Common.h"

class Object2D
{
public:
	Object2D() = default;
	Object2D(const class Image *pImage);
	~Object2D() = default;

	inline virtual uint32_t Width() 
	{
		assert(m_Width);
		return m_Width;
	}

	inline virtual uint32_t Height()
	{
		assert(m_Height);
		return m_Height;
	}

	inline virtual const class Image *GetImage() const
	{
		assert(m_Image);
		return m_Image;
	}

	bool IsCollision(const Object2D &other);
protected:
private:
	uint32_t m_Width = 0U;
	uint32_t m_Height = 0U;

	const class Image *m_Image = nullptr;
};