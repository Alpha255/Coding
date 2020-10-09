#pragma once

#include "Gear/Math/Vector.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

class Vec2 : public Float2
{
public:
	inline Vec2()
		: Float2(0.0f, 0.0f)
	{
	}

	inline Vec2(float32_t value)
		: Float2(value, value)
	{
	}

	inline Vec2(float32_t x, float32_t y)
		: Float2(x, y)
	{
	}

	inline Vec2(const float32_t* pArray)
		: Float2(pArray)
	{                               
	}

#if defined(USE_SSE)
	VECTOR_MEMBER_FUNCTIONS_TRANSFORM_DECLARE(2)
	VECTOR_MEMBER_FUNCTIONS(2)
	VECTOR_MEMBER_FUNCTIONS_CROSS(2)
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
		float32_t factor = 0.0f;
		float32_t len = length();
		if (len > 0.0f)
		{
			factor = 1.0f / len;
		}
		x *= factor;
		y *= factor;
	}

	inline Vec2 cross(const Vec2& right)
	{
		float32_t value = x * right.y - y * right.x;
		x = y = value;
	}

	inline float32_t dot(const Vec2& right) const
	{
		return x * right.x + y * right.y;
	}

	inline void negate()
	{
		x = -x;
		y = -y;
	}

	inline void operator+=(const Vec2& right)
	{
		x += right.x;
		y += right.y;
	}

	inline void operator-=(const Vec2& right)
	{
		x -= right.x;
		y -= right.y;
	}

	inline void operator*=(float32_t right)
	{
		x *= right;
		y *= right;
	}

	inline void operator*=(const Vec2& right)
	{
		x *= right.x;
		y *= right.y;
	}
#endif
};

#if defined(USE_SSE)
	VECTOR_GLOBAL_FUNCTIONS(2)
	VECTOR_GLOBAL_FUNCTIONS_CROSS(2)
#else
inline Vec2 operator+(const Vec2 &left, const Vec2 &right)
{
	return Vec2(left.x + right.x, left.y + right.y);
}

inline Vec2 operator-(const Vec2 &left, const Vec2 &right)
{
	return Vec2(left.x - right.x, left.y - right.y);
}

inline Vec2 operator*(const Vec2 &left, float32_t right)
{
	return Vec2(left.x * right, left.y * right);
}

inline Vec2 operator*(const Vec2 &left, const Vec2 &right)
{
	return Vec2(left.x * right.x, left.y * right.y);
}

inline bool operator==(const Vec2 &left, const Vec2 &right)
{
	return (
		((left.x - right.x) <= g_epsilon) &&
		((left.y - right.y) <= g_epsilon)
		);
}

inline bool operator>=(const Vec2 &left, const Vec2 &right)
{
	return (
		((left.x - right.x) >= g_epsilon) &&
		((left.y - right.y) >= g_epsilon)
		);
}

inline bool operator>(const Vec2 &left, const Vec2 &right)
{
	return (
		((left.x - right.x) > g_epsilon) &&
		((left.y - right.y) > g_epsilon)
		);
}

inline Vec2 getMin(const Vec2 &left, const Vec2 &right)
{
	return left >= right ? right : left;
}

inline Vec2 getMax(const Vec2 &left, const Vec2 &right)
{
	return left >= right ? left : right;
}

inline Vec2 normalize(const Vec2 &targetVec)
{
	Vec2 result(targetVec);
	result.normalize();

	return result;
}

inline float32_t dot(const Vec2 &left, const Vec2 &right)
{
	return left.x * right.x + left.y * right.y;
}

inline Vec2 cross(const Vec2 &left, const Vec2 &right)
{
	float32_t value = left.x * right.y - left.y * right.x;
	return Vec2(value, value);
}

inline Vec2 negate(const Vec2 &targetVec)
{
	Vec2 result(targetVec);
	result.negate();

	return result;
}
#endif

NAMESPACE_END(Math)
NAMESPACE_END(Gear)
