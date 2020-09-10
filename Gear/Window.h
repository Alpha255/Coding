#pragma once

#include "Gear/Math/Math.h"

NAMESPACE_START(Gear)

enum class EKeyboardKey : uint8_t
{
	None,
	W,
	A,
	S,
	D,
	Q,
	E,
	Left,
	Right,
	Up,
	Down,
	Home,
	Ctrl,
	Alt,
	Shift,
	PageUp,
	PageDown,
	F1,
	Other
};

class WindowEvent
{
public:
	enum class EMessage : uint8_t
	{
		Active,
		Inactive,
		SizeMaximized,
		SizeMinimized,
		SizeRestored,
		EnterSizeMove,
		ExitSizeMove,
		Destroy,
		LButtonDoubleClick_NonclientArea,
		LButtonDown,
		LButtonUp,
		LButtonDoubleClick,
		RButtonDown,
		RButtonUp,
		RButtonDoubleClick,
		MButtonDown,
		MButtonUp,
		MButtonDoubleClick,
		MouseMove,
		MouseWheel,
		KeyDown,
		KeyUp,
		Other
	};

	struct EventParameter
	{
		EMessage Message;

		uint32_t Word;

		struct ParameterValue
		{
			uint32_t High;
			uint32_t Low;
		}Value;
	};

	inline void processMessage(uint32_t message, size_t wParam, intptr_t lParam)
	{
		handle(message, wParam, lParam);

		dispatch();
	}

protected:
	void handle(uint32_t message, size_t wParam, intptr_t lParam);
	void dispatch();
private:
	EventParameter m_Event;
};

class Window
{
public:
	Window(const std::string& title, uint32_t width, uint32_t height);

	void setMinSize(uint32_t width, uint32_t height)
	{
		m_MinSize = Math::Vec2(width * 1.0f, height * 1.0f);
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

	inline void processMessage(uint32_t message, size_t wParam, intptr_t lParam)
	{
		m_EventHandler.processMessage(message, wParam, lParam);
	}

	void update();
protected:
private:
	Math::Vec2 m_MinSize;
	uint32_t m_Width = 0u;
	uint32_t m_Height = 0u;
	uint64_t m_Handle = 0u;
	WindowEvent m_EventHandler;
};

NAMESPACE_END(Gear)
