#pragma once

#include "Gear/Math/Math.h"

NAMESPACE_START(Gear)

enum class EMouseState : uint8_t
{
	None,
	LButtonDown,
	LButtonUp,
	LButtonDBClick,
	RButtonDown,
	RButtonUp,
	RButtonDBClick,
	MButtonDown,
	MButtonUp,
	MButtonDBClick,
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

struct InputState
{
	EMouseState MouseState = EMouseState::None;
	EKeyboardKey KeyboardKey = EKeyboardKey::None;
	char8_t InputChar = 0;
	int8_t MouseWheelDelta = 0;

	Math::Vec2 MousePosition;

	bool8_t isValid() const
	{
		return MouseState != EMouseState::None || KeyboardKey != EKeyboardKey::None || InputChar != 0;
	}
};

class InputHandler
{
public:
	virtual void processInput(const InputState& state) = 0;

	virtual bool8_t hasFocus() const
	{
		return false;
	}
};

class InputTrigger : public NoneCopyable
{
public:
	static InputTrigger& instance();

	void registerHandler(InputHandler* handler)
	{
		assert(handler);
		m_Handlers.emplace_back(handler);
	}

	void process(const InputState& state)
	{
		InputHandler* focus = nullptr;
		for (auto handler : m_Handlers)
		{
			assert(handler);
			if (handler->hasFocus())
			{
				focus = handler;
				break;
			}
		}

		for (auto handler : m_Handlers)
		{
			assert(handler);
			if (focus && handler == focus)
			{
				handler->processInput(state);
				break;
			}
			if (!focus)
			{
				handler->processInput(state);
			}
		}
	}
private:
	std::vector<InputHandler*> m_Handlers;
};

NAMESPACE_END(Gear)