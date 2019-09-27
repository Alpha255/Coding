#pragma once

#include "Vector3.h"

namespaceStart(gear)
namespaceStart(math)

class vec4 : public DirectX::XMFLOAT4
{
public:
	inline vec4()
		: DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)
	{
	}

	inline vec4(float32_t x, float32_t y, float32_t z, float32_t w)
		: DirectX::XMFLOAT4(x, y, z, w)
	{
	}

	inline vec4(float32_t value)
		: DirectX::XMFLOAT4(value, value, value, value)
	{
	}

	inline vec4(const float32_t* pArray)
		: DirectX::XMFLOAT4(pArray)
	{
	}

	inline vec4(const vec2 &other, float32_t z = 0.0f, float32_t w = 0.0f)
		: DirectX::XMFLOAT4(other.x, other.y, z, w)
	{
	}

	inline vec4(const vec3 &other, float32_t w = 0.0f)
		: DirectX::XMFLOAT4(other.x, other.y, other.z, w)
	{
	}

#if defined(UsingSSE)
	vecMemberFuncs(4)
#else
	inline float32_t lengthSq()
	{
		return x * x + y * y + z * z + w * w;
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
		z *= factor;
		w *= factor;
	}

	inline void cross(const vec4 &right)
	{
		vec3 tempLeft(x, y, z);
		vec3 tempRight(right.x, right.y, right.z);
		tempLeft.cross(tempRight);

		x = tempLeft.x;
		y = tempLeft.y;
		z = tempLeft.z;
		w = 0.0f;
	}

	inline float32_t dot(const vec4 &right) const
	{
		return x * right.x + y * right.y + z * right.z + w * right.w;
	}

	inline void negate()
	{
		x = -x;
		y = -y;
		z = -z;
		w = -w;
	}

	inline void operator+=(const vec4 &right)
	{
		x += right.x;
		y += right.y;
		z += right.z;
		w += right.w;
	}

	inline void operator-=(const vec4 &right)
	{
		x -= right.x;
		y -= right.y;
		z -= right.z;
		w -= right.w;
	}

	inline void operator*=(float32_t right)
	{
		x *= right;
		y *= right;
		z *= right;
		w *= right;
	}

	inline void operator*=(const vec4 &right)
	{
		x *= right.x;
		y *= right.y;
		z *= right.z;
		w *= right.w;
	}
#endif
};

#if defined(UsingSSE)
	vecPublicFuncs(4)
#else
inline vec4 operator+(const vec4 &left, const vec4 &right)
{
	return vec4(left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w);
}

inline vec4 operator-(const vec4 &left, const vec4 &right)
{
	return vec4(left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w);
}

inline vec4 operator*(const vec4 &left, float32_t right)
{
	return vec4(left.x * right, left.y * right, left.z * right, left.w * right);
}

inline vec4 operator*(const vec4 &left, const vec4 &right)
{
	return vec4(left.x * right.x, left.y * right.y, left.z * right.z, left.w * right.w);
}

inline bool operator==(const vec4 &left, const vec4 &right)
{
	return (
		((left.x - right.x) <= g_epsilon) &&
		((left.y - right.y) <= g_epsilon) &&
		((left.z - right.z) <= g_epsilon) &&
		((left.w - right.w) <= g_epsilon)
		);
}

inline bool operator>=(const vec4 &left, const vec4 &right)
{
	return (
		((left.x - right.x) >= g_epsilon) &&
		((left.y - right.y) >= g_epsilon) &&
		((left.z - right.z) >= g_epsilon) &&
		((left.w - right.w) >= g_epsilon)
		);
}

inline bool operator>(const vec4 &left, const vec4 &right)
{
	return (
		((left.x - right.x) > g_epsilon) &&
		((left.y - right.y) > g_epsilon) &&
		((left.z - right.z) > g_epsilon) &&
		((left.w - right.w) > g_epsilon)
		);
}

inline vec4 getMin(const vec4 &left, const vec4 &right)
{
	return left >= right ? right : left;
}

inline vec4 getMax(const vec4 &left, const vec4 &right)
{
	return left >= right ? left : right;
}

inline vec4 normalize(const vec4 &targetVec)
{
	vec4 result(targetVec);
	result.normalize();

	return result;
}

inline float32_t dot(const vec4 &left, const vec4 &right)
{
	return left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
}

inline vec4 cross(const vec4 &left, const vec4 &right)
{
	vec3 tempLeft(left.x, left.y, left.z);
	vec3 tempRight(right.x, right.y, right.z);
	tempLeft.cross(tempRight);

	return vec4(tempLeft);
}

inline vec4 negate(const vec4 &targetVec)
{
	vec4 result(targetVec);
	result.negate();

	return result;
}
#endif

#include "Vector.hpp"

namespaceEnd(math)
namespaceEnd(gear)