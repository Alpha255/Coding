#include "dxutCamera.h"

namespaceStart(gear)

dxutCamera::dxutCamera()
{
	m_World.identity();

	setViewParams(math::vec3(0.0f, 0.0f, 0.0f), math::vec3(0.0f, 0.0f, 1.0f));
	setProjParams(math::g_pi_div4, 1.0f, 1.0f, 1000.0f);

	::GetCursorPos((::LPPOINT)m_MouseAction.LastPosition);
}

void dxutCamera::keyboardAction::handleMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM)
{
	auto action = keyAction(wParam);
	if (action != keyboardAction::eOther)
	{
		switch (msg)
		{
		case WM_KEYDOWN:
			activeKey(action);
			break;
		case WM_KEYUP:
			inactiveKey(action);
			break;
		}
	}
}

void dxutCamera::mouseAction::handleMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM)
{
	switch (msg)
	{
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
		ButtonDown[eRightButton] = true;
		::GetCursorPos((::LPPOINT)LastPosition);
		break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONDBLCLK:
		ButtonDown[eMiddleButton] = true;
		::GetCursorPos((::LPPOINT)LastPosition);
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		ButtonDown[eLeftButton] = true;
		::GetCursorPos((::LPPOINT)LastPosition);
		break;
	case WM_RBUTTONUP:
		ButtonDown[eRightButton] = false;
		break;
	case WM_MBUTTONUP:
		ButtonDown[eMiddleButton] = false;
		break;
	case WM_LBUTTONUP:
		ButtonDown[eLeftButton] = false;
		break;
	case WM_CAPTURECHANGED:
		break;
	case WM_MOUSEWHEEL:
		WheelDelta += (int32_t)HIWORD(wParam);
		break;
	}
}

void dxutCamera::keyboardAction::updateInput()
{
	KeyDirection = math::vec3(0.0f, 0.0f, 0.0f);

	if (isKeyActive(eStrafeRight))
	{
		KeyDirection.x += 1.0f;
	}
	if (isKeyActive(eStrafeLeft))
	{
		KeyDirection.x -= 1.0f;
	}

	if (EnableYAxisMovement)
	{
		if (isKeyActive(eMoveUp))
		{
			KeyDirection.y += 1.0f;
		}
		if (isKeyActive(eMoveDown))
		{
			KeyDirection.y -= 1.0f;
		}
	}

	if (isKeyActive(eMoveForward))
	{
		KeyDirection.z += 1.0f;
	}
	if (isKeyActive(eMoveBackward))
	{
		KeyDirection.z -= 1.0f;
	}
}

void dxutCamera::mouseAction::updateInput()
{
	if (isRotateButtonDown())
	{
		int32_t curPos[2U] = {};
		int32_t deltaPos[2U] = {};
		::GetCursorPos((::LPPOINT)curPos);
		deltaPos[0] = curPos[0] - LastPosition[0];
		deltaPos[1] = curPos[1] - LastPosition[1];

		LastPosition[0] = curPos[0];
		LastPosition[1] = curPos[1];

		float32_t percentNew = 1.0f / FrameCountToSmoothMouseData;
		float32_t percentOld = 1.0f - percentNew;

		Delta.x = Delta.x * percentOld + deltaPos[0] * percentNew;
		Delta.y = Delta.y * percentOld + deltaPos[1] * percentNew;

		RotateVelocity = Delta * RotateScaler;
	}
}

void dxutCamera::setViewParams(const math::vec3 &eye, const math::vec3 &lookAt)
{
	m_DefaultEye = m_Eye = eye;
	m_DefaultLookAt = m_LookAt = lookAt;

	math::vec3 up(0.0f, 1.0f, 0.0f);
	m_View = math::matrix::lookAtLH(m_Eye, m_LookAt, up);

	math::matrix inverseView = math::matrix::inverse(m_View);

	math::vec3 zBasis(inverseView.m[2]);
	m_Yaw = atan2f(zBasis.x, zBasis.z);
	float32_t len = sqrtf(zBasis.z * zBasis.z + zBasis.x * zBasis.x);
	m_Pitch = -atan2f(zBasis.y, len);
}

void dxutCamera::setProjParams(float32_t fov, float32_t aspect, float32_t nearPlane, float32_t farPlane)
{
	m_FOV = fov;
	m_Aspect = aspect;
	m_NearPlane = nearPlane;
	m_FarPlane = farPlane;

	m_Proj = math::matrix::perspectiveFovLH(fov, aspect, nearPlane, farPlane);
}

dxutCamera::keyboardAction::eKeyAction dxutCamera::keyboardAction::keyAction(WPARAM wParam)
{
	switch (wParam)
	{
	case VK_CONTROL: return eControlDown;
	case VK_LEFT:    return eStrafeLeft;
	case VK_RIGHT:   return eStrafeRight;
	case VK_UP:      return eMoveForward;
	case VK_DOWN:    return eMoveBackward;
	case VK_PRIOR:   return eMoveUp;        /// page up
	case VK_NEXT:    return eMoveDown;      /// page down

	case 'A': return eStrafeLeft;
	case 'D': return eStrafeRight;
	case 'W': return eMoveForward;
	case 'S': return eMoveBackward;
	case 'Q': return eMoveDown;
	case 'E': return eMoveUp;

	case VK_NUMPAD4: return eStrafeLeft;
	case VK_NUMPAD6: return eStrafeRight;
	case VK_NUMPAD8: return eMoveForward;
	case VK_NUMPAD2: return eMoveBackward;
	case VK_NUMPAD9: return eMoveUp;
	case VK_NUMPAD3: return eMoveDown;
	case VK_HOME: return eReset;
	}

	return eOther;
}

void dxutCamera::updateVelocity(float32_t elapsedTime)
{
	math::vec3 accel = m_KeyAction.KeyDirection;
	accel.normalize();
	accel *= m_MouseAction.MoveScaler;

	if (accel * accel > 0.0f)
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
			m_Velocity = math::vec3(0.0f, 0.0f, 0.0f);
		}
	}
}

void dxutCamera::update(float32_t elapsedTime)
{
	if (m_KeyAction.isKeyActive(keyboardAction::eReset))
	{
		reset();
	}

	updateInput();

	updateVelocity(elapsedTime);

	math::vec3 deltaPos = m_Velocity * elapsedTime;

	if (m_MouseAction.isRotateButtonDown())
	{
		float32_t deltaYaw = m_MouseAction.RotateVelocity.x;
		float32_t deltaPitch = m_MouseAction.RotateVelocity.y;

		m_Pitch += deltaPitch;
		m_Yaw += deltaYaw;

		/// Limit pitch to straight up or straight down
		m_Pitch = std::max<float32_t>(-DirectX::XM_PIDIV2, m_Pitch);
		m_Pitch = std::min<float32_t>(DirectX::XM_PIDIV2, m_Pitch);
	}

	///math::matrix rotate = math::matrix::RotateRollPitchYaw(m_Pitch, m_Yaw, 0.0f);

	math::vec3 up(0.0f, 1.0f, 0.0f);
	math::vec3 forward(0.0f, 0.0f, 1.0f);

	///math::vec3 worldUp = TransformCoord(up, rotate);
	///math::vec3 worldForward = TransformCoord(forward, rotate);

	///math::vec3 deltaWorld = TransformCoord(deltaPos, rotate);

	///m_Eye += deltaWorld;

	/// Clip to boundary

	///m_LookAt = m_Eye + worldForward;

	///m_View = math::matrix::lookAtLH(m_Eye, m_LookAt, worldUp);
	m_World = math::matrix::inverse(m_View);
}

namespaceEnd(gear)