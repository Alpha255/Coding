#pragma once

#include "Gear/Math/Matrix.h"

namespaceStart(Gear)

class DXUTCamera
{
public:
	DXUTCamera()
	{
		m_World.identity();
		setView(Math::Vec3(0.0f, 0.0f, 0.0f), Math::Vec3(0.0f, 0.0f, 1.0f));
		setPerspective(Math::PI_Div4, 1.0f, 1.0f, 1000.0f);
	}

	void setView(const Math::Vec3& eye, const Math::Vec3& lookAt);
	void setPerspective(float32_t fov, float32_t aspect, float32_t nearPlane, float32_t farPlane);

	void processMessage(const struct WindowMessage& message, float32_t elapsedTime);

	void handleWindowResize(uint32_t width, uint32_t height)
	{
		setPerspective(m_Fov, width * 1.0f / height, m_NearPlane, m_FarPlane);
	}

	inline const Math::Matrix& worldMatrix() const 
	{
		return m_World;
	}

	inline const Math::Matrix& viewMatrix() const 
	{
		return m_View;
	}

	inline const Math::Matrix& projectionMatrix() const 
	{
		return m_Projection;
	}

	inline Math::Matrix WVPMatrix() const
	{
		return m_View * m_Projection;
	}

	inline const Math::Vec3& eye() const
	{
		return m_Eye;
	}

	inline const Math::Vec3& lookAt() const
	{
		return m_LookAt;
	}

	inline void setScalers(float32_t rotateScaler = 0.01f, float32_t moveScaler = 5.0f)
	{
		m_Scaler = Math::Vec2(rotateScaler, moveScaler);
	}
protected:
	inline void reset()
	{
		m_RotateDelta = Math::Vec2();
		setView(m_DefaultEye, m_DefaultLookAt);
	}

	void updateVelocity(float32_t elapsedTime);

	void updateKeys(const struct WindowMessage& message);

	bool8_t isReset(const struct WindowMessage& message) const;
	bool8_t isLButtonDown(const struct WindowMessage& message) const;
	bool8_t isRButtonDown(const struct WindowMessage& message) const;
private:
	Math::Vec3 m_Eye;
	Math::Vec3 m_LookAt;

	Math::Vec3 m_DefaultEye;
	Math::Vec3 m_DefaultLookAt;

	Math::Vec3 m_Velocity;
	Math::Vec3 m_VelocityDrag;

	float32_t m_Fov = 0.0f;
	float32_t m_Aspect = 1.0f;
	float32_t m_NearPlane = 0.0f;
	float32_t m_FarPlane = 0.0f;

	float32_t m_Yaw = 0.0f;
	float32_t m_Pitch = 0.0f;

	float32_t m_DragTimer = 0.0f;
	uint32_t m_SmoothMouse = 2u;
	int16_t m_LastMouseWheel = 0;

	Math::Vec2 m_RotateVelocity;
	Math::Vec3 m_KeyDirection;
	Math::Vec2 m_Scaler{ 0.01f, 5.0f };
	Math::Vec2 m_MouseDelta;
	Math::Vec2 m_LastMousePos;
	Math::Vec2 m_RotateDelta;

	Math::Matrix m_World;
	Math::Matrix m_View;
	Math::Matrix m_Projection;
};

namespaceEnd(Gear)
