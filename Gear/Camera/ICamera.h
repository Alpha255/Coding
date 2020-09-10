#pragma once

#include "Gear/Math/Math.h"

NAMESPACE_START(Gear)

class ICamera
{
public:
	virtual void processMessage(const struct WindowMessage& message, float32_t elapsedTime) = 0;
	virtual void handleWindowResize(uint32_t width, uint32_t height) = 0;

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

	inline const Math::Vec3& eye() const
	{
		return m_Eye;
	}

	virtual ~ICamera() = default;
protected:
	Math::Matrix m_World;
	Math::Matrix m_View;
	Math::Matrix m_Projection;
	Math::Vec3 m_Eye;

	float32_t m_Fov = 0.0f;
	float32_t m_Aspect = 1.0f;
	float32_t m_NearPlane = 0.0f;
	float32_t m_FarPlane = 0.0f;
};

NAMESPACE_END(Gear)
