#pragma once

#include "D3DMath.h"

class Camera
{
public:
	Camera()
		: m_EyePos(0.0f, 0.0f, 0.0f, 1.0f)
		, m_LookAt(0.0f, 0.0f, 0.0f, 1.0f)
	{
		m_World = DirectX::XMMatrixIdentity();

		SetViewParams(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f));
		SetProjParams(DirectX::XM_PIDIV4, 1.0f, 1.0f, 1000.0f);
	}

	virtual ~Camera() {}

	inline void SetWorldMatrix(const DirectX::XMMATRIX& worldMatrix)
	{
		memcpy(&m_World, &worldMatrix, sizeof(DirectX::XMMATRIX));
	}

	inline void Translation(float x, float y, float z)
	{
		DirectX::XMMATRIX transMatrix = DirectX::XMMatrixTranslation(x, y, z);

		m_World *= transMatrix;
	}

	inline void Rotate(float x, float y, float z, float angle)
	{
		DirectX::XMVECTOR axis = DirectX::XMVectorSet(x, y, z, 0.0f);

		DirectX::XMMATRIX rotateMatrix = DirectX::XMMatrixRotationAxis(axis, angle);

		m_World *= rotateMatrix;
	}

	virtual void SetViewParams(const DirectX::XMFLOAT3& eyePos, const DirectX::XMFLOAT3& lookAt)
	{
		memcpy(&m_EyePos, &eyePos, sizeof(DirectX::XMFLOAT3));
		memcpy(&m_LookAt, &lookAt, sizeof(DirectX::XMFLOAT3));

		DirectX::XMVECTOR eye = DirectX::XMVectorSet(eyePos.x, eyePos.y, eyePos.z, 1.0f);
		DirectX::XMVECTOR look = DirectX::XMVectorSet(lookAt.x, lookAt.y, lookAt.z, 0.0f);
		DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		m_View = DirectX::XMMatrixLookAtLH(eye, look, up);
	}

	virtual void SetProjParams(float fov, float aspect, float nearZ, float farZ)
	{
		m_Fov = fov;
		m_Aspect = aspect;
		m_NearZ = nearZ;
		m_FarZ = farZ;

		m_Proj = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);
	}

	inline const DirectX::XMMATRIX& GetWorldMatrix() const
	{
		return m_World;
	}

	inline const DirectX::XMMATRIX& GetViewMatrix() const
	{
		return m_View;
	}

	inline const DirectX::XMMATRIX& GetProjMatrix() const
	{
		return m_Proj;
	}
protected:
private:
	float m_Fov;
	float m_Aspect;
	float m_NearZ;
	float m_FarZ;

	DirectX::XMFLOAT4 m_EyePos;
	DirectX::XMFLOAT4 m_LookAt;

	DirectX::XMMATRIX m_World;
	DirectX::XMMATRIX m_View;
	DirectX::XMMATRIX m_Proj;
};
