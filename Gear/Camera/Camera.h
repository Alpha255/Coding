#pragma once

#include "Gear/Camera/ICamera.h"

/// From DXUT

namespaceStart(Gear)

class Camera : public ICamera
{
public:
	Camera();

	void setView(const Math::Vec3& eye, const Math::Vec3& lookAt);
	void setPerspective(float32_t fov, float32_t aspect, float32_t nearPlane, float32_t farPlane);

	void processMessage(const struct WindowMessage& message, float32_t elapsedTime, uint32_t width, uint32_t height) override final;

	void handleWindowResize(uint32_t width, uint32_t height) override final
	{
		setPerspective(m_Fov, width * 1.0f / height, m_NearPlane, m_FarPlane);
	}

	inline Math::Matrix WVPMatrix() const
	{
		return m_View * m_Projection;
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
	bool8_t isMouseButtonDown(const struct WindowMessage& message) const;
private:
	Math::Vec3 m_DefaultEye;
	Math::Vec3 m_DefaultLookAt;

	Math::Vec3 m_Velocity;
	Math::Vec3 m_VelocityDrag;

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
};

namespaceEnd(Gear)
