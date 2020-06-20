#include "Gear/Gear.h"
#include "Gear/Window.h"

namespaceStart(Gear)

void DXUTCamera::setView(const Math::Vec3 &eye, const Math::Vec3 &lookAt)
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

void DXUTCamera::setPerspective(float32_t fov, float32_t aspect, float32_t nearPlane, float32_t farPlane)
{
	m_Fov = fov;
	m_Aspect = aspect;
	m_NearPlane = nearPlane;
	m_FarPlane = farPlane;

	m_Projection = Math::Matrix::perspectiveFovLH(fov, aspect, nearPlane, farPlane);
}

void DXUTCamera::updateVelocity(float32_t elapsedTime)
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

void DXUTCamera::updateKeys(const WindowMessage& message)
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

	m_KeyDirection.z += message.Mouse.WheelDelta * 0.005f;

	if (isRotate(message))
	{
		float32_t percentNew = 1.0f / m_SmoothMouse;
		float32_t percentOld = 1.0f - percentNew;

		Math::Vec2 curMousePos = message.Mouse.Pos;
		Math::Vec2 delta = message.Mouse.Pos - m_LastMousePos;
		m_LastMousePos = curMousePos;

		m_MouseDelta.x = m_MouseDelta.x * percentOld + delta.x * percentNew;
		m_MouseDelta.y = m_MouseDelta.y * percentOld + delta.y * percentNew;

		m_RotateVelocity = m_MouseDelta * m_Scaler.x;
	}
	else
	{
		m_LastMousePos = message.Mouse.Pos;
	}
}

bool8_t DXUTCamera::isReset(const WindowMessage& message) const
{
	return message.Key == eKeyboardKey::eKey_Home;
}

bool8_t DXUTCamera::isRotate(const WindowMessage& message) const
{
	return message.Mouse.Left.KeyDown;
}

void DXUTCamera::processMessage(const WindowMessage& message, float32_t elapsedTime)
{
	if (isReset(message))
	{
		reset();
	}

	updateKeys(message);

	updateVelocity(elapsedTime);

	Math::Vec3 deltaPos = m_Velocity * elapsedTime;

	if (isRotate(message))
	{
		float32_t deltaYaw = m_RotateVelocity.x;
		float32_t deltaPitch = m_RotateVelocity.y;

		m_Pitch += deltaPitch;
		m_Yaw += deltaYaw;

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

	///m_View = Math::Matrix::rotateX(m_MouseDelta.x) * Math::Matrix::rotateY(m_MouseDelta.y) * Math::Matrix::translate(m_Eye);

	m_Eye += deltaWorld;

	/// Clip to boundary

	m_LookAt = m_Eye + worldForward;

	m_View = Math::Matrix::lookAtLH(m_Eye, m_LookAt, worldUp);
	m_World = Math::Matrix::inverse(m_View);
}

namespaceEnd(Gear)
