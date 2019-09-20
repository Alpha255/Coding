#pragma once

#include "Vector.h"

namespaceStart(gear)
namespaceStart(math)

class vec2 : public DirectX::XMFLOAT2A
{
public:
	inline vec2()
		: DirectX::XMFLOAT2A(0.0f, 0.0f)
	{
	}

	inline vec2(float32_t x, float32_t y)
		: DirectX::XMFLOAT2A(x, y)
	{
	}

	inline vec2(const float32_t* pArray)
		: DirectX::XMFLOAT2A(pArray)
	{
	}

#if !defined(UsingSSE)
	vecMemberFuncsA(2)
#else
	inline void operator+=(const vec2 &right)
	{
		x += right.x;
		y += right.y;
	}

	inline void operator-=(const vec2 &right)
	{
		x -= right.x;
		y -= right.y;
	}

	inline void operator*=(const float32_t right)
	{
		x *= right;
		y *= right;
	}

	inline float32_t operator*=(const vec2 &right)
	{
		return x * right.x + y * right.y;
	}
#endif
};

#if !defined(UsingSSE)
	vecPublicFuncsA(2)
#else
inline vec2 operator*(const vec2 &left, const float32_t right)
{
	return vec2(left.x * right, left.y * right);
}

inline float32_t operator*(const vec2 &left, const vec2 &right)
{
	return left.x * right.x + left.y * right.y;
}

inline vec2 operator+(const vec2 &left, const vec2 &right)
{
	return vec2(left.x + right.x, left.y + right.y);
}

inline vec2 operator-(const vec2 &left, const vec2 &right)
{
	return vec2(left.x - right.x, left.y - right.y);
}
#endif

namespaceEnd(math)
namespaceEnd(gear)
