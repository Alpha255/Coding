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
		DirectX::XMMATRIX temp = DirectX::XMLoadFloat4x4(this);
		DirectX::XMMATRIX result = DirectX::XMMatrixTranspose(temp);
		DirectX::XMStoreFloat4x4A(this, result);
	}

	inline void inverse()
	{
		DirectX::XMMATRIX temp = DirectX::XMLoadFloat4x4(this);
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

		DirectX::XMMATRIX temp = DirectX::XMLoadFloat4x4(this);
		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(temp);
		DirectX::XMMATRIX result = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, temp));
		DirectX::XMStoreFloat4x4A(this, result);
	}

	inline void operator*=(const matrix &right)
	{
		DirectX::XMMATRIX vLeft = DirectX::XMLoadFloat4x4(this);
		DirectX::XMMATRIX vRight = DirectX::XMLoadFloat4x4(&right);
		DirectX::XMMATRIX vResult = DirectX::XMMatrixMultiply(vLeft, vRight);
		DirectX::XMStoreFloat4x4A(this, vResult);
	}

	inline void operator+=(const matrix &right)
	{
		DirectX::XMMATRIX vLeft = DirectX::XMLoadFloat4x4(this);
		DirectX::XMMATRIX vRight = DirectX::XMLoadFloat4x4(&right);
		DirectX::XMMATRIX vResult = DirectX::XMMatrixMultiply(vLeft, vRight);
		DirectX::XMStoreFloat4x4A(this, vResult);
	}

	inline static matrix setTranslate(float32_t x, float32_t y, float32_t z)
	{
		matrix result;
		DirectX::XMMATRIX vResult = DirectX::XMMatrixTranslation(x, y, z);
		DirectX::XMStoreFloat4x4A(&result, vResult);

		return result;
	}

	inline static matrix setTranslate(const vec3 &translation)
	{
		return setTranslate(translation.x, translation.y, translation.z);
	}

	inline static matrix setScale(float32_t x, float32_t y, float32_t z)
	{
		matrix result;
		DirectX::XMMATRIX vResult = DirectX::XMMatrixScaling(x, y, z);
		DirectX::XMStoreFloat4x4A(&result, vResult);

		return result;
	}

	inline static matrix setScale(float32_t factor)
	{
		return setScale(factor, factor, factor);
	}

	inline static matrix setRotate(float32_t x, float32_t y, float32_t z, float32_t angle)
	{
		matrix result;
		DirectX::XMVECTOR axis = DirectX::XMVectorSet(x, y, z, 0.0f);
		DirectX::XMMATRIX vResult = DirectX::XMMatrixRotationAxis(axis, angle * DirectX::XM_PI / 180.0f);
		DirectX::XMStoreFloat4x4A(&result, vResult);

		return result;
	}

	inline static matrix setRotate(const vec3 &axis, float32_t angle)
	{
		return setRotate(axis.x, axis.y, axis.z, angle);
	}

	inline static matrix setRotateX(float32_t angle)
	{
		matrix result;
		DirectX::XMMATRIX vResult = DirectX::XMMatrixRotationX(angle * DirectX::XM_PI / 180.0f);
		DirectX::XMStoreFloat4x4A(&result, vResult);

		return result;
	}

	inline static matrix setRotateY(float32_t angle)
	{
		matrix result;
		DirectX::XMMATRIX vResult = DirectX::XMMatrixRotationY(angle * DirectX::XM_PI / 180.0f);
		DirectX::XMStoreFloat4x4A(&result, vResult);

		return result;
	}

	inline static matrix setRotateZ(float32_t angle)
	{
		matrix result;
		DirectX::XMMATRIX vResult = DirectX::XMMatrixRotationZ(angle * DirectX::XM_PI / 180.0f);
		DirectX::XMStoreFloat4x4A(&result, vResult);

		return result;
	}

	inline static matrix setRotateRollPitchYaw(float32_t roll, float32_t pitch, float32_t yaw)
	{
		matrix result;
		DirectX::XMMATRIX vResult = DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw);
		DirectX::XMStoreFloat4x4A(&result, vResult);

		return result;
	}

	inline static matrix transpose(const matrix &targetMatrix)
	{
		matrix result;

		DirectX::XMMATRIX temp = DirectX::XMLoadFloat4x4(&targetMatrix);
		DirectX::XMMATRIX vResult = DirectX::XMMatrixTranspose(temp);
		DirectX::XMStoreFloat4x4A(&result, vResult);

		return result;
	}

	inline static matrix inverse(const matrix &targetMatrix)
	{
		matrix result;

		DirectX::XMMATRIX temp = DirectX::XMLoadFloat4x4(&targetMatrix);
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

		DirectX::XMMATRIX temp = DirectX::XMLoadFloat4x4(&copy);
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

	inline static matrix perspectiveOffCenterLH(float32_t left, float32_t right, float32_t bottom, float32_t top, float32_t nearPlane, float32_t farPlane)
	{
		matrix result;

		DirectX::XMMATRIX vResult = DirectX::XMMatrixPerspectiveOffCenterLH(left, right, bottom, top, nearPlane, farPlane);
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

	inline static matrix OrthographicOffCenterLH(float32_t left, float32_t right, float32_t bottom, float32_t top, float32_t nearPlane, float32_t farPlane)
	{
		matrix result;

		DirectX::XMMATRIX vResult = DirectX::XMMatrixOrthographicOffCenterLH(left, right, bottom, top, nearPlane, farPlane);
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
		// inverse() invert matrices using determinants;
		// gaussJordanInverse() use the Gauss-Jordan method.
		// inverse() are significantly faster than gaussJordanInverse(), 
		// but the results may be slightly less accurate.
		if (m[0][3] != 0.0f || m[1][3] != 0.0f || m[2][3] != 0.0f || m[3][3] != 1.0f)
		{
			gaussJordanInverse();
			return;
		}

		matrix s(
			m[1][1] * m[2][2] - m[2][1] * m[1][2],
			m[2][1] * m[0][2] - m[0][1] * m[2][2],
			m[0][1] * m[1][2] - m[1][1] * m[0][2],
			0.0f,

			m[2][0] * m[1][2] - m[1][0] * m[2][2],
			m[0][0] * m[2][2] - m[2][0] * m[0][2],
			m[1][0] * m[0][2] - m[0][0] * m[1][2],
			0.0f,

			m[1][0] * m[2][1] - m[2][0] * m[1][1],
			m[2][0] * m[0][1] - m[0][0] * m[2][1],
			m[0][0] * m[1][1] - m[1][0] * m[0][1],
			0.0f,

			0.0f,
			0.0f,
			0.0f,
			1.0f);

		float32_t r = m[0][0] * s.m[0][0] + m[0][1] * s.m[1][0] + m[0][2] * s.m[2][0];
		if (std::abs(r) >= 1.0f)
		{
			for (uint32_t i = 0u; i < 3u; ++i)
			{
				for (uint32_t j = 0u; j < 3u; ++j)
				{
					s.m[i][j] /= r;
				}
			}
		}
		else
		{
			float32_t mr = std::abs(r) / FLT_MIN;
			for (uint32_t i = 0u; i < 3u; ++i)
			{
				for (uint32_t j = 0u; j < 3u; ++j)
				{
					if (mr > std::abs(s.m[i][j]))
					{
						s.m[i][j] /= r;
					}
					else
					{
						/// singular matrix
						assert(0);
					}
				}
			}
		}

		s.m[3][0] = -m[3][0] * s.m[0][0] - m[3][1] * s.m[1][0] - m[3][2] * s.m[2][0];
		s.m[3][1] = -m[3][0] * s.m[0][1] - m[3][1] * s.m[1][1] - m[3][2] * s.m[2][1];
		s.m[3][2] = -m[3][0] * s.m[0][2] - m[3][1] * s.m[1][2] - m[3][2] * s.m[2][2];

		*this = s;
	}

	inline void inverseTranspose()
	{
		matrix copy(*this);
		copy._41 = 0.0f;
		copy._42 = 0.0f;
		copy._43 = 0.0f;
		copy._44 = 1.0f;

		inverse();
		transpose();
	}

	inline static matrix setTranslate(float32_t x, float32_t y, float32_t z)
	{
		matrix result;
		result._41 = x;
		result._42 = y;
		result._43 = z;
		return result;
	}

	inline static matrix setTranslate(const vec3 &translation)
	{
		return setTranslate(translation.x, translation.y, translation.z);
	}

	inline static matrix setScale(float32_t value)
	{
		return setScale(value, value, value);
	}

	inline static matrix setScale(const vec3 &scalling)
	{
		return setScale(scalling.x, scalling.y, scalling.z);
	}

	inline static matrix setScale(float32_t x, float32_t y, float32_t z)
	{
		matrix result;
		result._11 = x;
		result._22 = y;
		result._33 = z;
		return result;
	}

	inline static matrix setRotate(float32_t x, float32_t y, float32_t z, float32_t angle)
	{
		matrix result;
		vec3 axis(x, y, z);
		axis.normalize();

		float32_t radian = angle * DirectX::XM_PI / 180.0f;
		float32_t cosTheta = ::cosf(radian);
		float32_t sinTheta = ::sinf(radian);

		result._11 = axis.x * axis.x * (1.0f - cosTheta) + cosTheta;
		result._12 = axis.x * axis.y * (1.0f - cosTheta) + axis.z * sinTheta;
		result._13 = axis.x * axis.z * (1.0f - cosTheta) - axis.y * sinTheta;
		result._14 = 0.0f;

		result._21 = axis.x * axis.y * (1.0f - cosTheta) - axis.z * sinTheta;
		result._22 = axis.y * axis.y * (1.0f - cosTheta) + cosTheta;
		result._23 = axis.y * axis.z * (1.0f - cosTheta) + axis.x * sinTheta;
		result._33 = 0.0f;

		result._31 = axis.x * axis.z * (1.0f - cosTheta) + axis.y * sinTheta;
		result._32 = axis.y * axis.z * (1.0f - cosTheta) - axis.x * sinTheta;
		result._33 = axis.z * axis.z * (1.0f - cosTheta) + cosTheta;
		result._34 = 0.0f;

		result._41 = 0.0f;
		result._42 = 0.0f;
		result._43 = 0.0f;
		result._43 = 1.0f;

		return result;
	}

	inline static matrix setRotate(const vec3 &axis, float32_t angle)
	{
		return setRotate(axis.x, axis.y, axis.z, angle);
	}

	inline static matrix setRotateX(float32_t angle)
	{
		matrix result;
		float32_t radian = angle * DirectX::XM_PI / 180.0f;
		float32_t cosTheta = ::cosf(radian);
		float32_t sinTheta = ::sinf(radian);

		result._11 = 1.0f; result._12 = 0.0f;      result._13 = 0.0f;     result._14 = 0.0f;
		result._12 = 0.0f; result._22 = cosTheta;  result._23 = sinTheta; result._24 = 0.0f;
		result._31 = 0.0f; result._32 = -sinTheta; result._33 = cosTheta; result._34 = 0.0f;
		result._41 = 0.0f; result._42 = 0.0f;      result._43 = 0.0f;     result._44 = 1.0f;

		return result;
	}

	inline static matrix setRotateY(float32_t angle)
	{
		matrix result;
		float32_t radian = angle * DirectX::XM_PI / 180.0f;
		float32_t cosTheta = ::cosf(radian);
		float32_t sinTheta = ::sinf(radian);

		result._11 = cosTheta; result._12 = 0.0f; result._13 = -sinTheta; result._14 = 0.0f;
		result._12 = 0.0f;     result._22 = 1.0f; result._23 = 0.0f;      result._24 = 0.0f;
		result._31 = sinTheta; result._32 = 0.0f; result._33 = cosTheta;  result._34 = 0.0f;
		result._41 = 0.0f;     result._42 = 0.0f; result._43 = 0.0f;      result._44 = 1.0f;

		return result;
	}

	inline static matrix setRotateZ(float32_t angle)
	{
		matrix result;
		float32_t radian = angle * DirectX::XM_PI / 180.0f;
		float32_t cosTheta = ::cosf(radian);
		float32_t sinTheta = ::sinf(radian);

		result._11 = cosTheta;  result._12 = sinTheta; result._13 = 0.0f; result._14 = 0.0f;
		result._12 = -sinTheta; result._22 = cosTheta; result._23 = 0.0f; result._24 = 0.0f;
		result._31 = 0.0f;      result._32 = 0.0f;     result._33 = 1.0f; result._34 = 0.0f;
		result._41 = 0.0f;      result._42 = 0.0f;     result._43 = 0.0f; result._44 = 1.0f;

		return result;
	}

	inline static matrix setRotateRollPitchYaw(float32_t, float32_t, float32_t)
	{

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
		float32_t radian = fov * DirectX::XM_PI / 180.0f;
		float32_t cosTheta = ::cosf(radian);
		float32_t sinTheta = ::sinf(radian);
		float32_t height = cosTheta / sinTheta;
		float32_t width = height / aspect;
		float32_t range = farPlane / (farPlane - nearPlane);

		matrix result(
			vec4(width, 0.0f,   0.0f,               0.0f),
			vec4(0.0f,  height, 0.0f,               0.0f),
			vec4(0.0f,  0.0f,   range,              1.0f),
			vec4(0.0f,  0.0f,   -range * nearPlane, 0.0f)
		);
		return result;
	}

	inline static matrix perspectiveOffCenterLH(float32_t left, float32_t right, float32_t bottom, float32_t top, float32_t nearPlane, float32_t farPlane)
	{
		float32_t reciprocalWidth = 1.0f / (right - left);
		float32_t reciprocalHeight = 1.0f / (top - bottom);
		float32_t range = farPlane / (farPlane - nearPlane);

		matrix result(
			vec4(2.0f * nearPlane * reciprocalWidth, 0.0f,                                0.0f,               0.0f),
			vec4(0.0f,                               2.0f * nearPlane * reciprocalHeight, 0.0f,               0.0f),
			vec4(-(left + right) * reciprocalWidth,  -(top + bottom) * reciprocalHeight,  range,              1.0f),
			vec4(0.0f,                               0.0f,                                -range * nearPlane, 0.0f)
		);
		return result;
	}

	inline static matrix orthographicLH(float32_t width, float32_t height, float32_t nearPlane, float32_t farPlane)
	{
		float32_t range = 1.0f / (farPlane - nearPlane);
		matrix result(
			vec4(2.0f / width, 0.0f,          0.0f,               0.0f),
			vec4(0.0f,         2.0f / height, 0.0f,               0.0f),
			vec4(0.0f,         0.0f,          range,              0.0f),
			vec4(0.0f,         0.0f,          -range * nearPlane, 1.0f)
		);
		return result;
	}

	inline static matrix orthographicOffCenterLH(float32_t left, float32_t right, float32_t bottom, float32_t top, float32_t nearPlane, float32_t farPlane)
	{
		/// Model Space -> World Space -> Camera Space -> Projection Space -> Clipping Space -> Homogeneous Screen Space
		/*
		/// For D3D volume box is min(-1,-1,0), max(1,1,1)
		/// For GL volume box is min(-1,-1,-1), max(1,1,1)
		/// Center the Viewing Volume at the Origin
		1. center
		center(x, y, z) center.x = (1 - 0.5) * left + 0.5 * right
		center((left + right) / 2, (up + bottom) / 2, (far + near) / 2)
							 |   1           0          0       0  |
		centerAboutOrigion = |   0           1          0       0  |
							 |   0           0          1       0  |
							 |-center.x  -center.y  -center.z  1.0 |
		2. scale the Viewing Volume to unit volume
		width * factor = unitWidth
		factor = unitWidth / width
		scale(2 / (right - left), 2 / (up - bottom), 1 / (far - near))
							 | scale.x       0          0       0  |
		scaleViewingVolume = |   0         scale.y      0       0  |
							 |   0           0       scale.z    0  |
							 |   0           0          0      1.0 |
		3. switch Coordinate Systems
							  |   1           0          0       0  |
		convertToLeftHanded = |   0           1          0       0  |
							  |   0           0         -1       0  |
							  |   0           0          0      1.0 |

							   |   1           0          0       0  |
		convertToRightHanded = |   0           1          0       0  |
							   |   0           0          1       0  |
							   |   0           0          0      1.0 |

		final = centerAboutOrigion * scaleViewingVolume * convertToRightHanded
		/// https://docs.microsoft.com/en-us/windows/win32/dxtecharts/the-direct3d-transformation-pipeline
		*/
		float32_t reciprocalWidth = 1.0f / (right - left);
		float32_t reciprocalHeight = 1.0f / (top - bottom);
		float32_t range = 1.0f / (farPlane - nearPlane);
		matrix result(
			vec4(2.0f * reciprocalWidth,            0.0f,                               0.0f,               0.0f),
			vec4(0.0f,                              2.0f * reciprocalHeight,            0.0f,               0.0f),
			vec4(0.0f,                              0.0f,                               range,              0.0f),
			vec4(-(left + right) * reciprocalWidth, -(top + bottom) * reciprocalHeight, -range * nearPlane, 1.0f)
		);
		return result;
	}

	inline static matrix lookAtLH(const vec3 &eye, const vec3 &lookAt, const vec3 &up)
	{
		/********************************************************************
		|  1      0      0    0  |      |  xAxis.x   yAxis.x   zAxis.x   0  |
		|  0      1      0    0  |  *   |  xAxis.y	 yAxis.y   zAxis.y   0  |
		|  0      0      1    0  |      |  xAxis.z	 yAxis.z   zAxis.z   0  |
		|-eye.x -eye.y -eye.z 1  |      |     0         0         0      1  |
		********************************************************************/
		vec3 zAxis = normalize(lookAt - eye);
		vec3 xAxis = normalize(cross(up, zAxis));
		vec3 yAxis = cross(zAxis, xAxis);

		matrix result(
			vec4(xAxis.x, yAxis.x, zAxis.x, 0.0f), 
			vec4(xAxis.y, yAxis.y, zAxis.y, 0.0f),
			vec4(xAxis.z, yAxis.z, zAxis.z, 0.0f),
			vec4(-dot(yAxis, eye), -dot(yAxis, eye), -dot(zAxis, eye), 1.0f)
		);

		return result;
	}
#endif
protected:
#if !defined(UsingSSE)
	void gaussJordanInverse();
#endif
private:
};

#if defined(UsingSSE)
inline matrix operator*(const matrix &left, const matrix &right)
{
	matrix result;

	DirectX::XMMATRIX vLeft = DirectX::XMLoadFloat4x4(&left);
	DirectX::XMMATRIX vRight = DirectX::XMLoadFloat4x4(&right);
	DirectX::XMMATRIX vResult = vLeft * vRight;
	DirectX::XMStoreFloat4x4A(&result, vResult);

	return result;
}

inline vec4 operator*(const vec4 &left, const matrix &right)
{

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
