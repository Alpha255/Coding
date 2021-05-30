#include "Gear/Gear.h"
#include "Gear/Window.h"

NAMESPACE_START(Gear)

void Camera::setPerspective(float32_t fov, float32_t aspect, float32_t nearPlane, float32_t farPlane)
{
	m_Fov = fov;
	m_Aspect = aspect;
	m_NearPlane = nearPlane;
	m_FarPlane = farPlane;

	m_Projection = Math::Matrix::perspectiveFovLH(fov, aspect, nearPlane, farPlane);
}

void Camera::updateMoving(float32_t elapsedTime)
{
	Math::Vec3 accel = m_Move;
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
			m_Velocity = Math::Vec3();
			m_Moving = false;
		}
	}
}

#if 0
void Camera::processInputs(const WindowMessage& message)
{
	m_Move = Math::Vec3();

	switch (message.Key)
	{
	case eKeyboardKey::eKey_A:
	case eKeyboardKey::eKey_Left:
		m_Moving = true;
		m_Move.x += 1.0f;
		break;
	case eKeyboardKey::eKey_D:
	case eKeyboardKey::eKey_Right:
		m_Moving = true;
		m_Move.x += -1.0f;
		break;
	case eKeyboardKey::eKey_W:
	case eKeyboardKey::eKey_Up:
		m_Moving = true;
		m_Move.z -= 1.0f;
		break;
	case eKeyboardKey::eKey_S:
	case eKeyboardKey::eKey_Down:
		m_Moving = true;
		m_Move.z += 1.0f;
		break;
	}

	if (m_LastMouseWheelDelta != message.Mouse.WheelDelta)
	{
		m_Translation.z += (message.Mouse.WheelDelta - m_LastMouseWheelDelta) * 0.005f;
		setViewMatrix();
		m_LastMouseWheelDelta = message.Mouse.WheelDelta;
	}

	if (message.Mouse.Left.KeyDown || message.Mouse.Right.KeyDown || message.Mouse.Middle.KeyDown)
	{
		Math::Vec2 delta = message.Mouse.Pos - m_LastMousePos;
		delta *= m_Scaler.x;
		
		if (message.Mouse.Left.KeyDown)
		{

		}
		else if (message.Mouse.Right.KeyDown)
		{
			rotate(Math::Vec3(delta.y, -delta.x, 0.0f));
		}
		else if (message.Mouse.Middle.KeyDown)
		{
			translate(Math::Vec3(delta.x * 0.01f, delta.y * 0.01f, 0.0f));
		}
	}

	m_LastMousePos = message.Mouse.Pos;
}

bool8_t Camera::isReset(const WindowMessage& message) const
{
	return message.Key == eKeyboardKey::eKey_Home;
}
#endif

void Camera::setViewMatrix()
{
	Math::Matrix translation = Math::Matrix::translate(m_Translation);

	Math::Matrix rotateX = Math::Matrix::rotateX(m_Rotation.x);
	Math::Matrix rotateY = Math::Matrix::rotateY(m_Rotation.y);
	Math::Matrix rotateZ = Math::Matrix::rotateZ(m_Rotation.z);

	Math::Matrix rotation = rotateX * rotateY * rotateZ;

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

#if 0
void Camera::processMessage(const WindowMessage& message, float32_t elapsedTime)
{
	if (isReset(message))
	{
		reset();
	}

	processInputs(message);

	updateMoving(elapsedTime);

	if (m_Moving)
	{
		//Math::Vec3 front;
		//front.x = -cosf(Math::radians(m_Rotation.x) * sinf(Math::radians(m_Rotation.y)));
		//front.y = sinf(Math::radians(m_Rotation.x));
		//front.z = cos(Math::radians(m_Rotation.x) * cosf(Math::radians(m_Rotation.y)));
		//front.normalize();

		m_Translation += m_Velocity * elapsedTime;

		setViewMatrix();
	}
}
#endif

NAMESPACE_END(Gear)
