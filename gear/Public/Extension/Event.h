#pragma once

#include "gear/Public/Independent/Singleton.h"

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
	eLeftClick,
	eLeftDoubleClick,
	eRightClick,
	eMiddleClick,
	eMove,
	eWheel,
	eMouseEvent_MaxEnum
};

enum class eAppEvent
{
	eAppEvent_None,
	eActive,
	eInactive,
	eDestory,
	eDoubleClick_NonclientArea,
	eMaximize,
	eMinimize,
	eClose,
	eRestore,
	eResizing_Start,
	eResizing_End,
	eAppMessage_MaxEnum
};

enum class eKeyboardKey
{
	eKeyboardKey_None,
	eKey_W,
	eKey_A,
	eKey_S,
	eKey_D,
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
	void dispatchEvent();
	void pushEvent(const void *pEventData, size_t size);

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
protected:
private:
	eMouseEvent m_MouseEvent = eMouseEvent::eMouseEvent_None;
	eKeyboardEvent m_KeyboardEvent = eKeyboardEvent::eKeyboardEvent_None;
	eAppEvent m_AppEvent = eAppEvent::eAppEvent_None;
	eKeyboardKey m_KeyboardKey = eKeyboardKey::eKeyboardKey_None;
	float32_t m_MouseWheelDelta = 0.0f;
};

namespaceEnd(gear)
