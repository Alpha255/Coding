#include "DXUTCamera.h"

DXUTCamera::DXUTCamera()
{
	SetViewParams(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	SetProjParams(DirectX::XM_PIDIV4, 1.0f, 1.0f, 1000.0f);

	::GetCursorPos((::LPPOINT)&m_MouseAction.LastPosition);
}

void DXUTCamera::HandleWindowMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM)
{
	switch (msg)
	{
	case WM_KEYDOWN:
	{
		auto keyAction = m_KeyAction.GetKeyAction(wParam);
		if (keyAction != KeyboardAction::eOther)
		{
			m_KeyAction.KeyDown(keyAction);
		}
	} 
		break;
	case WM_KEYUP:
	{
		auto keyAction = m_KeyAction.GetKeyAction(wParam);
		if (keyAction != KeyboardAction::eOther)
		{
			m_KeyAction.KeyUp(keyAction);
		}
	}
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
		m_MouseAction.RButtonDown = true;
		m_MouseAction.ButtonMask |= MouseAction::eRightButton;
		break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONDBLCLK:
		m_MouseAction.MButtonDown = true;
		m_MouseAction.ButtonMask |= MouseAction::eMiddleButton;
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		m_MouseAction.LButtonDown = true;
		m_MouseAction.ButtonMask |= MouseAction::eLeftButton;
		break;
	case WM_RBUTTONUP:
		m_MouseAction.RButtonDown = false;
		m_MouseAction.ButtonMask &= (~MouseAction::eRightButton);
		break;
	case WM_MBUTTONUP:
		m_MouseAction.MButtonDown = false;
		m_MouseAction.ButtonMask &= (~MouseAction::eMiddleButton);
		break;
	case WM_LBUTTONUP:
		m_MouseAction.LButtonDown = false;
		m_MouseAction.ButtonMask &= (~MouseAction::eLeftButton);
		break;
	case WM_CAPTURECHANGED:
		break; 
	case WM_MOUSEWHEEL:
		m_MouseAction.WheelDelta += (int32_t)HIWORD(wParam);
		break;
	}

	if (m_MouseAction.RButtonDown || m_MouseAction.MButtonDown || m_MouseAction.LButtonDown)
	{
		::GetCursorPos((LPPOINT)m_MouseAction.LastPosition);
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

void DXUTCamera::UpdateInput()
{
	m_KeyAction.KeyDirection = Vec3(0.0f, 0.0f, 0.0f);

	if (m_KeyAction.IsKeyDown(KeyboardAction::eStrafeRight))
	{
		m_KeyAction.KeyDirection.x += 1.0f;
	}
	if (m_KeyAction.IsKeyDown(KeyboardAction::eStrafeLeft))
	{
		m_KeyAction.KeyDirection.x -= 1.0f;
	}

	if (m_KeyAction.EnableYAxisMovement)
	{
		if (m_KeyAction.IsKeyDown(KeyboardAction::eMoveUp))
		{
			m_KeyAction.KeyDirection.y += 1.0f;
		}
		if (m_KeyAction.IsKeyDown(KeyboardAction::eMoveDown))
		{
			m_KeyAction.KeyDirection.y -= 1.0f;
		}
	}

	if (m_KeyAction.IsKeyDown(KeyboardAction::eMoveForward))
	{
		m_KeyAction.KeyDirection.z += 1.0f;
	}
	if (m_KeyAction.IsKeyDown(KeyboardAction::eMoveBackward))
	{
		m_KeyAction.KeyDirection.z -= 1.0f;
	}

	if (m_MouseAction.ButtonMask & m_MouseAction.RotateButtonMask)
	{
		int32_t curPos[2U] = {};
		int32_t deltaPos[2U] = {};
		::GetCursorPos((LPPOINT)curPos);
		deltaPos[0] = curPos[0] - m_MouseAction.LastPosition[0];
		deltaPos[1] = curPos[1] - m_MouseAction.LastPosition[1];

		m_MouseAction.LastPosition[0] = curPos[0];
		m_MouseAction.LastPosition[1] = curPos[1];

		float percentNew = 1.0f / m_MouseAction.FrameCountToSmoothMouseData;
		float percentOld = 1.0f - percentNew;

		m_MouseAction.Delta.x = m_MouseAction.Delta.x * percentOld + deltaPos[0] * percentNew;
		m_MouseAction.Delta.y = m_MouseAction.Delta.y * percentOld + deltaPos[1] * percentNew;

		m_MouseAction.RotateVelocity = m_MouseAction.Delta * m_MouseAction.RotateScaler;
	}
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
	if (m_KeyAction.IsKeyDown(KeyboardAction::eReset))
	{
		Reset();
	}

	UpdateInput();

	UpdateVelocity(elapsedTime);

	Vec3 deltaPos = m_Velocity * elapsedTime;

	if (m_MouseAction.ButtonMask & m_MouseAction.RotateButtonMask)
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
