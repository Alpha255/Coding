#include "dxutCamera.h"
#include "Gear/Public/Extension/Event.h"

namespaceStart(gear)

dxutCamera::dxutCamera()
{
	m_World.identity();

	setViewParams(math::vec3(0.0f, 0.0f, 0.0f), math::vec3(0.0f, 0.0f, 1.0f));
	setProjParams(math::g_pi_div4, 1.0f, 1.0f, 1000.0f);

	m_MouseAction.LastPosition = eventHandler::instance().getMousePosition();
}

void dxutCamera::keyboardAction::processEvent()
{
	auto action = getKeyAction();
	if (action != keyboardAction::eOther)
	{
		auto keyEvent = eventHandler::instance().getKeyboardEvent();
		switch (keyEvent)
		{
		case eKeyboardEvent::eKeyDown:
			activeKey(action);
			break;
		case eKeyboardEvent::eKeyUp:
			inactiveKey(action);
			break;
		}
	}
}

void dxutCamera::mouseAction::processEvent()
{
	auto mouseEvent = eventHandler::instance().getMouseEvent();
	switch (mouseEvent)
	{
	case eMouseEvent::eRightClick:
	case eMouseEvent::eRightDoubleClick:
		ButtonDown[eRightButton] = true;
		LastPosition = eventHandler::instance().getMousePosition();
		break;
	case eMouseEvent::eMiddleClick:
	case eMouseEvent::eMiddleDoubleClick:
		ButtonDown[eMiddleButton] = true;
		LastPosition = eventHandler::instance().getMousePosition();
		break;
	case eMouseEvent::eLeftClick:
	case eMouseEvent::eLeftDoubleClick:
		ButtonDown[eLeftButton] = true;
		LastPosition = eventHandler::instance().getMousePosition();
		break;
	case eMouseEvent::eRightUp:
		ButtonDown[eRightButton] = false;
		break;
	case eMouseEvent::eMiddleUp:
		ButtonDown[eMiddleButton] = false;
		break;
	case eMouseEvent::eLeftUp:
		ButtonDown[eLeftButton] = false;
		break;
	///case WM_MOUSEWHEEL:
		///WheelDelta += ((GET_WHEEL_DELTA_WPARAM(wParam)) * 0.005f);
		///ButtonDown[eWheel] = true;
		///break;
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
		math::vec2 curPos = eventHandler::instance().getMousePosition();
		math::vec2 deltaPos = curPos - LastPosition;
		LastPosition = curPos;

		float32_t percentNew = 1.0f / FrameCountToSmoothMouseData;
		float32_t percentOld = 1.0f - percentNew;

		Delta.x = Delta.x * percentOld + deltaPos.x * percentNew;
		Delta.y = Delta.y * percentOld + deltaPos.y * percentNew;

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

dxutCamera::keyboardAction::eKeyAction dxutCamera::keyboardAction::getKeyAction()
{
	auto key = eventHandler::instance().getKeyboardKey();
	switch (key)
	{
	case eKeyboardKey::eKey_Left:
	case eKeyboardKey::eKey_A:
		return eStrafeLeft;
	case eKeyboardKey::eKey_Right:
	case eKeyboardKey::eKey_D:
		return eStrafeRight;
	case eKeyboardKey::eKey_Up:
	case eKeyboardKey::eKey_W:
		return eMoveForward;
	case eKeyboardKey::eKey_Down:
	case eKeyboardKey::eKey_S:
		return eMoveBackward;
	case eKeyboardKey::eKey_PageUp:
	case eKeyboardKey::eKey_E:
		return eMoveUp;
	case eKeyboardKey::eKey_PageDown:
	case eKeyboardKey::eKey_Q:
		return eMoveDown;
	case eKeyboardKey::eKey_Home:
		return eReset;
	}

	return eOther;
}

void dxutCamera::updateVelocity(float32_t elapsedTime)
{
	math::vec3 accel = m_KeyAction.KeyDirection;
	accel.normalize();
	accel *= m_MouseAction.MoveScaler;

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
		m_Pitch = std::max<float32_t>(-math::g_pi_div2, m_Pitch);
		m_Pitch = std::min<float32_t>(math::g_pi_div2, m_Pitch);
	}

	math::vec3 up(0.0f, 1.0f, 0.0f);
	math::vec3 forward(0.0f, 0.0f, 1.0f);
	math::matrix rotate = math::matrix::setRotateRollPitchYaw(m_Pitch, m_Yaw, 0.0f);

	math::vec3 worldUp = math::vec3::transformCoord(up, rotate);
	math::vec3 worldForward = math::vec3::transformCoord(forward, rotate);
	math::vec3 deltaWorld = math::vec3::transformCoord(deltaPos, rotate);

	m_Eye += deltaWorld;

	/// Clip to boundary

	m_LookAt = m_Eye + worldForward;

	m_View = math::matrix::lookAtLH(m_Eye, m_LookAt, worldUp);
	m_World = math::matrix::inverse(m_View);

	m_MouseAction.ButtonDown[mouseAction::eWheel] = false;
}

namespaceEnd(gear)