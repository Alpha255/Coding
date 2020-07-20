#include "Gear/Gear.h"
#include "Gear/Window.h"

namespaceStart(Gear)

Camera::Camera()
{
	m_World.identity();
	setView(Math::Vec3(0.0f, 0.0f, 0.0f), Math::Vec3(0.0f, 0.0f, 1.0f));
	setPerspective(Math::PI_Div4, 1.0f, 1.0f, 1000.0f);
}

void Camera::setView(const Math::Vec3 &eye, const Math::Vec3 &lookAt)
{
	m_DefaultEye = m_Eye = eye;
	m_DefaultLookAt = m_LookAt = lookAt;

	Math::Vec3 up(0.0f, 1.0f, 0.0f);
	m_View = Math::Matrix::lookAtLH(m_Eye, m_LookAt, up);

	Math::Matrix inverseView = Math::Matrix::inverse(m_View);

	Math::Vec3 zBasis(inverseView.m[2]);
	m_Yaw = atan2f(zBasis.x, zBasis.z);
	float32_t len = sqrtf(zBasis.z * zBasis.z + zBasis.x * zBasis.x);
	m_Pitch = -atan2f(zBasis.y, len);
}

void Camera::setPerspective(float32_t fov, float32_t aspect, float32_t nearPlane, float32_t farPlane)
{
	m_Fov = fov;
	m_Aspect = aspect;
	m_NearPlane = nearPlane;
	m_FarPlane = farPlane;

	m_Projection = Math::Matrix::perspectiveFovLH(fov, aspect, nearPlane, farPlane);
}

void Camera::updateVelocity(float32_t elapsedTime)
{
	Math::Vec3 accel = m_KeyDirection;
	accel.normalize();
	accel *= m_Scaler.y;

	if (dot(accel, accel) > 0.0f)
	{
		m_Velocity = accel;
		m_DragTimer = 0.25f;
		m_VelocityDrag = accel * (1.0f / m_DragTimer);
	}
	else
	{
		if (m_DragTimer > 0.0f)
		{
			m_Velocity -= m_VelocityDrag * elapsedTime;
			m_DragTimer -= elapsedTime;
		}
		else
		{
			m_Velocity = Math::Vec3(0.0f, 0.0f, 0.0f);
		}
	}
}

void Camera::updateKeys(const WindowMessage& message)
{
	m_KeyDirection = Math::Vec3(0.0f, 0.0f, 0.0f);
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
	case eKeyboardKey::eKey_Q:
	case eKeyboardKey::eKey_PageUp:
		m_KeyDirection.y += 1.0f;
		break;
	case eKeyboardKey::eKey_E:
	case eKeyboardKey::eKey_PageDown:
		m_KeyDirection.y -= 1.0f;
		break;
	}

	if (m_LastMouseWheel != message.Mouse.WheelDelta)
	{
		auto delta = message.Mouse.WheelDelta - m_LastMouseWheel;
		m_LastMouseWheel = message.Mouse.WheelDelta;
		m_KeyDirection.z += delta * 0.005f;
	}

	if (isMouseButtonDown(message))
	{
		float32_t percentNew = 1.0f / m_SmoothMouse;
		float32_t percentOld = 1.0f - percentNew;

		Math::Vec2 curMousePos = message.Mouse.Pos;
		Math::Vec2 delta = message.Mouse.Pos - m_LastMousePos;
		m_LastMousePos = curMousePos;

		m_MouseDelta.x = m_MouseDelta.x * percentOld + delta.x * percentNew;
		///m_MouseDelta.y = m_MouseDelta.y * percentOld + delta.y * percentNew;

		m_RotateVelocity = m_MouseDelta * m_Scaler.x;
	}
	else
	{
		m_LastMousePos = message.Mouse.Pos;
	}

	if (message.Mouse.Middle.KeyDown)
	{
		m_KeyDirection += m_MouseDelta;
	}
}

bool8_t Camera::isReset(const WindowMessage& message) const
{
	return message.Key == eKeyboardKey::eKey_Home;
}

bool8_t Camera::isLButtonDown(const WindowMessage& message) const
{
	return message.Mouse.Left.KeyDown;
}

bool8_t Camera::isRButtonDown(const WindowMessage& message) const
{
	return message.Mouse.Right.KeyDown;
}

bool8_t Camera::isMouseButtonDown(const WindowMessage& message) const
{
	return message.Mouse.Left.KeyDown || message.Mouse.Right.KeyDown || message.Mouse.Middle.KeyDown;
}

void Camera::processMessage(const WindowMessage& message, float32_t elapsedTime, uint32_t width, uint32_t height)
{
	if (isReset(message))
	{
		reset();
	}

	if (message.State == eWindowState::eResized)
	{
		handleWindowResize(width, height);
	}

	updateKeys(message);

	updateVelocity(elapsedTime);

	Math::Vec3 deltaPos = m_Velocity * elapsedTime;

	if (message.Key == eKeyboardKey::eKey_Alt)
	{
		//m_Eye -= deltaPos;

		//if (isLButtonDown(message))
		//{
		//	m_RotateDelta.x += m_MouseDelta.y;
		//	m_RotateDelta.y -= m_MouseDelta.x;
		//}

		//m_View = Math::Matrix::rotateX(m_RotateDelta.x) * Math::Matrix::rotateY(m_RotateDelta.y) * Math::Matrix::translate(m_Eye);
	}
	else
	{
		if (isLButtonDown(message) || isRButtonDown(message))
		{
			m_Yaw += m_RotateVelocity.x;
			m_Pitch += m_RotateVelocity.y;

			/// Limit pitch to straight up or straight down
			m_Pitch = std::max<float32_t>(-Math::PI_Div2, m_Pitch);
			m_Pitch = std::min<float32_t>(Math::PI_Div2, m_Pitch);
		}

		Math::Vec3 up(0.0f, 1.0f, 0.0f);
		Math::Vec3 forward(0.0f, 0.0f, 1.0f);
		Math::Matrix rotate = Math::Matrix::rotateRollPitchYaw(m_Pitch, m_Yaw, 0.0f);

		Math::Vec3 worldUp = Math::Vec3::transformCoord(up, rotate);
		Math::Vec3 worldForward = Math::Vec3::transformCoord(forward, rotate);
		Math::Vec3 deltaWorld = Math::Vec3::transformCoord(deltaPos, rotate);

		m_Eye += deltaWorld;

		/// Clip to boundary

		m_LookAt = m_Eye + worldForward;
		m_View = Math::Matrix::lookAtLH(m_Eye, m_LookAt, worldUp);
		m_World = Math::Matrix::inverse(m_View);
	}
}

namespaceEnd(Gear)
