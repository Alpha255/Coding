#pragma once

#include "Gear/Math/Math.h"

namespaceStart(Gear)

class Camera
{
public:
	inline float32_t nearPlane() const
	{
		return m_Near;
	}

	inline float32_t farPlane() const
	{
		return m_Far;
	}

	inline const Math::Matrix& viewMatrix() const
	{
		return m_View;
	}

	inline const Math::Matrix& projectionMatrix() const
	{
		return m_Projection;
	}

	void setPerspective(float32_t fov, float32_t aspect, float32_t nearPlane, float32_t farPlane)
	{
		m_Fov = fov;
		m_Near = nearPlane;
		m_Far = farPlane;
		m_Projection = Math::Matrix::perspectiveFovLH(Math::radians(fov), aspect, nearPlane, farPlane);
	}

	void setTranslation(const Math::Vec3& translation)
	{
		m_Position = translation;
		updateViewMatrix();
	}

	void setRotation(const Math::Vec3& rotation)
	{
		m_Rotation = rotation;
		updateViewMatrix();
	}

	void rotate(const Math::Vec3& rotation)
	{
		m_Rotation += rotation;
		updateViewMatrix();
	}

	void translate(const Math::Vec3& translation)
	{
		m_Position += translation;
		updateViewMatrix();
	}

	void processMessage(const struct WindowMessage& message, float32_t elapsedTime, uint32_t width, uint32_t height);
protected:
	struct Keys
	{
		bool8_t Left = false;
		bool8_t Right = false;
		bool8_t Up = false;
		bool8_t Down = false;

		inline bool8_t isMoving() const
		{
			return Left || Right || Up || Down;
		}

		inline void reset()
		{
			Left = Right = Up = Down = false;
		}
	};

	void updateViewMatrix();

	void updateKeys(const struct WindowMessage& message);

	inline void updatePerspective(float32_t aspect)
	{
		m_Projection = Math::Matrix::perspectiveFovLH(Math::radians(m_Fov), aspect, m_Near, m_Far);
	}
private:
	float32_t m_Fov = 0.0f;
	float32_t m_Near = 0.0f;
	float32_t m_Far = 0.0f;
	float32_t m_RotateSpeed = 1.0f;
	float32_t m_MoveSpeed = 1.0f;

	Math::Vec3 m_Rotation;
	Math::Vec3 m_Position;
	Math::Matrix m_Projection;
	Math::Matrix m_View;

	Keys m_Keys;
};

namespaceEnd(Gear)
