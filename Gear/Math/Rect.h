#pragma once

#include "Gear/Math/Vector4.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

class Rect : public Vec4
{
public:
	Rect()
		: Vec4(0.0f, 0.0f, 0.0f, 0.0f)
	{
	}

	Rect(float32_t left, float32_t top, float32_t right, float32_t bottom)
		: Vec4(left, top, right, bottom)
	{
	}

	Rect(const Vec4& other)
		: Vec4(other)
	{
	}

	inline float32_t width()
	{
		return z - x;
	}

	inline float32_t height()
	{
		return w - y;
	}

	bool8_t isIntersect(const Rect& other);
protected:
private:
};

NAMESPACE_END(Math)
NAMESPACE_END(Gear)
