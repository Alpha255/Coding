#pragma once

#include "Vector3.h"

namespaceStart(Gear)
namespaceStart(Math)

class Vec4 : public DirectX::XMFLOAT4A
{
public:
	inline Vec4()
		: DirectX::XMFLOAT4A(0.0f, 0.0f, 0.0f, 0.0f)
	{
	}

	inline Vec4(float32_t x, float32_t y, float32_t z, float32_t w)
		: DirectX::XMFLOAT4A(x, y, z, w)
	{
	}

	inline Vec4(float32_t value)
		: DirectX::XMFLOAT4A(value, value, value, value)
	{
	}

	inline Vec4(const float32_t* pArray)
		: DirectX::XMFLOAT4A(pArray)
	{
	}

	inline Vec4(const Vec2 &other, float32_t z = 0.0f, float32_t w = 0.0f)
		: DirectX::XMFLOAT4A(other.x, other.y, z, w)
	{
	}

	inline Vec4(const Vec3 &other, float32_t w = 0.0f)
		: DirectX::XMFLOAT4A(other.x, other.y, other.z, w)
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
		float32_t factor = 0.0f;
		float32_t len = length();
		if (len > 0.0f)
		{
			factor = 1.0f / len;
		}
		x *= factor;
		y *= factor;
		z *= factor;
		w *= factor;
	}

	inline void cross(const Vec4 &right)
	{
		Vec3 tempLeft(x, y, z);
		Vec3 tempRight(right.x, right.y, right.z);
		tempLeft.cross(tempRight);

		x = tempLeft.x;
		y = tempLeft.y;
		z = tempLeft.z;
		w = 0.0f;
	}

	inline float32_t dot(const Vec4 &right) const
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

	inline void operator+=(const Vec4 &right)
	{
		x += right.x;
		y += right.y;
		z += right.z;
		w += right.w;
	}

	inline void operator-=(const Vec4 &right)
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

	inline void operator*=(const Vec4 &right)
	{
		x *= right.x;
		y *= right.y;
		z *= right.z;
		w *= right.w;
	}
#endif
};

#if defined(UsingSSE)
	vecPublicFuncsA(4)
#else
inline Vec4 operator+(const Vec4 &left, const Vec4 &right)
{
	return Vec4(left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w);
}

inline Vec4 operator-(const Vec4 &left, const Vec4 &right)
{
	return Vec4(left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w);
}

inline Vec4 operator*(const Vec4 &left, float32_t right)
{
	return Vec4(left.x * right, left.y * right, left.z * right, left.w * right);
}

inline Vec4 operator*(const Vec4 &left, const Vec4 &right)
{
	return Vec4(left.x * right.x, left.y * right.y, left.z * right.z, left.w * right.w);
}

inline bool operator==(const Vec4 &left, const Vec4 &right)
{
	return (
		((left.x - right.x) <= g_epsilon) &&
		((left.y - right.y) <= g_epsilon) &&
		((left.z - right.z) <= g_epsilon) &&
		((left.w - right.w) <= g_epsilon)
		);
}

inline bool operator>=(const Vec4 &left, const Vec4 &right)
{
	return (
		((left.x - right.x) >= g_epsilon) &&
		((left.y - right.y) >= g_epsilon) &&
		((left.z - right.z) >= g_epsilon) &&
		((left.w - right.w) >= g_epsilon)
		);
}

inline bool operator>(const Vec4 &left, const Vec4 &right)
{
	return (
		((left.x - right.x) > g_epsilon) &&
		((left.y - right.y) > g_epsilon) &&
		((left.z - right.z) > g_epsilon) &&
		((left.w - right.w) > g_epsilon)
		);
}

inline Vec4 getMin(const Vec4 &left, const Vec4 &right)
{
	return left >= right ? right : left;
}

inline Vec4 getMax(const Vec4 &left, const Vec4 &right)
{
	return left >= right ? left : right;
}

inline Vec4 normalize(const Vec4 &targetVec)
{
	Vec4 result(targetVec);
	result.normalize();

	return result;
}

inline float32_t dot(const Vec4 &left, const Vec4 &right)
{
	return left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
}

inline Vec4 cross(const Vec4 &left, const Vec4 &right)
{
	Vec3 tempLeft(left.x, left.y, left.z);
	Vec3 tempRight(right.x, right.y, right.z);
	tempLeft.cross(tempRight);

	return Vec4(tempLeft);
}

inline Vec4 negate(const Vec4 &targetVec)
{
	Vec4 result(targetVec);
	result.negate();

	return result;
}
#endif

namespaceEnd(Math)
namespaceEnd(Gear)
