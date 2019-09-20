#pragma once

#include "Vector.h"

namespaceStart(gear)
namespaceStart(math)

class vec3 : public DirectX::XMFLOAT3A
{
public:
	inline vec3()
		: DirectX::XMFLOAT3A(0.0f, 0.0f, 0.0f)
	{
	}

	inline vec3(float32_t x, float32_t y, float32_t z)
		: DirectX::XMFLOAT3A(x, y, z)
	{
	}

	inline vec3(const float32_t* pArray)
		: DirectX::XMFLOAT3A(pArray)
	{
	}

#if defined(UsingSSE)
	vecMemberFuncsA(3)
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
		float32_t factor = 1.0f / length();
		x *= factor;
		y *= factor;
		z *= factor;
	}

	inline void operator+=(const vec3 &right)
	{
		x += right.x;
		y += right.y;
		z += right.z;
	}

	inline void operator-=(const vec3 &right)
	{
		x -= right.x;
		y -= right.y;
		z -= right.z;
	}

	inline void operator*=(const float32_t right)
	{
		x *= right;
		y *= right;
		z *= right;
	}

	inline float32_t operator*=(const vec3 &right)
	{
		return x * right.x + y * right.y + z * right.z;
	}
#endif
};

#if defined(UsingSSE)
	vecPublicFuncsA(3)
#else
inline vec3 operator+(const vec3 &left, const vec3 &right)
{
	return vec3(left.x + right.x, left.y + right.y, left.z + right.z);
}

inline vec3 operator-(const vec3 &left, const vec3 &right)
{
	return vec3(left.x - right.x, left.y - right.y, left.z - right.z);
}

inline float32_t operator*(const vec3 &left, const vec3 &right)
{
	return (left.x * right.x + left.y * right.y + left.z * right.z);
}

inline vec3 operator*(const vec3 &left, float32_t right)
{
	return vec3(left.x * right, left.y * right, left.z * right);
}
#endif

namespaceEnd(math)
namespaceEnd(gear)