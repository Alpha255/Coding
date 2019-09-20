#pragma once

#include "Vector.h"

namespaceStart(gear)
namespaceStart(math)

class vec4 : public DirectX::XMFLOAT4A
{
public:
	inline vec4()
		: DirectX::XMFLOAT4A(0.0f, 0.0f, 0.0f, 0.0f)
	{
	}

	inline vec4(float32_t x, float32_t y, float32_t z, float32_t w)
		: DirectX::XMFLOAT4A(x, y, z, w)
	{
	}

	inline vec4(const float32_t* pArray)
		: DirectX::XMFLOAT4A(pArray)
	{
	}

#if defined(UsingSSE)
	vecMemberFuncsA(4)
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

	inline void operator*=(const float32_t right)
	{
		x *= right;
		y *= right;
		z *= right;
		w *= right;
	}

	inline float32_t operator*=(const vec4 &right)
	{
		return x * right.x + y * right.y + z * right.z + w * right.w;
	}
#endif
};

#if defined(UsingSSE)
	vecPublicFuncsA(4)
#else
inline vec4 operator+(const vec4 &left, const vec4 &right)
{
	return vec4(left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w);
}

inline vec4 operator-(const vec4 &left, const vec4 &right)
{
	return vec4(left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w);
}

inline float32_t operator*(const vec4 &left, const vec4 &right)
{
	return (left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w);
}

inline vec4 operator*(const vec4 &left, float32_t right)
{
	return vec4(left.x * right, left.y * right, left.z * right, left.w * right);
}
#endif

namespaceEnd(math)
namespaceEnd(gear)