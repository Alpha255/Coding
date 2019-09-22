#pragma once

#include "Vector4.h"

namespaceStart(gear)
namespaceStart(math)

class matrix : public DirectX::XMFLOAT4X4A
{
public:
	inline matrix()
	{
		identity();
	}

	inline matrix(
		float32_t m00, float32_t m01, float32_t m02, float32_t m03,
		float32_t m10, float32_t m11, float32_t m12, float32_t m13,
		float32_t m20, float32_t m21, float32_t m22, float32_t m23,
		float32_t m30, float32_t m31, float32_t m32, float32_t m33)
		: DirectX::XMFLOAT4X4A(
			m00, m01, m02, m03,
			m10, m11, m12, m13,
			m20, m21, m22, m23,
			m30, m31, m32, m33)
	{
	}

	inline matrix(const vec4 &row0, const vec4 &row1, const vec4 &row2, const vec4 &row3)
		: matrix(
			row0.x, row0.y, row0.z, row0.w,
			row1.x, row1.y, row1.z, row1.w,
			row2.x, row2.y, row2.z, row2.w,
			row3.x, row3.y, row3.z, row3.w)
	{
	}

	inline matrix(const vec4 &row)
		: matrix(row, row, row, row)
	{
	}

#if defined(UsingSSE)
	inline void identity()
	{
		DirectX::XMMATRIX iMatrix = DirectX::XMMatrixIdentity();
		DirectX::XMStoreFloat4x4A(this, iMatrix);
	}

	inline void transpose()
	{
		DirectX::XMMATRIX temp = DirectX::XMLoadFloat4x4A(this);
		DirectX::XMMATRIX result = DirectX::XMMatrixTranspose(temp);
		DirectX::XMStoreFloat4x4A(this, result);
	}

	inline void inverse()
	{
		DirectX::XMMATRIX temp = DirectX::XMLoadFloat4x4A(this);
		DirectX::XMMATRIX result = DirectX::XMMatrixInverse(nullptr, temp);
		DirectX::XMStoreFloat4x4A(this, result);
	}

	inline void inverseTranspose()
	{
		/// Inverse-transpose is just applied to normals.  So zero out 
		/// translation row so that it doesn't get into our inverse-transpose
		/// calculation--we don't want the inverse-transpose of the translation.
		_41 = 0.0f;
		_42 = 0.0f;
		_43 = 0.0f;
		_44 = 1.0f;

		DirectX::XMMATRIX temp = DirectX::XMLoadFloat4x4A(this);
		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(temp);
		DirectX::XMMATRIX result = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, temp));
		DirectX::XMStoreFloat4x4A(this, result);
	}

	inline void translate(float32_t x, float32_t y, float32_t z)
	{
		DirectX::XMMATRIX vResult = DirectX::XMMatrixTranslation(x, y, z);
		DirectX::XMStoreFloat4x4A(this, vResult);
	}

	inline void translate(const vec3 &translation)
	{
		translate(translation.x, translation.y, translation.z);
	}

	inline void scale(float32_t x, float32_t y, float32_t z)
	{
		DirectX::XMMATRIX vResult = DirectX::XMMatrixScaling(x, y, z);
		DirectX::XMStoreFloat4x4A(this, vResult);
	}

	inline void scale(float32_t factor)
	{
		scale(factor, factor, factor);
	}

	inline void rotate(float32_t x, float32_t y, float32_t z, float32_t angle)
	{
		DirectX::XMVECTOR axis = DirectX::XMVectorSet(x, y, z, 0.0f);
		DirectX::XMMATRIX vResult = DirectX::XMMatrixRotationAxis(axis, angle * DirectX::XM_PI / 180.0f);
		DirectX::XMStoreFloat4x4A(this, vResult);
	}

	inline void rotate(const vec3 &axis, float32_t angle)
	{
		rotate(axis.x, axis.y, axis.z, angle);
	}

	inline void rotateX(float32_t angle)
	{
		DirectX::XMMATRIX vResult = DirectX::XMMatrixRotationX(angle * DirectX::XM_PI / 180.0f);
		DirectX::XMStoreFloat4x4A(this, vResult);
	}

	inline void rotateY(float32_t angle)
	{
		DirectX::XMMATRIX vResult = DirectX::XMMatrixRotationY(angle * DirectX::XM_PI / 180.0f);
		DirectX::XMStoreFloat4x4A(this, vResult);
	}

	inline void rotateZ(float32_t angle)
	{
		DirectX::XMMATRIX vResult = DirectX::XMMatrixRotationZ(angle * DirectX::XM_PI / 180.0f);
		DirectX::XMStoreFloat4x4A(this, vResult);
	}

	inline void operator*=(const matrix &right)
	{
		DirectX::XMMATRIX vLeft = DirectX::XMLoadFloat4x4A(this);
		DirectX::XMMATRIX vRight = DirectX::XMLoadFloat4x4A(&right);
		DirectX::XMMATRIX vResult = DirectX::XMMatrixMultiply(vLeft, vRight);
		DirectX::XMStoreFloat4x4A(this, vResult);
	}

	inline void operator+=(const matrix &right)
	{
		DirectX::XMMATRIX vLeft = DirectX::XMLoadFloat4x4A(this);
		DirectX::XMMATRIX vRight = DirectX::XMLoadFloat4x4A(&right);
		DirectX::XMMATRIX vResult = DirectX::XMMatrixMultiply(vLeft, vRight);
		DirectX::XMStoreFloat4x4A(this, vResult);
	}

	inline static matrix transpose(const matrix &targetMatrix)
	{
		matrix result;

		DirectX::XMMATRIX temp = DirectX::XMLoadFloat4x4A(&targetMatrix);
		DirectX::XMMATRIX vResult = DirectX::XMMatrixTranspose(temp);
		DirectX::XMStoreFloat4x4A(&result, vResult);

		return result;
	}

	inline static matrix inverse(const matrix &targetMatrix)
	{
		matrix result;

		DirectX::XMMATRIX temp = DirectX::XMLoadFloat4x4A(&targetMatrix);
		DirectX::XMMATRIX vResult = DirectX::XMMatrixInverse(nullptr, temp);
		DirectX::XMStoreFloat4x4A(&result, vResult);

		return result;
	}

	inline static matrix inverseTranspose(const matrix &targetMatrix)
	{
		/// Inverse-transpose is just applied to normals.  So zero out 
		/// translation row so that it doesn't get into our inverse-transpose
		/// calculation--we don't want the inverse-transpose of the translation.
		matrix result;

		matrix copy = targetMatrix;
		copy._41 = 0.0f;
		copy._42 = 0.0f;
		copy._43 = 0.0f;
		copy._44 = 1.0f;

		DirectX::XMMATRIX temp = DirectX::XMLoadFloat4x4A(&copy);
		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(temp);
		DirectX::XMMATRIX vResult = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, temp));
		DirectX::XMStoreFloat4x4A(&result, vResult);

		return result;
	}

	inline static matrix perspectiveFovLH(float32_t fov, float32_t aspect, float32_t nearPlane, float32_t farPlane)
	{
		matrix result;

		DirectX::XMMATRIX vResult = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, nearPlane, farPlane);
		DirectX::XMStoreFloat4x4A(&result, vResult);

		return result;
	}

	inline static matrix OrthographicLH(float32_t width, float32_t height, float32_t nearPlane, float32_t farPlane)
	{
		matrix result;

		DirectX::XMMATRIX vResult = DirectX::XMMatrixOrthographicLH(width, height, nearPlane, farPlane);
		DirectX::XMStoreFloat4x4A(&result, vResult);

		return result;
	}

	inline static matrix lookAtLH(const vec3 &eye, const vec3 &lookAt, const vec3 &up)
	{
		matrix result;

		DirectX::XMVECTOR vEye = DirectX::XMVectorSet(eye.x, eye.y, eye.z, 1.0f);
		DirectX::XMVECTOR vLookAt = DirectX::XMVectorSet(lookAt.x, lookAt.y, lookAt.z, 0.0f);
		DirectX::XMVECTOR vUp = DirectX::XMVectorSet(up.x, up.y, up.z, 0.0f);
		DirectX::XMMATRIX vResult = DirectX::XMMatrixLookAtLH(vEye, vLookAt, vUp);
		DirectX::XMStoreFloat4x4A(&result, vResult);

		return result;
	}
#else
	inline void identity()
	{
		_11 = 1.0f; _12 = 0.0f; _13 = 0.0f; _14 = 0.0f;
		_21 = 0.0f; _22 = 1.0f; _23 = 0.0f; _24 = 0.0f;
		_31 = 0.0f; _32 = 0.0f; _33 = 1.0f; _34 = 0.0f;
		_41 = 0.0f; _42 = 0.0f; _43 = 0.0f; _44 = 1.0f;
	}

	inline void transpose()
	{
		matrix temp(
			_11, _21, _31, _41,
			_12, _22, _32, _42,
			_13, _23, _33, _43,
			_14, _24, _34, _44
		);

		*this = temp;
	}

	inline void inverse()
	{
	}

	inline void inverseTranspose()
	{
	}

	inline void translate(float32_t x, float32_t y, float32_t z)
	{
		_41 = x;
		_42 = y;
		_43 = z;
	}

	inline void translate(const vec3 &translation)
	{
		translate(translation.x, translation.y, translation.z);
	}

	inline void scale(float32_t value)
	{
		scale(value, value, value);
	}

	inline void scale(const vec3 &scalling)
	{
		scale(scalling.x, scalling.y, scalling.z);
	}

	inline void scale(float32_t x, float32_t y, float32_t z)
	{
		_11 = x;
		_22 = y;
		_33 = z;
	}

	inline void rotate(float32_t x, float32_t y, float32_t z, float32_t angle)
	{
		vec3 axis(x, y, z);
		axis.normalize();

		float32_t cosTheta = ::cosf(angle * 180.0f / DirectX::XM_PI);
		float32_t sinTheta = ::sinf(angle * 180.0f / DirectX::XM_PI);

		_11 = axis.x * axis.x * (1.0f - cosTheta) + cosTheta;
		_12 = axis.x * axis.y * (1.0f - cosTheta) + axis.z * sinTheta;
		_13 = axis.x * axis.z * (1.0f - cosTheta) - axis.y * sinTheta;
		_14 = 0.0f;

		_21 = axis.x * axis.y * (1.0f - cosTheta) - axis.z * sinTheta;
		_22 = axis.y * axis.y * (1.0f - cosTheta) + cosTheta;
		_23 = axis.y * axis.z * (1.0f - cosTheta) + axis.x * sinTheta;
		_33 = 0.0f;

		_31 = axis.x * axis.z * (1.0f - cosTheta) + axis.y * sinTheta;
		_32 = axis.y * axis.z * (1.0f - cosTheta) - axis.x * sinTheta;
		_33 = axis.z * axis.z * (1.0f - cosTheta) + cosTheta;
		_34 = 0.0f;

		_41 = 0.0f;
		_42 = 0.0f;
		_43 = 0.0f;
		_43 = 1.0f;
	}

	inline void rotate(const vec3 &axis, float32_t angle)
	{
		rotate(axis.x, axis.y, axis.z, angle);
	}

	inline void rotateX(float32_t angle)
	{
		float32_t cosTheta = ::cosf(angle * 180.0f / DirectX::XM_PI);
		float32_t sinTheta = ::sinf(angle * 180.0f / DirectX::XM_PI);

		_11 = 1.0f; _12 = 0.0f;      _13 = 0.0f;     _14 = 0.0f;
		_12 = 0.0f; _22 = cosTheta;  _23 = sinTheta; _24 = 0.0f;
		_31 = 0.0f; _32 = -sinTheta; _33 = cosTheta; _34 = 0.0f;
		_41 = 0.0f; _42 = 0.0f;      _43 = 0.0f;     _44 = 1.0f;
	}

	inline void rotateY(float32_t angle)
	{
		float32_t cosTheta = ::cosf(angle * 180.0f / DirectX::XM_PI);
		float32_t sinTheta = ::sinf(angle * 180.0f / DirectX::XM_PI);

		_11 = cosTheta; _12 = 0.0f; _13 = -sinTheta; _14 = 0.0f;
		_12 = 0.0f;     _22 = 1.0f; _23 = 0.0f;      _24 = 0.0f;
		_31 = sinTheta; _32 = 0.0f; _33 = cosTheta;  _34 = 0.0f;
		_41 = 0.0f;     _42 = 0.0f; _43 = 0.0f;      _44 = 1.0f;
	}

	inline void rotateZ(float32_t angle)
	{
		float32_t cosTheta = ::cosf(angle * 180.0f / DirectX::XM_PI);
		float32_t sinTheta = ::sinf(angle * 180.0f / DirectX::XM_PI);

		_11 = cosTheta;  _12 = sinTheta; _13 = 0.0f; _14 = 0.0f;
		_12 = -sinTheta; _22 = cosTheta; _23 = 0.0f; _24 = 0.0f;
		_31 = 0.0f;      _32 = 0.0f;     _33 = 1.0f; _34 = 0.0f;
		_41 = 0.0f;      _42 = 0.0f;     _43 = 0.0f; _44 = 1.0f;
	}

	inline void operator*=(const matrix &right)
	{
		vec4 row(_11, _12, _13, _14);
		_11 = row.x * right._11 + row.y * right._21 + row.z * right._31 + row.w * right._41;
		_12 = row.x * right._12 + row.y * right._22 + row.z * right._32 + row.w * right._42;
		_13 = row.x * right._13 + row.y * right._23 + row.z * right._33 + row.w * right._43;
		_14 = row.x * right._14 + row.y * right._24 + row.z * right._34 + row.w * right._44;

		row = { _21, _22, _23, _24 };
		_21 = row.x * right._11 + row.y * right._21 + row.z * right._31 + row.w * right._41;
		_22 = row.x * right._12 + row.y * right._22 + row.z * right._32 + row.w * right._42;
		_23 = row.x * right._13 + row.y * right._23 + row.z * right._33 + row.w * right._43;
		_24 = row.x * right._14 + row.y * right._24 + row.z * right._34 + row.w * right._44;

		row = { _31, _32, _33, _34 };
		_31 = row.x * right._11 + row.y * right._21 + row.z * right._31 + row.w * right._41;
		_32 = row.x * right._12 + row.y * right._22 + row.z * right._32 + row.w * right._42;
		_33 = row.x * right._13 + row.y * right._23 + row.z * right._33 + row.w * right._43;
		_34 = row.x * right._14 + row.y * right._24 + row.z * right._34 + row.w * right._44;

		row = { _41, _42, _43, _44 };
		_41 = row.x * right._11 + row.y * right._21 + row.z * right._31 + row.w * right._41;
		_42 = row.x * right._12 + row.y * right._22 + row.z * right._32 + row.w * right._42;
		_42 = row.x * right._13 + row.y * right._23 + row.z * right._33 + row.w * right._43;
		_44 = row.x * right._14 + row.y * right._24 + row.z * right._34 + row.w * right._44;
	}

	inline void operator*=(const float32_t factor)
	{
		_11 *= factor; _12 *= factor; _13 *= factor; _14 *= factor;
		_21 *= factor; _22 *= factor; _23 *= factor; _24 *= factor;
		_31 *= factor; _32 *= factor; _33 *= factor; _34 *= factor;
		_41 *= factor; _42 *= factor; _43 *= factor; _44 *= factor;
	}

	inline void operator+=(const matrix &right)
	{
		_11 += right._11; _12 += right._12; _13 += right._13; _14 += right._14;
		_21 += right._21; _22 += right._22; _23 += right._23; _24 += right._24;
		_31 += right._31; _32 += right._32; _33 += right._33; _34 += right._34;
		_41 += right._41; _42 += right._42; _43 += right._43; _44 += right._44;
	}

	inline void operator-=(const matrix &right)
	{
		_11 -= right._11; _12 -= right._12; _13 -= right._13; _14 -= right._14;
		_21 -= right._21; _22 -= right._22; _23 -= right._23; _24 -= right._24;
		_31 -= right._31; _32 -= right._32; _33 -= right._33; _34 -= right._34;
		_41 -= right._41; _42 -= right._42; _43 -= right._43; _44 -= right._44;
	}

	inline static matrix transpose(const matrix &targetMatrix)
	{
		matrix result(targetMatrix);
		result.transpose();

		return result;
	}

	inline static matrix inverse(const matrix &targetMatrix)
	{
		matrix result(targetMatrix);
		result.inverse();

		return result;
	}

	inline static matrix inverseTranspose(const matrix &targetMatrix)
	{
		matrix result(targetMatrix);
		result.inverseTranspose();

		return result;
	}

	inline static matrix perspectiveFovLH(float32_t fov, float32_t aspect, float32_t nearPlane, float32_t farPlane)
	{
		matrix result;

		return result;
	}

	inline static matrix OrthographicLH(float32_t width, float32_t height, float32_t nearPlane, float32_t farPlane)
	{
		matrix result;

		return result;
	}

	inline static matrix lookAtLH(const vec3 &eye, const vec3 &lookAt, const vec3 &up)
	{
		vec3 dir = normalize(lookAt - eye);
		vec3 right = cross(normalize(up), dir);
		vec3 newUp = cross(dir, right);

		matrix result(
			vec4(right.x, newUp.x, dir.x, eye.x), 
			vec4(right.y, newUp.y, dir.y, eye.y), 
			vec4(right.z, newUp.z, dir.z, eye.z), 
			vec4(0.0f, 0.0f, 0.0f, 1.0f)
		);
		return result;
	}
#endif
protected:
private:
};

#if defined(UsingSSE)
inline matrix operator*(const matrix &left, const matrix &right)
{
	matrix result;

	DirectX::XMMATRIX vLeft = DirectX::XMLoadFloat4x4A(&left);
	DirectX::XMMATRIX vRight = DirectX::XMLoadFloat4x4A(&right);
	DirectX::XMMATRIX vResult = vLeft * vRight;
	DirectX::XMStoreFloat4x4A(&result, vResult);

	return result;
}
#else
inline matrix operator*(const matrix &left, const matrix &right)
{
	matrix result(left);
	result *= right;

	return result;
}

inline vec4 operator*(const vec4 &left, const matrix &right)
{
	vec4 result;
	result.x = left.x * right._11 + left.y * right._21 + left.z * right._31 + left.w * right._41;
	result.y = left.x * right._12 + left.y * right._22 + left.z * right._32 + left.w * right._42;
	result.z = left.x * right._13 + left.y * right._23 + left.z * right._33 + left.w * right._43;
	result.w = left.x * right._14 + left.y * right._24 + left.z * right._34 + left.w * right._44;

	return result;
}
#endif

namespaceEnd(math)
namespaceEnd(gear)
