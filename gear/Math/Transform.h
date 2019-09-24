#pragma once

#include "Functions.h"
#include "Matrix.h"
#include "Euler.h"
#include "Quaternions.h"

namespaceStart(gear)
namespaceStart(math)

class transform
{
public:
	inline void reset()
	{
		m_Scalling = vec3(0.0f, 0.0f, 0.0f);
		m_Translation = vec3(0.0f, 0.0f, 0.0f);
		m_Rotation = vec4(0.0f, 0.0f, 0.0f, 0.0f);
		m_mMatrix.identity();
	}

	inline matrix get()
	{
		build();
		return m_mMatrix;
	}

	inline void scale(const vec3 &scale)
	{
		m_Scalling *= scale;
	}

	inline void scale(const float32_t x, const float32_t y, const float32_t z)
	{
		m_Scalling *= vec3(x, y, z);
	}

	inline void scale(const float32_t factor)
	{
		m_Scalling *= vec3(factor, factor, factor);
	}

	inline void translate(const vec3 &translate)
	{
		m_Translation += translate;
	}

	inline void translate(const float32_t x, const float32_t y, const float32_t z)
	{
		m_Translation += vec3(x, y, z);
	}

	inline void rotateX(const float32_t angle)
	{
		m_Rotation += vec4(1.0f, 0.0f, 0.0f, angle);
	}

	inline void rotateY(const float32_t angle)
	{
		m_Rotation += vec4(0.0f, 1.0f, 0.0f, angle);
	}

	inline void rotateZ(const float32_t angle)
	{
		m_Rotation += vec4(0.0f, 1.0f, 0.0f, angle);
	}

	inline void rotate(const float32_t x, const float32_t y, const float32_t z, const float32_t angle)
	{
		m_Rotation += vec4(x, y, z, angle);
	}

	inline void rotate(const vec3 &axis, const float32_t angle)
	{
		m_Rotation += vec4(axis, angle);
	}

	inline matrix getTransposed()
	{
		build();
		return matrix::transpose(m_mMatrix);
	}

	inline matrix getInversed()
	{
		build();
		return matrix::inverse(m_mMatrix);
	}

	inline matrix getInverseTransposed()
	{
		build();
		return matrix::inverseTranspose(m_mMatrix);
	}
protected:
	void build()
	{
		reset();

		matrix translation = matrix::setTranslate(m_Translation);
		matrix rotation = matrix::setRotate(m_Rotation.x, m_Rotation.y, m_Rotation.z, m_Rotation.w);
		matrix scalling = matrix::setScale(m_Scalling);

		m_mMatrix = translation * rotation * scalling;
	}
private:
	matrix m_mMatrix;
	vec3 m_Scalling;
	vec3 m_Translation;
	vec4 m_Rotation;
};

namespaceEnd(math)
namespaceEnd(gear)
