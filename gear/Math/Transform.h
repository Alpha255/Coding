#pragma once

#include "Functions.h"
#include "Matrix.h"
#include "Euler.h"
#include "Quater.h"

namespaceStart(gear)
namespaceStart(math)

class transform
{
public:
	inline void reset()
	{
		m_Scalling = vec3(0.0f, 0.0f, 0.0f);
		m_Translation = vec3(0.0f, 0.0f, 0.0f);
		m_Matrix.identity();
	}

	inline matrix get()
	{
		build();
		return m_Matrix;
	}

	inline void setScale(const vec3 &scale)
	{
		m_Scalling *= scale;
	}

	inline void setScale(const float32_t x, const float32_t y, const float32_t z)
	{
		m_Scalling *= vec3(x, y, z);
	}

	inline void setScale(const float32_t factor)
	{
		m_Scalling *= vec3(factor, factor, factor);
	}

	inline void setTranslate(const vec3 &translate)
	{
		m_Translation += translate;
	}

	inline void setTranslate(const float32_t x, const float32_t y, const float32_t z)
	{
		m_Translation += vec3(x, y, z);
	}

	inline void setRotateX(const float32_t angle)
	{
		m_Rotation += vec4(1.0f, 0.0f, 0.0f, angle);
	}

	inline void setRotateY(const float32_t angle)
	{
		m_Rotation += vec4(0.0f, 1.0f, 0.0f, angle);
	}

	inline void setRotateZ(const float32_t angle)
	{
		m_Rotation += vec4(0.0f, 1.0f, 0.0f, angle);
	}

	inline void setRotate(const float32_t x, const float32_t y, const float32_t z, const float32_t angle)
	{
		m_Rotation += vec4(x, y, z, angle);
	}

	inline void setRotate(const vec3 &axis, const float32_t angle)
	{
		m_Rotation += vec4(axis, angle);
	}

	inline matrix getTransposed()
	{
		build();
		return matrix::transpose(m_Matrix);
	}

	inline matrix getInversed()
	{
		build();
		return matrix::inverse(m_Matrix);
	}

	inline matrix getInverseTransposed()
	{
		build();
		return matrix::inverseTranspose(m_Matrix);
	}
protected:
	void build()
	{
		reset();

		m_Matrix.translate(m_Translation);
		m_Matrix.rotate(m_Rotation.x, m_Rotation.y, m_Rotation.z, m_Rotation.z);
		m_Matrix.scale(m_Scalling);
	}
private:
	matrix m_Matrix;
	vec3 m_Scalling;
	vec3 m_Translation;
	vec4 m_Rotation;
};

namespaceEnd(math)
namespaceEnd(gear)
