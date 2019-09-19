#pragma once

#include "Vector.h"

class Vec3 : public DirectX::XMFLOAT3
{
public:
	inline Vec3()
		: DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)
	{
	}

	inline Vec3(float32_t x, float32_t y, float32_t z)
		: DirectX::XMFLOAT3(x, y, z)
	{
	}

	inline Vec3(const float32_t* pArray)
		: DirectX::XMFLOAT3(pArray)
	{
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

	inline void operator*=(const float32_t right)
	{
		x *= right;
		y *= right;
		z *= right;
	}

	inline float32_t operator*=(const Vec3 &right)
	{
		return x * right.x + y * right.y + z * right.z;
	}

	inline Vec3 cross(const Vec3 &right)
	{
		Vec3 result;

		DirectX::XMVECTOR v0 = DirectX::XMLoadFloat3(this);
		DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&right);
		DirectX::XMStoreFloat3(&result, DirectX::XMVector3Cross(v0, v1));

		return result;
	}

	inline void normalize()
	{
		DirectX::XMVECTOR temp = DirectX::XMLoadFloat3(this);
		DirectX::XMVECTOR result = DirectX::XMVector3Normalize(temp);
		DirectX::XMStoreFloat3(this, temp);
	}
};

inline Vec3 operator+(const Vec3 &left, const Vec3 &right)
{
	return Vec3(left.x + right.x, left.y + right.y, left.z + right.z);
}

inline Vec3 operator-(const Vec3 &left, const Vec3 &right)
{
	return Vec3(left.x - right.x, left.y - right.y, left.z - right.z);
}

inline float32_t operator*(const Vec3 &left, const Vec3 &right)
{
	return (left.x * right.x + left.y * right.y + left.z * right.z);
}

inline Vec3 operator*(const Vec3 &left, float32_t right)
{
	return Vec3(left.x * right, left.y * right, left.z * right);
}

inline Vec3 cross(const Vec3 &left, const Vec3 &right)
{
	Vec3 result;

	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat3(&left);
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&right);
	DirectX::XMStoreFloat3(&result, DirectX::XMVector3Cross(v0, v1));

	return result;
}

inline Vec3 getMin(const Vec3 &left, const Vec3 &right)
{
	Vec3 result;

	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat3(&left);
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&right);
	DirectX::XMStoreFloat3(&result, DirectX::XMVectorMin(v0, v1));

	return result;
}

inline Vec3 getMax(const Vec3 &left, const Vec3 &right)
{
	Vec3 result;

	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat3(&left);
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&right);
	DirectX::XMStoreFloat3(&result, DirectX::XMVectorMax(v0, v1));

	return result;
}