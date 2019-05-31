#include "DXUTCamera.h"

DXUTCamera::DXUTCamera()
{
	SetViewParams(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	SetProjParams(DirectX::XM_PIDIV4, 1.0f, 1.0f, 1000.0f);

	::GetCursorPos((::LPPOINT)&m_MouseAction.LastPosition);
}

void DXUTCamera::KeyboardAction::HandleWindowMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM)
{
	auto keyAction = GetKeyAction(wParam);
	if (keyAction != KeyboardAction::eOther)
	{
		switch (msg)
		{
		case WM_KEYDOWN:
			ActiveKey(keyAction);
		break;
		case WM_KEYUP:
			InactiveKey(keyAction);
		break;
		}
	}
}

void DXUTCamera::MouseAction::HandleWindowMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM)
{
	switch (msg)
	{
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
		RButtonDown = true;
		ButtonMask |= eRightButton;
		break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONDBLCLK:
		MButtonDown = true;
		ButtonMask |= eMiddleButton;
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		LButtonDown = true;
		ButtonMask |= eLeftButton;
		break;
	case WM_RBUTTONUP:
		RButtonDown = false;
		ButtonMask &= ~eRightButton;
		break;
	case WM_MBUTTONUP:
		MButtonDown = false;
		ButtonMask &= ~eMiddleButton;
		break;
	case WM_LBUTTONUP:
		LButtonDown = false;
		ButtonMask &= ~eLeftButton;
		break;
	case WM_CAPTURECHANGED:
		break;
	case WM_MOUSEWHEEL:
		WheelDelta += (int32_t)HIWORD(wParam);
		break;
	}

	if (RButtonDown || MButtonDown || LButtonDown)
	{
		::GetCursorPos((LPPOINT)LastPosition);
	}
}

void DXUTCamera::KeyboardAction::UpdateInput()
{
	KeyDirection = Vec3(0.0f, 0.0f, 0.0f);

	if (IsKeyActive(eStrafeRight))
	{
		KeyDirection.x += 1.0f;
	}
	if (IsKeyActive(KeyboardAction::eStrafeLeft))
	{
		KeyDirection.x -= 1.0f;
	}

	if (EnableYAxisMovement)
	{
		if (IsKeyActive(KeyboardAction::eMoveUp))
		{
			KeyDirection.y += 1.0f;
		}
		if (IsKeyActive(KeyboardAction::eMoveDown))
		{
			KeyDirection.y -= 1.0f;
		}
	}

	if (IsKeyActive(KeyboardAction::eMoveForward))
	{
		KeyDirection.z += 1.0f;
	}
	if (IsKeyActive(KeyboardAction::eMoveBackward))
	{
		KeyDirection.z -= 1.0f;
	}
}

void DXUTCamera::MouseAction::UpdateInput()
{
	if (ButtonMask & RotateButtonMask)
	{
		int32_t curPos[2U] = {};
		int32_t deltaPos[2U] = {};
		::GetCursorPos((LPPOINT)curPos);
		deltaPos[0] = curPos[0] - LastPosition[0];
		deltaPos[1] = curPos[1] - LastPosition[1];

		LastPosition[0] = curPos[0];
		LastPosition[1] = curPos[1];

		float percentNew = 1.0f / FrameCountToSmoothMouseData;
		float percentOld = 1.0f - percentNew;

		Delta.x = Delta.x * percentOld + deltaPos[0] * percentNew;
		Delta.y = Delta.y * percentOld + deltaPos[1] * percentNew;

		RotateVelocity = Delta * RotateScaler;
	}
}

void DXUTCamera::SetViewParams(const Vec3 &eye, const Vec3 &lookAt)
{
	m_DefaultEye = m_Eye = eye;
	m_DefaultLookAt = m_LookAt = lookAt;

	Vec3 up(0.0f, 1.0f, 0.0f);
	m_View = Matrix::LookAtLH(m_Eye, m_LookAt, up);

	Matrix inverseView = Matrix::Inverse(m_View);

	Vec3 zBasis(inverseView.m[2]);
	m_Yaw = atan2f(zBasis.x, zBasis.z);
	float len = sqrtf(zBasis.z * zBasis.z + zBasis.x * zBasis.x);
	m_Pitch = -atan2f(zBasis.y, len);
}

void DXUTCamera::SetProjParams(float fov, float aspect, float nearPlane, float farPlane)
{
	m_FOV = fov;
	m_Aspect = aspect;
	m_NearPlane = nearPlane;
	m_FarPlane = farPlane;

	m_Proj = Matrix::PerspectiveFovLH(fov, aspect, nearPlane, farPlane);
}

DXUTCamera::KeyboardAction::eKeyAction DXUTCamera::KeyboardAction::GetKeyAction(WPARAM wParam)
{
	switch (wParam)
	{
	case VK_CONTROL: return eControlDown;
	case VK_LEFT: return eStrafeLeft;
	case VK_RIGHT: return eStrafeRight;
	case VK_UP: return eMoveForward;
	case VK_DOWN: return eMoveBackward;
	case VK_PRIOR: return eMoveUp;       /// page up
	case VK_NEXT: return eMoveDown;      /// page down

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

void DXUTCamera::UpdateVelocity(float elapsedTime)
{
	Vec3 accel = m_KeyAction.KeyDirection;
	accel.Normalize();
	accel *= m_MouseAction.MoveScaler;

	if (accel.LengthSq() > 0.0f)
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
			m_Velocity = Vec3(0.0f, 0.0f, 0.0f);
		}
	}
}

void DXUTCamera::Update(float elapsedTime)
{
	if (m_KeyAction.IsKeyActive(KeyboardAction::eReset))
	{
		Reset();
	}

	UpdateInput();

	UpdateVelocity(elapsedTime);

	Vec3 deltaPos = m_Velocity * elapsedTime;

	if (m_MouseAction.IsRotateButtonActive())
	{
		float deltaYaw = m_MouseAction.RotateVelocity.x;
		float deltaPitch = m_MouseAction.RotateVelocity.y;

		m_Pitch += deltaPitch;
		m_Yaw += deltaYaw;

		m_Pitch = std::max<float>(-DirectX::XM_PIDIV2, m_Pitch);
		m_Pitch = std::min<float>(DirectX::XM_PIDIV2, m_Pitch);
	}

	Matrix rotate = Matrix::RotateRollPitchYaw(m_Pitch, m_Yaw, 0.0f);

	Vec3 up(0.0f, 1.0f, 0.0f);
	Vec3 forward(0.0f, 0.0f, 1.0f);

	Vec3 worldUp = Vec3::TransformCoord(up, rotate);
	Vec3 worldForward = Vec3::TransformCoord(forward, rotate);

	Vec3 deltaWorld = Vec3::TransformCoord(deltaPos, rotate);

	m_Eye += deltaWorld;

	/// Clip to boundary

	m_LookAt = m_Eye + worldForward;

	m_View = Matrix::LookAtLH(m_Eye, m_LookAt, worldUp);
	m_View.Inverse();
}
