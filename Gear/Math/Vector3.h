#pragma once

#include "Gear/Math/Vector2.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

class Vec3 : public Float3
{
public:
	inline Vec3()
		: Float3(0.0f, 0.0f, 0.0f)
	{
	}

	inline Vec3(float32_t x, float32_t y, float32_t z)
		: Float3(x, y, z)
	{
	}

	inline Vec3(float32_t value)
		: Float3(value, value, value)
	{
	}

	inline Vec3(const float32_t* pArray)
		: Float3(pArray)
	{
	}

	inline Vec3(const Vec2& other, float32_t z = 0.0f)
		: Float3(other.x, other.y, z)
	{
	}

#if defined(USE_SSE)
	VECTOR_MEMBER_FUNCTIONS_TRANSFORM_DECLARE(3)
	VECTOR_MEMBER_FUNCTIONS(3)
	VECTOR_MEMBER_FUNCTIONS_CROSS(3)
#else
	inline float32_t lengthSq()
	{
		return x * x + y * y + z * z;
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
		z *= factor;
	}

	inline void cross(const Vec3 &right)
	{
		float32_t vx = y * right.z - z * right.y;
		float32_t vy = z * right.x - x * right.z;
		float32_t vz = x * right.y - y * right.x;

		x = vx;
		y = vy;
		z = vz;
	}

	inline float32_t dot(const Vec3 &right) const
	{
		return x * right.x + y * right.y + z * right.z;
	}

	inline void negate()
	{
		x = -x;
		y = -y;
		z = -z;
	}

	inline void operator+=(const Vec3 &right)
	{
		x += right.x;
		y += right.y;
		z += right.z;
	}

	inline void operator-=(const Vec3 &right)
	{
		x -= right.x;
		y -= right.y;
		z -= right.z;
	}

	inline void operator*=(float32_t right)
	{
		x *= right;
		y *= right;
		z *= right;
	}

	inline void operator*=(const Vec3 &right)
	{
		x *= right.x;
		y *= right.y;
		z *= right.z;
	}
#endif
};

#if defined(USE_SSE)
	VECTOR_GLOBAL_FUNCTIONS(3)
	VECTOR_GLOBAL_FUNCTIONS_CROSS(3)
#else
inline Vec3 operator+(const Vec3 &left, const Vec3 &right)
{
	return Vec3(left.x + right.x, left.y + right.y, left.z + right.z);
}

inline Vec3 operator-(const Vec3 &left, const Vec3 &right)
{
	return Vec3(left.x - right.x, left.y - right.y, left.z - right.z);
}

inline Vec3 operator*(const Vec3 &left, float32_t right)
{
	return Vec3(left.x * right, left.y * right, left.z * right);
}

inline Vec3 operator*(const Vec3 &left, const Vec3 &right)
{
	return Vec3(left.x * right.x, left.y * right.y, left.z * right.z);
}

inline bool operator==(const Vec3 &left, const Vec3 &right)
{
	return (
		((left.x - right.x) <= g_epsilon) &&
		((left.y - right.y) <= g_epsilon) &&
		((left.z - right.z) <= g_epsilon)
		);
}

inline bool operator>=(const Vec3 &left, const Vec3 &right)
{
	return (
		((left.x - right.x) >= g_epsilon) &&
		((left.y - right.y) >= g_epsilon) &&
		((left.z - right.z) >= g_epsilon)
		);
}

inline bool operator>(const Vec3 &left, const Vec3 &right)
{
	return (
		((left.x - right.x) > g_epsilon) &&
		((left.y - right.y) > g_epsilon) &&
		((left.z - right.z) > g_epsilon)
		);
}

inline Vec3 getMin(const Vec3 &left, const Vec3 &right)
{
	return left >= right ? right : left;
}

inline Vec3 getMax(const Vec3 &left, const Vec3 &right)
{
	return left >= right ? left : right;
}

inline Vec3 normalize(const Vec3 &targetVec)
{
	Vec3 result(targetVec);
	result.normalize();

	return result;
}

inline float32_t dot(const Vec3 &left, const Vec3 &right)
{
	return left.x * right.x + left.y * right.y + left.z * right.z;
}

inline Vec3 cross(const Vec3 &left, const Vec3 &right)
{
	float32_t vx = left.y * right.z - left.z * right.y;
	float32_t vy = left.z * right.x - left.x * right.z;
	float32_t vz = left.x * right.y - left.y * right.x;

	return Vec3(vx, vy, vz);
}

inline Vec3 negate(const Vec3 &targetVec)
{
	Vec3 result(targetVec);
	result.negate();

	return result;
}
#endif

NAMESPACE_END(Math)
NAMESPACE_END(Gear)
