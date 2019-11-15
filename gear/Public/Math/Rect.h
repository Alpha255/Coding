#pragma once

#include "Vector4.h"

namespaceStart(gear)
namespaceStart(math)

class rect : public vec4
{
public:
	rect(float32_t left, float32_t top, float32_t right, float32_t bottom)
		: vec4(left, top, right, bottom)
	{
	}

	rect(const vec4 &other)
		: vec4(other)
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

	bool8_t isIntersect(const rect &other);
protected:
private:
};

namespaceEnd(math)
namespaceEnd(gear)
