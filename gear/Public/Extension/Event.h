#pragma once

#include "Gear/Public/Independent/Singleton.h"
#include "Gear/Public/Independent/Math/Vector2.h"

namespaceStart(gear)

enum class eKeyboardEvent
{
	eKeyboardEvent_None,
	eKeyDown,
	eKeyUp,
	eKeyboardEvent_MaxEnum
};

enum class eMouseEvent
{
	eMouseEvent_None,
	eLeftUp,
	eLeftClick,
	eLeftDoubleClick,
	eRightUp,
	eRightClick,
	eRightDoubleClick,
	eMiddleUp,
	eMiddleClick,
	eMiddleDoubleClick,
	eMove,
	eWheel,
	eMouseEvent_MaxEnum
};

enum class eAppEvent
{
	eAppEvent_None,
	eActive,
	eInactive,
	eDoubleClickNonClientArea,
	eMaximize,
	eMinimize,
	eQuit,
	eRestore,
	eResizing,
	eSetWindowSizeLimitations,
	eAppMessage_MaxEnum
};

enum class eKeyboardKey
{
	eKeyboardKey_None,
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
	eKeyboardKey_MaxEnum
};

class eventHandler : public singleton<eventHandler>
{
	singletonDeclare(eventHandler);
public:
	void processEvent();
	bool8_t dispatchEvent();

	inline void pushEvent(const byte *pEventData, size_t size)
	{
		m_EventData.reset(new byte[size]());
		assert(m_EventData);

		memcpy(m_EventData.get(), pEventData, size);

		processEvent();
	}

	inline eMouseEvent getMouseEvent() const
	{
		return m_MouseEvent;
	}

	inline eKeyboardEvent getKeyboardEvent() const
	{
		return m_KeyboardEvent;
	}

	inline eAppEvent getAppEvent() const
	{
		return m_AppEvent;
	}

	inline eKeyboardKey getKeyboardKey() const
	{
		return m_KeyboardKey;
	}

	inline float32_t getMouseWheelDelta() const
	{
		return m_MouseWheelDelta;
	}

	inline math::vec2 getMousePosition() const
	{
		return m_MousePosition;
	}

	inline void setWindowSizeLimitations(math::vec2 limitations)
	{
		m_WindowSizeLimitations = limitations;
	}
protected:
private:
	eMouseEvent m_MouseEvent = eMouseEvent::eMouseEvent_None;
	eKeyboardEvent m_KeyboardEvent = eKeyboardEvent::eKeyboardEvent_None;
	eAppEvent m_AppEvent = eAppEvent::eAppEvent_None;
	eKeyboardKey m_KeyboardKey = eKeyboardKey::eKeyboardKey_None;
	float32_t m_MouseWheelDelta = 0.0f;
	math::vec2 m_MousePosition;
	math::vec2 m_WindowSizeLimitations;
	std::unique_ptr<byte> m_EventData = nullptr;
};

namespaceEnd(gear)
