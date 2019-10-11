#pragma once

#include "Vector2.h"

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

	inline vec3(float32_t value)
		: DirectX::XMFLOAT3A(value, value, value)
	{
	}

	inline vec3(const float32_t* pArray)
		: DirectX::XMFLOAT3A(pArray)
	{
	}

	inline vec3(const vec2 &other, float32_t z = 0.0f)
		: DirectX::XMFLOAT3A(other.x, other.y, z)
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

	inline void cross(const vec3 &right)
	{
		float32_t vx = y * right.z - z * right.y;
		float32_t vy = z * right.x - x * right.z;
		float32_t vz = x * right.y - y * right.x;

		x = vx;
		y = vy;
		z = vz;
	}

	inline float32_t dot(const vec3 &right) const
	{
		return x * right.x + y * right.y + z * right.z;
	}

	inline void negate()
	{
		x = -x;
		y = -y;
		z = -z;
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

	inline void operator*=(float32_t right)
	{
		x *= right;
		y *= right;
		z *= right;
	}

	inline void operator*=(const vec3 &right)
	{
		x *= right.x;
		y *= right.y;
		z *= right.z;
	}
#endif

	static vec3 transformCoord(const vec3 &src, const class matrix &mat);
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

inline vec3 operator*(const vec3 &left, float32_t right)
{
	return vec3(left.x * right, left.y * right, left.z * right);
}

inline vec3 operator*(const vec3 &left, const vec3 &right)
{
	return vec3(left.x * right.x, left.y * right.y, left.z * right.z);
}

inline bool operator==(const vec3 &left, const vec3 &right)
{
	return (
		((left.x - right.x) <= g_epsilon) &&
		((left.y - right.y) <= g_epsilon) &&
		((left.z - right.z) <= g_epsilon)
		);
}

inline bool operator>=(const vec3 &left, const vec3 &right)
{
	return (
		((left.x - right.x) >= g_epsilon) &&
		((left.y - right.y) >= g_epsilon) &&
		((left.z - right.z) >= g_epsilon)
		);
}

inline bool operator>(const vec3 &left, const vec3 &right)
{
	return (
		((left.x - right.x) > g_epsilon) &&
		((left.y - right.y) > g_epsilon) &&
		((left.z - right.z) > g_epsilon)
		);
}

inline vec3 getMin(const vec3 &left, const vec3 &right)
{
	return left >= right ? right : left;
}

inline vec3 getMax(const vec3 &left, const vec3 &right)
{
	return left >= right ? left : right;
}

inline vec3 normalize(const vec3 &targetVec)
{
	vec3 result(targetVec);
	result.normalize();

	return result;
}

inline float32_t dot(const vec3 &left, const vec3 &right)
{
	return left.x * right.x + left.y * right.y + left.z * right.z;
}

inline vec3 cross(const vec3 &left, const vec3 &right)
{
	float32_t vx = left.y * right.z - left.z * right.y;
	float32_t vy = left.z * right.x - left.x * right.z;
	float32_t vz = left.x * right.y - left.y * right.x;

	return vec3(vx, vy, vz);
}

inline vec3 negate(const vec3 &targetVec)
{
	vec3 result(targetVec);
	result.negate();

	return result;
}
#endif

namespaceEnd(math)
namespaceEnd(gear)