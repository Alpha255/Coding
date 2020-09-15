#pragma once

#include "Gear/Math/Math.h"

NAMESPACE_START(Gear)

class WindowMessager
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

	struct WindowMessage
	{
		EMessage Message;
		EKeyboardKey KeyboardKey;
		int16_t MouseWheelDelta;

		Math::Vec2 MousePosition;
	};

	inline void process(uint32_t message, size_t wParam, intptr_t lParam)
	{
		handle(message, wParam, lParam);

		dispatch();
	}

protected:
	void handle(uint32_t message, size_t wParam, intptr_t lParam);
	void dispatch();
private:
	WindowMessage m_Message;
};

DECLARE_UNIQUE_PTR(Window)
class Window
{
public:
	Window(uint64_t instance, const std::string& title, const Math::Vec2& size, const Math::Vec2& minSize);

	inline const uint32_t width() const
	{
		return static_cast<uint32_t>(m_Size.x);
	}

	inline const uint32_t height() const
	{
		return static_cast<uint32_t>(m_Size.y);
	}

	inline const uint64_t handle() const
	{
		return m_Handle;
	}

	inline void processMessage(uint32_t message, size_t wParam, intptr_t lParam)
	{
		m_Messager.process(message, wParam, lParam);
	}

	void update();
protected:
private:
	Math::Vec2 m_Size;
	Math::Vec2 m_MinSize;
	uint64_t m_Handle = 0u;
	WindowMessager m_Messager;
};

NAMESPACE_END(Gear)
