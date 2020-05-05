#pragma once

#include "Gear/Math/Matrix.h"

namespaceStart(Gear)

class Camera
{
public:
	Camera()
		: m_Eye(0.0f, 0.0f, 0.0f, 0.0f)
		, m_LookAt(0.0f, 0.0f, 0.0f, 1.0f)
		, m_Up(0.0f, 1.0f, 0.0f, 1.0f)
	{
	}

	~Camera() = default;

	inline void setWorldMatrix(const Math::Matrix &world)
	{
		m_mWorld = world;
	}

	inline void translate(float32_t x, float32_t y, float32_t z)
	{
		m_mWorld *= Math::Matrix::translate(x, y, z);
	}

	inline void rotate(float32_t x, float32_t y, float32_t z, float32_t angle)
	{
		m_mWorld *= Math::Matrix::rotate(x, y, z, angle);
	}

	inline void setViewParams(
		const Math::Vec3 &eye, 
		const Math::Vec3 &lookAt, 
		const Math::Vec3 &up = Math::Vec3(0.0f, 1.0f, 0.0f))
	{
		m_Eye = Math::Vec4(eye);
		m_mView = Math::Matrix::lookAtLH(eye, lookAt, up);
	}

	inline void setProjParams(float32_t fov, float32_t aspect, float32_t nearPlane, float32_t farPlane)
	{
		m_Fov = fov;
		m_Aspect = aspect;
		m_NearZ = nearPlane;
		m_FarZ = farPlane;
		m_mProjection = Math::Matrix::perspectiveFovLH(fov, aspect, nearPlane, farPlane);
	}

	inline const Math::Matrix &worldMatrix() const
	{
		return m_mWorld;
	}

	inline const Math::Matrix &viewMatrix() const
	{
		return m_mView;
	}

	inline const Math::Matrix &projMatrix() const
	{
		return m_mProjection;
	}

	inline Math::Matrix WVPMatrix() const
	{
		return m_mWorld * m_mView * m_mProjection;
	}

	inline const Math::Vec4 &eye() const
	{
		return m_Eye;
	}

	inline void move(int32_t x, int32_t y)
	{
		/// Spherical coordinate system
		float32_t dx = 0.25f * x * Math::PI / 180.0f;
		float32_t dy = 0.25f * y * Math::PI / 180.0f;

		m_Theta += dx;
		m_Phi += dy;

		m_Phi = Math::clamp(m_Phi, 0.1f, Math::PI - 0.1f);
	}

	inline void update()
	{
		float32_t sx = m_Radius * sinf(m_Phi) * cosf(m_Theta);
		float32_t sz = m_Radius * sinf(m_Phi) * sinf(m_Theta);
		float32_t sy = m_Radius * cosf(m_Phi);
		setViewParams(Math::Vec3(sx, sy, sz), Math::Vec3(m_LookAt.x, m_LookAt.y, m_LookAt.z));
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
	float32_t m_Phi = Math::PI_Div2;
	float32_t m_Theta = Math::PI * 1.5f;

	Math::Vec4 m_Eye;
	Math::Vec4 m_LookAt;
	Math::Vec4 m_Up;

	Math::Matrix m_mWorld;
	Math::Matrix m_mView;
	Math::Matrix m_mProjection;
};

namespaceEnd(Gear)
