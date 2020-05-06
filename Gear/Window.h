#pragma once

#include "Gear/Math/Math.h"

namespaceStart(Gear)

enum class eKeyboardKey
{
	eKey_None,
	eKey_W,
	eKey_A,
	eKey_S,
	eKey_D,
	eKey_Q,
	eKey_E,
	eKey_Left,
	eKey_Right,
	eKey_Up,
	eKey_Down,
	eKey_Home,
	eKey_Ctrl,
	eKey_Alt,
	eKey_Shift,
	eKey_PageUp,
	eKey_PageDown,
	eKey_F1,
	eKey_Other,
	eKeyboardKey_MaxEnum
};

struct MouseKey
{
	bool8_t KeyDown = false;
	bool8_t DoubleClick = false;
};

struct MouseMessage
{
	MouseKey Left;
	MouseKey Right;
	MouseKey Middle;

	Math::Vec2 Pos;
	int16_t WheelDelta = 0;
};

enum class eWindowState
{
	eActive,
	eInactive,
	eDestroy,
	eResized,
	eWindowState_MaxEnum
};

struct WindowMessage
{
	eWindowState State = eWindowState::eInactive;

	eKeyboardKey Key = eKeyboardKey::eKey_None;

	MouseMessage Mouse;
};

class Window
{
public:
	Window(const std::string& title, uint32_t width, uint32_t height);

	void setMinSize(uint32_t width, uint32_t height)
	{
		m_MinSize = Math::Vec2(width * 1.0f, height * 1.0f);
	}

	inline const WindowMessage& message() const
	{
		return m_Message;
	}

	inline const uint32_t width() const
	{
		return m_Width;
	}

	inline const uint32_t height() const
	{
		return m_Height;
	}

	inline const uint64_t handle() const
	{
		return m_Handle;
	}

	void processMessage(uint32_t message, uint64_t wParam, int64_t lParam);

	void update();
protected:
private:
	WindowMessage m_Message{};
	Math::Vec2 m_MinSize;
	uint32_t m_Width = 0u;
	uint32_t m_Height = 0u;
	uint64_t m_Handle = 0u;
};
using WindowPtr = std::unique_ptr<Window>;

namespaceEnd(Gear)
