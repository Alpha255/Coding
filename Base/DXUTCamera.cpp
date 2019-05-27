#include "DXUTCamera.h"

DXUTCamera::eKeyAction DXUTCamera::GetKeyAction(WPARAM wParam)
{
	switch (wParam)
	{
	case VK_CONTROL:
		return eControlDown;
	case VK_LEFT:
		return eStrafeLeft;
	case VK_RIGHT:
		return eStrafeRight;
	case VK_UP:
		return eMoveForward;
	case VK_DOWN:
		return eMoveBackward;
	case VK_PRIOR:
		return eMoveUp;        /// page up
	case VK_NEXT:
		return eMoveDown;      /// page down

	case 'A':
		return eStrafeLeft;
	case 'D':
		return eStrafeRight;
	case 'W':
		return eMoveForward;
	case 'S':
		return eMoveBackward;
	case 'Q':
		return eMoveDown;
	case 'E':
		return eMoveUp;

	case VK_NUMPAD4:
		return eStrafeLeft;
	case VK_NUMPAD6:
		return eStrafeRight;
	case VK_NUMPAD8:
		return eMoveForward;
	case VK_NUMPAD2:
		return eMoveBackward;
	case VK_NUMPAD9:
		return eMoveUp;
	case VK_NUMPAD3:
		return eMoveDown;
	case VK_HOME:
		return eReset;
	}

	return eUnknown;
}
