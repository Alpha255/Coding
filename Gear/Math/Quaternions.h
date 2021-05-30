#pragma once

#include "Gear/Math/Matrix.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

class Quaternions : public Float4
{
public:
	/// The DirectXMath quaternion functions use an XMVECTOR 4-vector to represent quaternions, 
	/// where the X, Y, and Z components are the vector part and the W component is the scalar part.
	Quaternions(float32_t i, float32_t j, float32_t k, float32_t s)
		: Float4(i, j, k, s)
	{
	}

	inline void identity()
	{
		VECTOR_STORE(4, this, DirectX::XMQuaternionIdentity());
	}

	inline void inverse()
	{
		VECTOR_STORE(4, this, DirectX::XMQuaternionInverse(VECTOR_LOAD(4, this)));
	}

	inline void normalize()
	{
		VECTOR_STORE(4, this, DirectX::XMQuaternionNormalize(VECTOR_LOAD(4, this)));
	}

	inline void normalizeEst()
	{
		VECTOR_STORE(4, this, DirectX::XMQuaternionNormalizeEst(VECTOR_LOAD(4, this)));
	}

	inline Quaternions conjugate() const
	{
		Quaternions result(0.0f, 0.0f, 0.0f, 0.0f);
		VECTOR_STORE(4, &result, DirectX::XMQuaternionConjugate(VECTOR_LOAD(4, this)));
		return result;
	}

	inline Vec4 logN() const
	{
	}

	inline Vec4 exp() const
	{
	}

	inline Quaternions operator+(const Quaternions& other) const
	{
		return Quaternions(x + other.x, y + other.y, z + other.z, w + other.w);
	}

	inline Quaternions& operator+=(const Quaternions& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
		return *this;
	}

	inline Quaternions operator-(const Quaternions& other) const
	{
		return Quaternions(x - other.x, y - other.y, z - other.z, w - other.w);
	}

	inline Quaternions& operator-=(const Quaternions& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;
		return *this;
	}

	inline Quaternions operator*(const Quaternions& other) const
	{
		Quaternions result(0.0f, 0.0f, 0.0f, 0.0f);
		VECTOR_STORE(4, &result, DirectX::XMQuaternionMultiply(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &other)));
		return result;
	}

	inline Quaternions& operator*=(const Quaternions& other)
	{
		VECTOR_STORE(4, this, DirectX::XMQuaternionMultiply(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &other)));
		return *this;
	}

	inline bool8_t isNaN() const
	{
		return DirectX::XMQuaternionIsNaN(VECTOR_LOAD(4, this));
	}

	inline bool8_t isInfinite() const
	{
		return DirectX::XMQuaternionIsInfinite(VECTOR_LOAD(4, this));
	}

	inline bool8_t isIdentity() const
	{
		return DirectX::XMQuaternionIsIdentity(VECTOR_LOAD(4, this));
	}

	inline bool8_t operator==(const Quaternions& other) const
	{
		return DirectX::XMQuaternionEqual(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &other));
	}

	inline bool8_t operator!=(const Quaternions& other) const
	{
		return DirectX::XMQuaternionNotEqual(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &other));
	}

	inline float32_t dot(const Quaternions& other) const
	{
		return DirectX::XMVectorGetX(DirectX::XMQuaternionDot(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &other)));
	}

	inline float32_t length() const
	{
		return DirectX::XMVectorGetX(DirectX::XMQuaternionLength(VECTOR_LOAD(4, this)));
	}

	inline float32_t lengthSq() const
	{
		return DirectX::XMVectorGetX(DirectX::XMQuaternionLengthSq(VECTOR_LOAD(4, this)));
	}

	inline float32_t reciprocalLength() const
	{
		return DirectX::XMVectorGetX(DirectX::XMQuaternionReciprocalLength(VECTOR_LOAD(4, this)));
	}

	inline static Quaternions rotateRollPitchYaw(float32_t pitch, float32_t yaw, float32_t roll)
	{
		Quaternions result(0.0f, 0.0f, 0.0f, 0.0f);
		VECTOR_STORE(4, &result, DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));
		return result;
	}

	inline static Quaternions rotateRollPitchYaw(const Vec3& pitchYawRoll)
	{
		Quaternions result(0.0f, 0.0f, 0.0f, 0.0f);
		VECTOR_STORE(4, &result, DirectX::XMQuaternionRotationRollPitchYaw(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z));
		return result;
	}

	inline static Quaternions rotate(const Vec3& axis, float32_t angle)
	{
		Quaternions result(0.0f, 0.0f, 0.0f, 0.0f);
		VECTOR_STORE(4, &result, DirectX::XMQuaternionRotationAxis(VECTOR_LOAD(3, &axis), angle));
		return result;
	}

	inline static Quaternions rotate(const Matrix& matrix)
	{
		Quaternions result(0.0f, 0.0f, 0.0f, 0.0f);
		VECTOR_STORE(4, &result, DirectX::XMQuaternionRotationMatrix(MATRIX_LOAD(&matrix)));
		return result;
	}

	inline std::pair<Vec3, float32_t> toAxisAngle()
	{
		DirectX::XMVECTOR vAxis;
		Vec3 axis;
		float32_t angle = 0.0f;
		DirectX::XMQuaternionToAxisAngle(&vAxis, &angle, VECTOR_LOAD(4, this));
		VECTOR_STORE(3, &axis, vAxis);
		return std::make_pair(axis, angle);
	}
protected:
private:
};

inline Quaternions slerp(const Quaternions& q0, const Quaternions& q1, float32_t factor)
{
	Quaternions result(0.0f, 0.0f, 0.0f, 0.0f);
	VECTOR_STORE(4, &result, DirectX::XMQuaternionSlerp(VECTOR_LOAD(4, &q0), VECTOR_LOAD(4, &q1), factor));
	return result;
}

inline Quaternions slerpV(const Quaternions& q0, const Quaternions& q1, const Vec4& factor)
{
	Quaternions result(0.0f, 0.0f, 0.0f, 0.0f);
	VECTOR_STORE(4, &result, DirectX::XMQuaternionSlerpV(VECTOR_LOAD(4, &q0), VECTOR_LOAD(4, &q1), VECTOR_LOAD(4, &factor)));
	return result;
}

NAMESPACE_END(Math)
NAMESPACE_END(Gear)
