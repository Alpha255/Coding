#include "Gear/Gear.h"
#include "Gear/Window.h"

namespaceStart(Gear)

void Camera::setPerspective(float32_t fov, float32_t aspect, float32_t nearPlane, float32_t farPlane)
{
	m_Fov = fov;
	m_Aspect = aspect;
	m_NearPlane = nearPlane;
	m_FarPlane = farPlane;

	///m_Projection = Math::Matrix::perspectiveFovLH(fov, aspect, nearPlane, farPlane);
	m_Projection = Math::Matrix::perspectiveFovRH(fov, aspect, nearPlane, farPlane);
}

void Camera::updateVelocity(float32_t elapsedTime)
{
	Math::Vec3 accel = m_KeyDirection;
	accel.normalize();
	accel *= m_Scaler.y;

	if (dot(accel, accel) > 0.0f)
	{
		m_Velocity = accel;
		m_SmoothTimer = 0.25f;
		m_VelocityDelta = accel * (1.0f / m_SmoothTimer);
	}
	else
	{
		if (m_SmoothTimer > 0.0f)
		{
			m_Velocity -= m_VelocityDelta * elapsedTime;
			m_SmoothTimer -= elapsedTime;
		}
		else
		{
			m_Velocity = Math::Vec3(0.0f, 0.0f, 0.0f);
		}
	}
}

void Camera::processInputs(const WindowMessage& message)
{
	m_KeyDirection = Math::Vec3(0.0f, 0.0f, 0.0f);
	m_Moving = true;
	switch (message.Key)
	{
	case eKeyboardKey::eKey_A:
	case eKeyboardKey::eKey_Left:
		m_KeyDirection.x += 1.0f;
		break;
	case eKeyboardKey::eKey_D:
	case eKeyboardKey::eKey_Right:
		m_KeyDirection.x += -1.0f;
		break;
	case eKeyboardKey::eKey_W:
	case eKeyboardKey::eKey_Up:
		m_KeyDirection.z += 1.0f;
		break;
	case eKeyboardKey::eKey_S:
	case eKeyboardKey::eKey_Down:
		m_KeyDirection.z -= 1.0f;
		break;
	default:
		m_Moving = false;
		break;
	}

	if (m_LastMouseWheel != message.Mouse.WheelDelta)
	{
		auto delta = message.Mouse.WheelDelta - m_LastMouseWheel;
		m_LastMouseWheel = message.Mouse.WheelDelta;
		m_KeyDirection.z += delta * 0.005f;
		m_Moving = true;
	}
	else
	{
		m_Moving = false;
	}

	if (message.Mouse.Left.KeyDown)
	{
		///m_Type = eFirstPerson;
	}
	else if (message.Mouse.Right.KeyDown)
	{
		Math::Vec2 curMousePos = message.Mouse.Pos;
		Math::Vec2 delta = message.Mouse.Pos - m_LastMousePos;
		m_LastMousePos = curMousePos;
		delta *= m_Scaler.x;

		rotate(Math::Vec3(delta.y, -delta.x));

		///m_Type = eModelViewer;
	}
	else if (message.Mouse.Middle.KeyDown)
	{
	}
	else
	{
		m_LastMousePos = message.Mouse.Pos;
	}
}

bool8_t Camera::isReset(const WindowMessage& message) const
{
	return message.Key == eKeyboardKey::eKey_Home;
}

void Camera::setViewMatrix()
{
	Math::Matrix translation = Math::Matrix::translate(m_Translation);

	Math::Matrix rotateX = Math::Matrix::rotateX(m_Rotation.x);
	Math::Matrix rotateY = Math::Matrix::rotateY(m_Rotation.y);
	Math::Matrix rotateZ = Math::Matrix::rotateZ(m_Rotation.z);

	Math::Matrix rotation = rotateZ * rotateY * rotateX;

	if (m_Type == eFirstPerson)
	{
		m_View = rotation * translation;
	}
	else if (m_Type == eModelViewer)
	{
		m_View = translation * rotation;
	}

	m_Eye = -1.0f * m_Translation;
}

void Camera::processMessage(const WindowMessage& message, float32_t elapsedTime)
{
	if (isReset(message))
	{
		reset();
	}

	processInputs(message);

	updateVelocity(elapsedTime);

	if (m_Moving)
	{
		Math::Vec3 deltaPos = m_Velocity * elapsedTime;

		Math::Vec3 front;
		front.x = -cosf(Math::radians(m_Rotation.x) * sinf(Math::radians(m_Rotation.y)));
		front.y = sinf(Math::radians(m_Rotation.x));
		front.z = cos(Math::radians(m_Rotation.x) * cosf(Math::radians(m_Rotation.y)));
		front.normalize();

		m_Translation += m_Velocity;

		setViewMatrix();
	}
}

namespaceEnd(Gear)
