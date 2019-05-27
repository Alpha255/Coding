#pragma once

#include "Base.h"

class DXUTCamera
{
public:
	DXUTCamera();
	~DXUTCamera() = default;
protected:
	enum eKeyAction
	{
		eUnknown,
		eReset,
		eStrafeLeft,
		eStrafeRight,
		eMoveForward,
		eMoveBackward,
		eMoveUp,
		eMoveDown,
		eControlDown,
		eMaxKeys
	};

	struct MouseAction
	{
		bool LButtonDown = false;
		bool MButtonDown = false;
		bool RButtonDown = false;
		int32_t WheelDelta = 0;
		int32_t ButtonMask = 0;
		std::pair<int32_t, int32_t> LastPosition = {};
	};

	struct KeyboardAction
	{
		eKeyAction Action = eMaxKeys;
		byte Keys[eMaxKeys] = {};
		uint32_t KeysDown = 0;
	};

	eKeyAction GetKeyAction(WPARAM wParam);
private:
	MouseAction m_MouseAction;
};
