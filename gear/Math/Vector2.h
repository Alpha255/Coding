#pragma once

#include "Functions.h"

namespaceStart(gear)
namespaceStart(math)

class vec2 : public DirectX::XMFLOAT2A
{
public:
	inline vec2()
		: DirectX::XMFLOAT2A(0.0f, 0.0f)
	{
	}

	inline vec2(float32_t value)
		: DirectX::XMFLOAT2A(value, value)
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

#if defined(UsingSSE)
	vecMemberFuncsA(2)
#else
	inline float32_t lengthSq()
	{
		return x * x + y * y;
	}

	inline float32_t length()
	{
		return ::sqrtf(lengthSq());
	}

	inline void normalize()
	{
		float32_t factor = 1.0f / length();
		x *= factor;
		y *= factor;
	}

	inline vec2 cross(const vec2 &right)
	{
		float32_t value = x * right.y - y * right.x;
		x = y = value;
	}

	inline float32_t dot(const vec2 &right) const
	{
		return x * right.x + y * right.y;
	}

	inline void negate()
	{
		x = -x;
		y = -y;
	}

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

	inline void operator*=(float32_t right)
	{
		x *= right;
		y *= right;
	}

	inline void operator*=(const vec2 &right)
	{
		x *= right.x;
		y *= right.y;
	}
#endif
};

#if defined(UsingSSE)
	vecPublicFuncsA(2)
#else
inline vec2 operator+(const vec2 &left, const vec2 &right)
{
	return vec2(left.x + right.x, left.y + right.y);
}

inline vec2 operator-(const vec2 &left, const vec2 &right)
{
	return vec2(left.x - right.x, left.y - right.y);
}

inline vec2 operator*(const vec2 &left, float32_t right)
{
	return vec2(left.x * right, left.y * right);
}

inline vec2 operator*(const vec2 &left, const vec2 &right)
{
	return vec2(left.x * right.x, left.y * right.y);
}

inline bool operator==(const vec2 &left, const vec2 &right)
{
	return (
		((left.x - right.x) <= g_epsilon) &&
		((left.y - right.y) <= g_epsilon)
		);
}

inline bool operator>=(const vec2 &left, const vec2 &right)
{
	return (
		((left.x - right.x) >= g_epsilon) &&
		((left.y - right.y) >= g_epsilon)
		);
}

inline bool operator>(const vec2 &left, const vec2 &right)
{
	return (
		((left.x - right.x) > g_epsilon) &&
		((left.y - right.y) > g_epsilon)
		);
}

inline vec2 getMin(const vec2 &left, const vec2 &right)
{
	return left >= right ? right : left;
}

inline vec2 getMax(const vec2 &left, const vec2 &right)
{
	return left >= right ? left : right;
}

inline vec2 normalize(const vec2 &targetVec)
{
	vec2 result(targetVec);
	result.normalize();

	return result;
}

inline float32_t dot(const vec2 &left, const vec2 &right)
{
	return left.x * right.x + left.y * right.y;
}

inline vec2 cross(const vec2 &left, const vec2 &right)
{
	float32_t value = left.x * right.y - left.y * right.x;
	return vec2(value, value);
}

inline vec2 negate(const vec2 &targetVec)
{
	vec2 result(targetVec);
	result.negate();

	return result;
}
#endif

namespaceEnd(math)
namespaceEnd(gear)
