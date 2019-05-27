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
		eKeyAction action = GetKeyAction(wParam);
		if (eUnknown != action)
		{

		}
	} 
		break;
	case WM_KEYUP:
		break;
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
		break;
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_LBUTTONUP:
		break;
	case WM_CAPTURECHANGED:
		break; 
	case WM_MOUSEWHEEL:
		m_MouseAction.WheelDelta += (int32_t)HIWORD(wParam);
		break;
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

DXUTCamera::eKeyAction DXUTCamera::GetKeyAction(WPARAM wParam)
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

	return eUnknown;
}
