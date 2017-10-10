#pragma once

#include "D3DMath.h"

class Camera
{
public:
	Camera()
		: m_EyePos(0.0f, 0.0f, 0.0f, 1.0f)
		, m_LookAt(0.0f, 0.0f, 0.0f, 1.0f)
	{
		m_World.Identity();

		SetViewParams(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
		SetProjParams(DirectX::XM_PIDIV4, 1.0f, 1.0f, 1000.0f);
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

	virtual void SetViewParams(const Vec3& eyePos, const Vec3& lookAt)
	{
		memcpy(&m_EyePos, &eyePos, sizeof(Vec3));
		memcpy(&m_LookAt, &lookAt, sizeof(Vec3));

		Vec3 upDir(0.0f, 1.0f, 0.0f);
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
protected:
private:
	float m_Fov;
	float m_Aspect;
	float m_NearZ;
	float m_FarZ;

	Vec4 m_EyePos;
	Vec4 m_LookAt;

	Matrix m_World;
	Matrix m_View;
	Matrix m_Proj;
};
