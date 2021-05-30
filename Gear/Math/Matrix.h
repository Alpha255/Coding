#pragma once

#include "Gear/Math/Vector4.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

class Matrix : public Float4x4
{
public:
	inline Matrix()
	{
		identity();
	}

	inline Matrix(
		float32_t m00, float32_t m01, float32_t m02, float32_t m03,
		float32_t m10, float32_t m11, float32_t m12, float32_t m13,
		float32_t m20, float32_t m21, float32_t m22, float32_t m23,
		float32_t m30, float32_t m31, float32_t m32, float32_t m33)
		: Float4x4(
			m00, m01, m02, m03,
			m10, m11, m12, m13,
			m20, m21, m22, m23,
			m30, m31, m32, m33)
	{
	}

	inline Matrix(const Vec4& row0, const Vec4& row1, const Vec4& row2, const Vec4& row3)
		: Matrix(
			row0.x, row0.y, row0.z, row0.w,
			row1.x, row1.y, row1.z, row1.w,
			row2.x, row2.y, row2.z, row2.w,
			row3.x, row3.y, row3.z, row3.w)
	{
	}

	inline Matrix(const Vec4& row)
		: Matrix(row, row, row, row)
	{
	}

#if defined(USE_SSE)
	inline void identity()
	{
		MATRIX_STORE(this, DirectX::XMMatrixIdentity());
	}

	inline void transpose()
	{
		MATRIX_STORE(this, DirectX::XMMatrixTranspose(MATRIX_LOAD(this)));
	}

	inline void inverse()
	{
		MATRIX_STORE(this, DirectX::XMMatrixInverse(nullptr, MATRIX_LOAD(this)));
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
		MATRIX_STORE(this, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, MATRIX_LOAD(this))));
	}

	inline void operator*=(const Matrix& right)
	{
		MATRIX_STORE(this, DirectX::XMMatrixMultiply(MATRIX_LOAD(this), MATRIX_LOAD(&right)));
	}

	inline void operator+=(const Matrix& right)
	{
		_11 += right._11; _12 += right._12; _13 += right._13; _14 += right._14;
		_21 += right._21; _22 += right._22; _23 += right._23; _24 += right._24;
		_31 += right._31; _32 += right._32; _33 += right._33; _34 += right._34;
		_41 += right._41; _42 += right._42; _43 += right._43; _44 += right._44;
	}

	inline void translation(float32_t x, float32_t y, float32_t z)
	{
		MATRIX_STORE(this, DirectX::XMMatrixMultiply(MATRIX_LOAD(this), DirectX::XMMatrixTranslation(x, y, z)));
	}

	inline void translation(const Vec3& value)
	{
		MATRIX_STORE(this, DirectX::XMMatrixMultiply(MATRIX_LOAD(this), DirectX::XMMatrixTranslation(value.x, value.y, value.z)));
	}

	inline void scalling(float32_t x, float32_t y, float32_t z)
	{
		MATRIX_STORE(this, DirectX::XMMatrixMultiply(MATRIX_LOAD(this), DirectX::XMMatrixScaling(x, y, z)));
	}

	inline void scalling(const Vec3& value)
	{
		MATRIX_STORE(this, DirectX::XMMatrixMultiply(MATRIX_LOAD(this), DirectX::XMMatrixScaling(value.x, value.y, value.z)));
	}

	inline void rotation(float32_t x, float32_t y, float32_t z, float32_t angle)
	{
		MATRIX_STORE(
			this, 
			DirectX::XMMatrixMultiply(
				MATRIX_LOAD(this), 
				DirectX::XMMatrixRotationAxis(
					DirectX::XMVectorSet(x, y, z, 0.0f), 
					DirectX::XMConvertToRadians(angle))));
	}

	inline void rotation(const Vec3& axis, float32_t angle)
	{
		MATRIX_STORE(
			this,
			DirectX::XMMatrixMultiply(
				MATRIX_LOAD(this),
				DirectX::XMMatrixRotationAxis(
					DirectX::XMVectorSet(axis.x, axis.y, axis.z, 0.0f),
					DirectX::XMConvertToRadians(angle))));
	}

	inline void rotationX(float32_t angle)
	{
		MATRIX_STORE(
			this,
			DirectX::XMMatrixMultiply(
				MATRIX_LOAD(this),
				DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(angle))));
	}

	inline void rotationY(float32_t angle)
	{
		MATRIX_STORE(
			this,
			DirectX::XMMatrixMultiply(
				MATRIX_LOAD(this),
				DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(angle))));
	}

	inline void rotationZ(float32_t angle)
	{
		MATRIX_STORE(
			this,
			DirectX::XMMatrixMultiply(
				MATRIX_LOAD(this),
				DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(angle))));
	}

	inline void rotationRollPitchYaw(float32_t pitch, float32_t yaw, float32_t roll)
	{
		MATRIX_STORE(
			this, 
			DirectX::XMMatrixMultiply(
				MATRIX_LOAD(this),
				DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll)));
	}

	inline static Matrix translate(float32_t x, float32_t y, float32_t z)
	{
		Matrix Result;
		MATRIX_STORE(&Result, DirectX::XMMatrixTranslation(x, y, z));
		return Result;
	}

	inline static Matrix translate(const Vec3& translation)
	{
		return translate(translation.x, translation.y, translation.z);
	}

	inline static Matrix scale(const Vec3& scaling)
	{
		return scale(scaling.x, scaling.y, scaling.z);
	}

	inline static Matrix scale(float32_t x, float32_t y, float32_t z)
	{
		Matrix Result;
		MATRIX_STORE(&Result, DirectX::XMMatrixScaling(x, y, z));
		return Result;
	}

	inline static Matrix scale(float32_t factor)
	{
		return scale(factor, factor, factor);
	}

	inline static Matrix rotate(float32_t x, float32_t y, float32_t z, float32_t angle)
	{
		Matrix Result;
		MATRIX_STORE(&Result, DirectX::XMMatrixRotationAxis(DirectX::XMVectorSet(x, y, z, 0.0f), DirectX::XMConvertToRadians(angle)));
		return Result;
	}

	inline static Matrix rotate(const Vec3& axis, float32_t angle)
	{
		return rotate(axis.x, axis.y, axis.z, angle);
	}

	inline static Matrix rotateX(float32_t angle)
	{
		Matrix Result;
		MATRIX_STORE(&Result, DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(angle)));
		return Result;
	}

	inline static Matrix rotateY(float32_t angle)
	{
		Matrix Result;
		MATRIX_STORE(&Result, DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(angle)));
		return Result;
	}

	inline static Matrix rotateZ(float32_t angle)
	{
		Matrix Result;
		MATRIX_STORE(&Result, DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(angle)));
		return Result;
	}

	inline static Matrix rotateRollPitchYaw(float32_t pitch, float32_t yaw, float32_t roll)
	{
		Matrix Result;
		MATRIX_STORE(&Result, DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll));
		return Result;
	}

	inline static Matrix transpose(const Matrix& other)
	{
		Matrix Result;
		MATRIX_STORE(&Result, DirectX::XMMatrixTranspose(MATRIX_LOAD(&other)));
		return Result;
	}

	inline static Matrix inverse(const Matrix& other)
	{
		Matrix Result;
		MATRIX_STORE(&Result, DirectX::XMMatrixInverse(nullptr, MATRIX_LOAD(&other)));
		return Result;
	}

	inline static Matrix inverseTranspose(const Matrix& other)
	{
		/// Inverse-transpose is just applied to normals.  So zero out 
		/// translation row so that it doesn't get into our inverse-transpose
		/// calculation--we don't want the inverse-transpose of the translation.
		Matrix Result = other;
		Result._41 = 0.0f;
		Result._42 = 0.0f;
		Result._43 = 0.0f;
		Result._44 = 1.0f;
		MATRIX_STORE(&Result, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, MATRIX_LOAD(&Result))));
		return Result;
	}

	inline static Matrix perspectiveFovLH(float32_t fov, float32_t aspect, float32_t nearPlane, float32_t farPlane)
	{
		Matrix Result;
		MATRIX_STORE(&Result, DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov), aspect, nearPlane, farPlane));
		return Result;
	}

	inline static Matrix perspectiveFovRH(float32_t fov, float32_t aspect, float32_t nearPlane, float32_t farPlane)
	{
		Matrix Result;
		MATRIX_STORE(&Result, DirectX::XMMatrixPerspectiveFovRH(DirectX::XMConvertToRadians(fov), aspect, nearPlane, farPlane));
		return Result;
	}

	inline static Matrix perspectiveOffCenterLH(float32_t left, float32_t right, float32_t bottom, float32_t top, float32_t nearPlane, float32_t farPlane)
	{
		Matrix Result;
		MATRIX_STORE(&Result, DirectX::XMMatrixPerspectiveOffCenterLH(left, right, bottom, top, nearPlane, farPlane));
		return Result;
	}

	inline static Matrix orthographicLH(float32_t width, float32_t height, float32_t nearPlane, float32_t farPlane)
	{
		Matrix Result;
		MATRIX_STORE(&Result, DirectX::XMMatrixOrthographicLH(width, height, nearPlane, farPlane));
		return Result;
	}

	inline static Matrix orthographicOffCenterLH(float32_t left, float32_t right, float32_t bottom, float32_t top, float32_t nearPlane, float32_t farPlane)
	{
		Matrix Result;
		MATRIX_STORE(&Result, DirectX::XMMatrixOrthographicOffCenterLH(left, right, bottom, top, nearPlane, farPlane));
		return Result;
	}

	inline static Matrix lookAtLH(const Vec3& eye, const Vec3& lookAt, const Vec3& up)
	{
		Matrix Result;
		MATRIX_STORE(&Result, DirectX::XMMatrixLookAtLH(
			DirectX::XMVectorSet(eye.x, eye.y, eye.z, 1.0f), 
			DirectX::XMVectorSet(lookAt.x, lookAt.y, lookAt.z, 0.0f), 
			DirectX::XMVectorSet(up.x, up.y, up.z, 0.0f)));
		return Result;
	}

	inline static Matrix lookAtRH(const Vec3& eye, const Vec3& lookAt, const Vec3& up)
	{
		Matrix Result;
		MATRIX_STORE(&Result, DirectX::XMMatrixLookAtRH(
			DirectX::XMVectorSet(eye.x, eye.y, eye.z, 1.0f), 
			DirectX::XMVectorSet(lookAt.x, lookAt.y, lookAt.z, 0.0f), 
			DirectX::XMVectorSet(up.x, up.y, up.z, 0.0f)));
		return Result;
	}

	inline static Matrix reflect(const Vec4& plane)
	{
		Matrix Result;
		MATRIX_STORE(&Result, DirectX::XMMatrixReflect(VECTOR_LOAD(4, &plane)));
		return Result;
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
		Matrix temp(
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

		Matrix s(
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
						/// singular Matrix
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
		Matrix copy(*this);
		copy._41 = 0.0f;
		copy._42 = 0.0f;
		copy._43 = 0.0f;
		copy._44 = 1.0f;

		inverse();
		transpose();
	}

	inline static Matrix translate(float32_t x, float32_t y, float32_t z)
	{
		Matrix result;
		result._41 = x;
		result._42 = y;
		result._43 = z;
		return result;
	}

	inline static Matrix translate(const Vec3 &translation)
	{
		return translate(translation.x, translation.y, translation.z);
	}

	inline static Matrix scale(float32_t value)
	{
		return scale(value, value, value);
	}

	inline static Matrix scale(const Vec3 &scalling)
	{
		return scale(scalling.x, scalling.y, scalling.z);
	}

	inline static Matrix scale(float32_t x, float32_t y, float32_t z)
	{
		Matrix result;
		result._11 = x;
		result._22 = y;
		result._33 = z;
		return result;
	}

	inline static Matrix rotate(float32_t x, float32_t y, float32_t z, float32_t angle)
	{
		Matrix result;
		Vec3 axis(x, y, z);
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

	inline static Matrix rotate(const Vec3 &axis, float32_t angle)
	{
		return setRotate(axis.x, axis.y, axis.z, angle);
	}

	inline static Matrix rotateX(float32_t angle)
	{
		Matrix result;
		float32_t radian = angle * DirectX::XM_PI / 180.0f;
		float32_t cosTheta = ::cosf(radian);
		float32_t sinTheta = ::sinf(radian);

		result._11 = 1.0f; result._12 = 0.0f;      result._13 = 0.0f;     result._14 = 0.0f;
		result._12 = 0.0f; result._22 = cosTheta;  result._23 = sinTheta; result._24 = 0.0f;
		result._31 = 0.0f; result._32 = -sinTheta; result._33 = cosTheta; result._34 = 0.0f;
		result._41 = 0.0f; result._42 = 0.0f;      result._43 = 0.0f;     result._44 = 1.0f;

		return result;
	}

	inline static Matrix rotateY(float32_t angle)
	{
		Matrix result;
		float32_t radian = angle * DirectX::XM_PI / 180.0f;
		float32_t cosTheta = ::cosf(radian);
		float32_t sinTheta = ::sinf(radian);

		result._11 = cosTheta; result._12 = 0.0f; result._13 = -sinTheta; result._14 = 0.0f;
		result._12 = 0.0f;     result._22 = 1.0f; result._23 = 0.0f;      result._24 = 0.0f;
		result._31 = sinTheta; result._32 = 0.0f; result._33 = cosTheta;  result._34 = 0.0f;
		result._41 = 0.0f;     result._42 = 0.0f; result._43 = 0.0f;      result._44 = 1.0f;

		return result;
	}

	inline static Matrix rotateZ(float32_t angle)
	{
		Matrix result;
		float32_t radian = angle * DirectX::XM_PI / 180.0f;
		float32_t cosTheta = ::cosf(radian);
		float32_t sinTheta = ::sinf(radian);

		result._11 = cosTheta;  result._12 = sinTheta; result._13 = 0.0f; result._14 = 0.0f;
		result._12 = -sinTheta; result._22 = cosTheta; result._23 = 0.0f; result._24 = 0.0f;
		result._31 = 0.0f;      result._32 = 0.0f;     result._33 = 1.0f; result._34 = 0.0f;
		result._41 = 0.0f;      result._42 = 0.0f;     result._43 = 0.0f; result._44 = 1.0f;

		return result;
	}

	inline static Matrix rotateRollPitchYaw(float32_t roll, float32_t pitch, float32_t yaw)
	{
		Matrix result;
		DirectX::XMMATRIX vResult = DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw);
		DirectX::XMStoreFloat4x4A(&result, vResult);

		return result;
	}

	inline void operator*=(const Matrix &right)
	{
		Vec4 row(_11, _12, _13, _14);
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

	inline void operator+=(const Matrix &right)
	{
		_11 += right._11; _12 += right._12; _13 += right._13; _14 += right._14;
		_21 += right._21; _22 += right._22; _23 += right._23; _24 += right._24;
		_31 += right._31; _32 += right._32; _33 += right._33; _34 += right._34;
		_41 += right._41; _42 += right._42; _43 += right._43; _44 += right._44;
	}

	inline void operator-=(const Matrix &right)
	{
		_11 -= right._11; _12 -= right._12; _13 -= right._13; _14 -= right._14;
		_21 -= right._21; _22 -= right._22; _23 -= right._23; _24 -= right._24;
		_31 -= right._31; _32 -= right._32; _33 -= right._33; _34 -= right._34;
		_41 -= right._41; _42 -= right._42; _43 -= right._43; _44 -= right._44;
	}

	inline static Matrix transpose(const Matrix &targetMatrix)
	{
		Matrix result(targetMatrix);
		result.transpose();

		return result;
	}

	inline static Matrix inverse(const Matrix &targetMatrix)
	{
		Matrix result(targetMatrix);
		result.inverse();

		return result;
	}

	inline static Matrix inverseTranspose(const Matrix &targetMatrix)
	{
		Matrix result(targetMatrix);
		result.inverseTranspose();

		return result;
	}

	inline static Matrix perspectiveFovLH(float32_t fov, float32_t aspect, float32_t nearPlane, float32_t farPlane)
	{
		float32_t radian = fov * 0.5f;
		float32_t cosTheta = ::cosf(radian);
		float32_t sinTheta = ::sinf(radian);
		float32_t height = cosTheta / sinTheta;
		float32_t width = height / aspect;
		float32_t range = farPlane / (farPlane - nearPlane);

		Matrix result(
			Vec4(width, 0.0f,   0.0f,               0.0f),
			Vec4(0.0f,  height, 0.0f,               0.0f),
			Vec4(0.0f,  0.0f,   range,              1.0f),
			Vec4(0.0f,  0.0f,   -range * nearPlane, 0.0f)
		);
		return result;
	}

	inline static Matrix perspectiveOffCenterLH(float32_t left, float32_t right, float32_t bottom, float32_t top, float32_t nearPlane, float32_t farPlane)
	{
		float32_t reciprocalWidth = 1.0f / (right - left);
		float32_t reciprocalHeight = 1.0f / (top - bottom);
		float32_t range = farPlane / (farPlane - nearPlane);

		Matrix result(
			Vec4(2.0f * nearPlane * reciprocalWidth, 0.0f,                                0.0f,               0.0f),
			Vec4(0.0f,                               2.0f * nearPlane * reciprocalHeight, 0.0f,               0.0f),
			Vec4(-(left + right) * reciprocalWidth,  -(top + bottom) * reciprocalHeight,  range,              1.0f),
			Vec4(0.0f,                               0.0f,                                -range * nearPlane, 0.0f)
		);
		return result;
	}

	inline static Matrix orthographicLH(float32_t width, float32_t height, float32_t nearPlane, float32_t farPlane)
	{
		float32_t range = 1.0f / (farPlane - nearPlane);
		Matrix result(
			Vec4(2.0f / width, 0.0f,          0.0f,               0.0f),
			Vec4(0.0f,         2.0f / height, 0.0f,               0.0f),
			Vec4(0.0f,         0.0f,          range,              0.0f),
			Vec4(0.0f,         0.0f,          -range * nearPlane, 1.0f)
		);
		return result;
	}

	inline static Matrix orthographicOffCenterLH(float32_t left, float32_t right, float32_t bottom, float32_t top, float32_t nearPlane, float32_t farPlane)
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
		Matrix result(
			Vec4(2.0f * reciprocalWidth,            0.0f,                               0.0f,               0.0f),
			Vec4(0.0f,                              2.0f * reciprocalHeight,            0.0f,               0.0f),
			Vec4(0.0f,                              0.0f,                               range,              0.0f),
			Vec4(-(left + right) * reciprocalWidth, -(top + bottom) * reciprocalHeight, -range * nearPlane, 1.0f)
		);
		return result;
	}

	inline static Matrix lookAtLH(const Vec3 &eye, const Vec3 &lookAt, const Vec3 &up)
	{
		/********************************************************************
		|  1      0      0    0  |      |  xAxis.x   yAxis.x   zAxis.x   0  |
		|  0      1      0    0  |  *   |  xAxis.y	 yAxis.y   zAxis.y   0  |
		|  0      0      1    0  |      |  xAxis.z	 yAxis.z   zAxis.z   0  |
		|-eye.x -eye.y -eye.z 1  |      |     0         0         0      1  |
		********************************************************************/
		Vec3 zAxis = normalize(lookAt - eye);
		Vec3 xAxis = normalize(cross(up, zAxis));
		Vec3 yAxis = cross(zAxis, xAxis);

		Matrix result(
			Vec4(xAxis.x, yAxis.x, zAxis.x, 0.0f), 
			Vec4(xAxis.y, yAxis.y, zAxis.y, 0.0f),
			Vec4(xAxis.z, yAxis.z, zAxis.z, 0.0f),
			Vec4(-dot(yAxis, eye), -dot(yAxis, eye), -dot(zAxis, eye), 1.0f)
		);

		return result;
	}
#endif
protected:
#if !defined(USE_SSE)
	void gaussJordanInverse();
#endif
private:
};

#if defined(USE_SSE)
inline Matrix operator*(const Matrix& left, const Matrix& right)
{
	Matrix Result;
	MATRIX_STORE(&Result, DirectX::XMMatrixMultiply(MATRIX_LOAD(&left), MATRIX_LOAD(&right)));
	return Result;
}

inline Vec4 operator*(const Vec4& left, const Matrix& right)
{
	Vec4 Result;
	VECTOR_STORE(4, &Result, DirectX::XMVector4Transform(VECTOR_LOAD(4, &left), MATRIX_LOAD(&right)));
	return Result;
}

inline Vec3 operator*(const Vec3& left, const Matrix& right)
{
	Vec3 Result;
	VECTOR_STORE(3, &Result, DirectX::XMVector3Transform(VECTOR_LOAD(3, &left), MATRIX_LOAD(&right)));
	return Result;
}

inline Vec2 operator*(const Vec2& left, const Matrix& right)
{
	Vec2 Result;
	VECTOR_STORE(2, &Result, DirectX::XMVector2Transform(VECTOR_LOAD(2, &left), MATRIX_LOAD(&right)));
	return Result;
}
#else
inline Matrix operator*(const Matrix &left, const Matrix &right)
{
	Matrix result(left);
	result *= right;

	return result;
}

inline Vec4 operator*(const Vec4 &left, const Matrix &right)
{
	Vec4 result;
	result.x = left.x * right._11 + left.y * right._21 + left.z * right._31 + left.w * right._41;
	result.y = left.x * right._12 + left.y * right._22 + left.z * right._32 + left.w * right._42;
	result.z = left.x * right._13 + left.y * right._23 + left.z * right._33 + left.w * right._43;
	result.w = left.x * right._14 + left.y * right._24 + left.z * right._34 + left.w * right._44;

	return result;
}
#endif

NAMESPACE_END(Math)
NAMESPACE_END(Gear)
