#include "gear/Public/Extension/Event.h"

#if defined(Platform_Win32)

namespaceStart(gear)

void eventHandler::processEvent()
{
	if (!m_EventData)
	{
		return;
	}

	struct win32Event
	{
		uint32_t Msg;
		::WPARAM WParam;
		::LPARAM LParam;
	};

	win32Event event{};
	memcpy(&event, m_EventData.get(), sizeof(win32Event));

	switch (event.Msg)
	{
	case WM_ACTIVATE:
		if (LOWORD(event.WParam) == WA_INACTIVE)
		{
			m_AppEvent = eAppEvent::eInactive;
		}
		else
		{
			m_AppEvent = eAppEvent::eActive;
		}
		break;
	case WM_SYSCOMMAND:
		if (SC_MAXIMIZE == event.WParam)
		{
			m_AppEvent = eAppEvent::eMaximize;
		}
		else if (SC_RESTORE == event.WParam)
		{
			m_AppEvent = eAppEvent::eRestore;
		}
		break;
	case WM_SIZING:
		m_AppEvent = eAppEvent::eResizing;
		break;
	case WM_DESTROY:
		m_AppEvent = eAppEvent::eQuit;
		::PostQuitMessage(0);
		break;
	case WM_NCLBUTTONDBLCLK:
		if (event.WParam == HTCAPTION)
		{
			m_AppEvent = eAppEvent::eDoubleClickNonClientArea;
		}
		break;
	case WM_GETMINMAXINFO:
	{
		::LPMINMAXINFO lpMinMaxInfo = (::LPMINMAXINFO)event.LParam;
		lpMinMaxInfo->ptMinTrackSize = { (long32_t)m_WindowSizeLimitations.x, (long32_t)m_WindowSizeLimitations.y };
		break;
	}
	case WM_LBUTTONDOWN:
		m_MouseEvent = eMouseEvent::eLeftClick;
		break;
	case WM_LBUTTONUP:
		m_MouseEvent = eMouseEvent::eLeftUp;
		break;
	case WM_LBUTTONDBLCLK:
		m_MouseEvent = eMouseEvent::eLeftDoubleClick;
		break;
	case WM_RBUTTONDOWN:
		m_MouseEvent = eMouseEvent::eRightClick;
		break;
	case WM_RBUTTONUP:
		m_MouseEvent = eMouseEvent::eRightUp;
		break;
	case WM_RBUTTONDBLCLK:
		m_MouseEvent = eMouseEvent::eRightDoubleClick;
		break;
	case WM_MBUTTONDOWN:
		m_MouseEvent = eMouseEvent::eMiddleClick;
		break;
	case WM_MBUTTONUP:
		m_MouseEvent = eMouseEvent::eMiddleUp;
		break;
	case WM_MBUTTONDBLCLK:
		m_MouseEvent = eMouseEvent::eMiddleDoubleClick;
		break;
	case WM_MOUSEMOVE:
		m_MouseEvent = eMouseEvent::eMove;
		m_MousePosition.x = (float32_t)((int16_t)(event.LParam));
		m_MousePosition.y = (float32_t)((int16_t)(event.LParam >> 16ull));
		break;
	}
}

bool8_t eventHandler::dispatchEvent()
{
	::MSG msg = {};
	if (PeekMessageW(&msg, nullptr, 0u, 0u, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessageW(&msg);
		return true;
	}

	return false;
}

bool8_t eventHandler::isFocusWindow(uint64_t windowHandle)
{
	return ::GetCapture() == reinterpret_cast<::HWND>(windowHandle);
}

void eventHandler::focusWindow(uint64_t windowHandle)
{
	::SetCapture(reinterpret_cast<::HWND>(windowHandle));
}

void eventHandler::unFocusWindow()
{
	::ReleaseCapture();
}

namespaceEnd(gear)

#endif
