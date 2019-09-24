#pragma once

#include "gear/Math/Matrix.h"
#include "gear/Math/Functions.h"

namespaceStart(gear)

class camera
{
public:
	camera()
		: m_Eye(0.0f, 0.0f, 0.0f, 0.0f)
		, m_LookAt(0.0f, 0.0f, 0.0f, 1.0f)
		, m_Up(0.0f, 1.0f, 0.0f, 1.0f)
	{
	}

	~camera() = default;

	inline void setWorldMatrix(const math::matrix &world)
	{
		m_mWorld = world;
	}

	inline void translate(float32_t x, float32_t y, float32_t z)
	{
		m_mWorld *= math::matrix::setTranslate(x, y, z);
	}

	inline void rotate(float32_t x, float32_t y, float32_t z, float32_t angle)
	{
		m_mWorld *= math::matrix::setRotate(x, y, z, angle);
	}

	inline void setViewParams(
		const math::vec3 &eye, 
		const math::vec3 &lookAt, 
		const math::vec3 &up = math::vec3(0.0f, 1.0f, 0.0f))
	{
		m_Eye = math::vec4(eye);
		m_mView = math::matrix::lookAtLH(eye, lookAt, up);
	}

	inline void setProjParams(float32_t fov, float32_t aspect, float32_t nearPlane, float32_t farPlane)
	{
		m_Fov = fov;
		m_Aspect = aspect;
		m_NearZ = nearPlane;
		m_FarZ = farPlane;
		m_mProjection = math::matrix::perspectiveFovLH(fov, aspect, nearPlane, farPlane);
	}

	inline const math::matrix &worldMatrix() const
	{
		return m_mWorld;
	}

	inline const math::matrix &viewMatrix() const
	{
		return m_mView;
	}

	inline const math::matrix &projMatrix() const
	{
		return m_mProjection;
	}

	inline math::matrix WVPMatrix() const
	{
		return m_mWorld * m_mView * m_mProjection;
	}

	inline const math::vec4 &eye() const
	{
		return m_Eye;
	}

	inline void move(int32_t x, int32_t y)
	{
		/// Spherical coordinate system
		float32_t dx = 0.25f * x * math::g_pi / 180.0f;
		float32_t dy = 0.25f * y * math::g_pi / 180.0f;

		m_Theta += dx;
		m_Phi += dy;

		m_Phi = math::clamp(m_Phi, 0.1f, math::g_pi - 0.1f);
	}

	inline void update()
	{
		float32_t sx = m_Radius * sinf(m_Phi) * cosf(m_Theta);
		float32_t sz = m_Radius * sinf(m_Phi) * sinf(m_Theta);
		float32_t sy = m_Radius * cosf(m_Phi);
		setViewParams(math::vec3(sx, sy, sz), math::vec3(m_LookAt.x, m_LookAt.y, m_LookAt.z));
	}

	inline void setViewRadius(float32_t radius)
	{
		m_Radius = radius;
	}
	inline float32_t viewRadius() const
	{
		return m_Radius;
	}
protected:
private:
	float32_t m_Fov = 0.0f;
	float32_t m_Aspect = 0.0f;
	float32_t m_NearZ = 0.0f;
	float32_t m_FarZ = 0.0f;

	float32_t m_Radius = 15.0f;
	float32_t m_Phi = math::g_pi_div2;
	float32_t m_Theta = math::g_pi * 1.5f;

	math::vec4 m_Eye;
	math::vec4 m_LookAt;
	math::vec4 m_Up;

	math::matrix m_mWorld;
	math::matrix m_mView;
	math::matrix m_mProjection;
};

namespaceEnd(gear)
