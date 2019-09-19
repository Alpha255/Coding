#pragma once

#include "Vector.h"

namespaceStart(gear)

class Vec2 : public DirectX::XMFLOAT2
{
public:
	inline Vec2()
		: DirectX::XMFLOAT2(0.0f, 0.0f)
	{
	}

	inline Vec2(float32_t x, float32_t y)
		: DirectX::XMFLOAT2(x, y)
	{
	}

	inline Vec2(const float32_t* pArray)
		: DirectX::XMFLOAT2(pArray)
	{
	}

	inline void normalize()
	{
		vecNormalize(2, this);
	}

	inline void operator+=(const Vec2 &right)
	{
		x += right.x;
		y += right.y;
	}

	inline void operator-=(const Vec2 &right)
	{
		x -= right.x;
		y -= right.y;
	}

	inline void operator*=(const float32_t right)
	{
		x *= right;
		y *= right;
	}

	inline float32_t operator*=(const Vec2 &right)
	{
		return x * right.x + y * right.y;
	}
};

inline Vec2 operator*(const Vec2 &left, const float32_t right)
{
	return Vec2(left.x * right, left.y * right);
}

inline float32_t operator*(const Vec2 &left, const Vec2 &right)
{
	return left.x * right.x + left.y * right.y;
}

inline Vec2 operator+(const Vec2 &left, const Vec2 &right)
{
	return Vec2(left.x + right.x, left.y + right.y);
}

inline Vec2 operator-(const Vec2 &left, const Vec2 &right)
{
	return Vec2(left.x - right.x, left.y - right.y);
}

namespaceEnd(gear)
