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

	rect(const ::RECT &winRect)
		: vec4(
		(float32_t)winRect.left, 
		(float32_t)winRect.top, 
		(float32_t)winRect.right, 
		(float32_t)winRect.bottom)
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

	inline ::RECT toWinRect()
	{
		return ::RECT{ (long32_t)x, (long32_t)y, (long32_t)z, (long32_t)w };
	}

	bool8_t isCollide(const rect &other);
protected:
private:
};

namespaceEnd(math)
namespaceEnd(gear)
