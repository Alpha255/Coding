#pragma once

#include "Gear/Math/Math.h"

NAMESPACE_START(Gear)

struct InputState
{
	enum class EMouseState : uint8_t
	{
		None,
		LButtonDown,
		LButtonUp,
		LButtonDoubleClick,
		RButtonDown,
		RButtonUp,
		RButtonDoubleClick,
		MButtonDown,
		MButtonUp,
		MButtonDoubleClick,
		Move,
		Wheel
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
		Enter,
		Shift,
		PageUp,
		PageDown,
		F1,
		Other
	};

	EMouseState MouseState = EMouseState::None;
	EKeyboardKey KeyboardKey = EKeyboardKey::None;
	char8_t InputChar{};
	int8_t MouseWheelDelta = 0;

	Math::Vec2 MousePosition;
protected:
	friend class Window;
	inline void reset()
	{
		MouseState = EMouseState::None;
		KeyboardKey = EKeyboardKey::None;
		InputChar = char8_t();
		MouseWheelDelta = 0;
	}
};

DECLARE_UNIQUE_PTR(Window)
class Window
{
public:
	enum class EState : uint8_t
	{
		Minimized,
		Maximized,
		Restored,
		Fullscreen,
		Destroyed,
	};

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

	inline const bool8_t isActive() const
	{
		return m_Active;
	}

	inline const EState state() const
	{
		return m_State;
	}

	inline const InputState& inputState() const
	{
		return m_InputState;
	}

	void processMessage(uint32_t message, size_t wParam, intptr_t lParam);

	void update();
protected:
	void updateSize();
private:
	Math::Vec2 m_Size;
	Math::Vec2 m_MinSize;
	bool8_t m_Active = true;
	EState m_State = EState::Restored;
	uint64_t m_Handle = 0u;
	InputState m_InputState{};
};

NAMESPACE_END(Gear)
