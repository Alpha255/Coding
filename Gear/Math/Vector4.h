#pragma once

#include "Gear/Math/Vector3.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

class Vec4 : public Float4
{
public:
	inline Vec4()
		: Float4(0.0f, 0.0f, 0.0f, 0.0f)
	{
	}

	inline Vec4(float32_t x, float32_t y, float32_t z, float32_t w)
		: Float4(x, y, z, w)
	{
	}

	inline Vec4(float32_t value)
		: Float4(value, value, value, value)
	{
	}

	inline Vec4(const float32_t* pArray)
		: Float4(pArray)
	{
	}

	inline Vec4(const Vec2& other, float32_t z = 0.0f, float32_t w = 0.0f)
		: Float4(other.x, other.y, z, w)
	{
	}

	inline Vec4(const Vec3& other, float32_t w = 0.0f)
		: Float4(other.x, other.y, other.z, w)
	{
	}

#if defined(USE_SSE)
	void transform(const class Matrix& trans);

	inline Vec4 cross(const Vec4& other)
	{
		Vec4 Result;
		VECTOR_STORE(4, &Result, DirectX::XMVector3Cross(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &other)));
		return Result;
	}

	inline Vec4 cross(const Vec4& v1, const Vec4& v2)
	{
		Vec4 Result;
		VECTOR_STORE(4, &Result, DirectX::XMVector4Cross(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &v1), VECTOR_LOAD(4, &v2)));
		return Result;
	}

	VECTOR_MEMBER_FUNCTIONS(4)
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

#if defined(USE_SSE)
	VECTOR_GLOBAL_FUNCTIONS(4)

	inline Vec4 cross(const Vec4& v0, const Vec4& v1)
	{
		Vec4 Result;
		VECTOR_STORE(4, &Result, DirectX::XMVector3Cross(VECTOR_LOAD(4, &v0), VECTOR_LOAD(4, &v1)));
		return Result;
	}

	inline Vec4 cross(const Vec4& v0, const Vec4& v1, const Vec4& v2)
	{        
		Vec4 Result;
		VECTOR_STORE(4, &Result, DirectX::XMVector4Cross(VECTOR_LOAD(4, &v0), VECTOR_LOAD(4, &v1), VECTOR_LOAD(4, &v2)));
		return Result;
	}
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

NAMESPACE_END(Math)
NAMESPACE_END(Gear)
