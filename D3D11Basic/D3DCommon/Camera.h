#pragma once

#include "D3DMath.h"

class Camera
{
public:
	Camera()
		: m_EyePos(0.0f, 0.0f, 0.0f, 0.0f)
		, m_LookAt(0.0f, 0.0f, 0.0f, 1.0f)
		, m_UpDir(0.0f, 1.0f, 0.0f, 1.0f)
	{
		m_World.Identity();
	}

	virtual ~Camera() {}

	inline void SetWorldMatrix(const Matrix& worldMatrix)
	{
		memcpy(&m_World, &worldMatrix, sizeof(Matrix));
	}

	inline void Translation(float x, float y, float z)
	{
		Matrix transMatrix = Matrix::Translation(x, y, z);

		m_World *= transMatrix;
	}

	inline void Rotate(float x, float y, float z, float angle)
	{
		Matrix rotateMatrix = Matrix::RotationAxis(x, y, z, angle);

		m_World *= rotateMatrix;
	}

	virtual void SetViewParams(const Vec3& eyePos, const Vec3& lookAt = Vec3(0.0f, 0.0f, 0.0f), const Vec3& upDir = Vec3(0.0f, 1.0f, 0.0f))
	{
		memcpy(&m_EyePos, &eyePos, sizeof(Vec3));
		memcpy(&m_LookAt, &lookAt, sizeof(Vec3));
		memcpy(&m_UpDir, &upDir, sizeof(Vec3));

		m_View = Matrix::LookAtLH(eyePos, lookAt, upDir);
	}

	virtual void SetProjParams(float fov, float aspect, float nearZ, float farZ)
	{
		m_Fov = fov;
		m_Aspect = aspect;
		m_NearZ = nearZ;
		m_FarZ = farZ;

		m_Proj = Matrix::PerspectiveFovLH(fov, aspect, nearZ, farZ);
	}

	inline const Matrix& GetWorldMatrix() const
	{
		return m_World;
	}

	inline const Matrix& GetViewMatrix() const
	{
		return m_View;
	}

	inline const Matrix& GetProjMatrix() const
	{
		return m_Proj;
	}

	inline const Matrix &GetWVPMatrix() const
	{
		return m_World * m_View * m_Proj;
	}

	inline const Vec4& GetEyePos() const
	{
		return m_EyePos;
	}

	void Move(int32_t x, int32_t y)
	{
		/// Spherical coordinate system
		float dx = DirectX::XMConvertToRadians(0.25f * x);
		float dy = DirectX::XMConvertToRadians(0.25f * y);

		m_Theta += dx;
		m_Phi += dy;

		m_Phi = Math::Clamp(m_Phi, 0.1f, DirectX::XM_PI - 0.1f);
	}

	void Update()
	{
		float sx = m_Radius * sinf(m_Phi) * cosf(m_Theta);
		float sz = m_Radius * sinf(m_Phi) * sinf(m_Theta);
		float sy = m_Radius * cosf(m_Phi);

		Vec3 eyePos(sx, sy, sz);
		SetViewParams(eyePos);
	}

	inline void SetViewRadius(float radius)
	{
		m_Radius = radius;
	}
	inline float GetViewRadius() const
	{
		return m_Radius;
	}
protected:
private:
	float m_Fov;
	float m_Aspect;
	float m_NearZ;
	float m_FarZ;

	float m_Radius = 15.0f;
	float m_Phi = DirectX::XM_PIDIV2;
	float m_Theta = DirectX::XM_PI * 1.5f;

	Vec4 m_EyePos;
	Vec4 m_LookAt;
	Vec4 m_UpDir;

	Matrix m_World;
	Matrix m_View;
	Matrix m_Proj;
};
