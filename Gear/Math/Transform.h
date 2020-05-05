#pragma once

#include "Matrix.h"
#include "Euler.h"
#include "Quaternions.h"

namespaceStart(Gear)
namespaceStart(Math)

class Transform
{
public:
	inline void reset()
	{
		m_Scalling = Vec3(0.0f, 0.0f, 0.0f);
		m_Translation = Vec3(0.0f, 0.0f, 0.0f);
		m_Rotation = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
		m_Matrix.identity();
	}

	inline Matrix matrix()
	{
		build();
		return m_Matrix;
	}

	inline void scale(const Vec3 &scale)
	{
		m_Scalling *= scale;
	}

	inline void scale(const float32_t x, const float32_t y, const float32_t z)
	{
		m_Scalling *= Vec3(x, y, z);
	}

	inline void scale(const float32_t factor)
	{
		m_Scalling *= Vec3(factor, factor, factor);
	}

	inline void translate(const Vec3 &translate)
	{
		m_Translation += translate;
	}

	inline void translate(const float32_t x, const float32_t y, const float32_t z)
	{
		m_Translation += Vec3(x, y, z);
	}

	inline void rotateX(const float32_t angle)
	{
		m_Rotation += Vec4(1.0f, 0.0f, 0.0f, angle);
	}

	inline void rotateY(const float32_t angle)
	{
		m_Rotation += Vec4(0.0f, 1.0f, 0.0f, angle);
	}

	inline void rotateZ(const float32_t angle)
	{
		m_Rotation += Vec4(0.0f, 1.0f, 0.0f, angle);
	}

	inline void rotate(const float32_t x, const float32_t y, const float32_t z, const float32_t angle)
	{
		m_Rotation += Vec4(x, y, z, angle);
	}

	inline void rotate(const Vec3 &axis, const float32_t angle)
	{
		m_Rotation += Vec4(axis, angle);
	}

	inline Matrix transposedMatrix()
	{
		build();
		return Matrix::transpose(m_Matrix);
	}

	inline Matrix inversedMatrix()
	{
		build();
		return Matrix::inverse(m_Matrix);
	}

	inline Matrix inverseTransposedMatrix()
	{
		build();
		return Matrix::inverseTranspose(m_Matrix);
	}
protected:
	void build()
	{
		reset();

		Matrix translation = Matrix::translate(m_Translation);
		Matrix rotation = Matrix::rotate(m_Rotation.x, m_Rotation.y, m_Rotation.z, m_Rotation.w);
		Matrix scalling = Matrix::scale(m_Scalling);

		m_Matrix = translation * rotation * scalling;
	}
private:
	Matrix m_Matrix;
	Vec3 m_Scalling;
	Vec3 m_Translation;
	Vec4 m_Rotation;
};

namespaceEnd(Math)
namespaceEnd(Gear)
