#pragma once

#include "Gear/Camera/ICamera.h"

namespaceStart(Gear)

class Camera : public ICamera
{
public:
	Camera() = default;

	void setPerspective(float32_t fov, float32_t aspect, float32_t nearPlane, float32_t farPlane);

	void processMessage(const struct WindowMessage& message, float32_t elapsedTime) override final;

	void handleWindowResize(uint32_t width, uint32_t height) override final
	{
		setPerspective(m_Fov, width * 1.0f / height, m_NearPlane, m_FarPlane);
	}

	inline Math::Matrix WVPMatrix() const
	{
		return m_View * m_Projection;
	}

	inline void setPosition(const Math::Vec3& translation, const Math::Vec3& rotation)
	{
		m_Translation = translation;
		m_Rotation = rotation;

		m_DefaultTranslation = translation;
		m_DefaultRotation = rotation;

		setViewMatrix();
	}
protected:
	enum eType
	{
		eFirstPerson,
		eModelViewer
	};

	inline void reset()
	{
		m_Translation = m_DefaultTranslation;
		m_Rotation = m_DefaultRotation;

		setViewMatrix();
	}

	inline void translate(const Math::Vec3& trans)
	{
		m_Translation += trans;
		setViewMatrix();
	}

	inline void rotate(const Math::Vec3& rotate)
	{
		m_Rotation += rotate;
		setViewMatrix();
	}

	void updateMoving(float32_t elapsedTime);

	void processInputs(const struct WindowMessage& message);

	bool8_t isReset(const struct WindowMessage& message) const;

	void setViewMatrix();
private:
	Math::Vec3 m_Velocity;
	Math::Vec3 m_VelocityDelta;

	float32_t m_SmoothTimer = 0.25f;
	Math::Vec2 m_Scaler{ 1.0f, 5.0f };

	Math::Vec3 m_Translation;
	Math::Vec3 m_Rotation;

	Math::Vec3 m_DefaultTranslation;
	Math::Vec3 m_DefaultRotation;

	Math::Vec3 m_Move;
	Math::Vec2 m_MouseDelta;
	Math::Vec2 m_LastMousePos;
	int16_t m_LastMouseWheelDelta = 0;

	eType m_Type = eFirstPerson;
	bool8_t m_Moving = false;
};

namespaceEnd(Gear)
