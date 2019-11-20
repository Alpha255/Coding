#include "Gear/Public/Extension/Application.h"
#include "Applications/Resource.h"

#if defined(Platform_Win32)

namespaceStart(gear)

::LRESULT messageProc(::HWND hWnd, uint32_t msg, ::WPARAM wParam, ::LPARAM lParam)
{
	if (msg == WM_CREATE)
	{
		::CREATESTRUCTA *pCreateStruct = reinterpret_cast<::CREATESTRUCTA *>(lParam);
		verify_Log(::SetWindowLongPtrA(hWnd, 0, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams)) == 0);
	}

	application *pApp = reinterpret_cast<application *>(::GetWindowLongPtrA(hWnd, 0));
	if (pApp)
	{
		struct win32Event
		{
			uint32_t Msg;
			::WPARAM WParam;
			::LPARAM LParam;
		};

		win32Event event
		{
			msg,
			wParam,
			lParam
		};
		eventHandler::instance().pushEvent((byte *)&event, sizeof(win32Event));

		pApp->processEvent();
	}

	return ::DefWindowProcA(hWnd, msg, wParam, lParam);
}

void application::makeWindow(const std::string &title, uint32_t width, uint32_t height, uint32_t extraWindowStyle, uint16_t iconID)
{
	::HINSTANCE hInstance = ::GetModuleHandleW(nullptr);
	verify_Log(hInstance);

	::HICON hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(iconID));
	verify_Log(hIcon);
	::WNDCLASSEXA wndClassEx
	{
		sizeof(::WNDCLASSEXA),
		CS_HREDRAW | CS_VREDRAW,
		messageProc,
		0,
		sizeof(void *),
		hInstance,
		hIcon,
		::LoadCursor(0, IDC_ARROW),
		(::HBRUSH)::GetStockObject(BLACK_BRUSH),
		nullptr,
		title.c_str(),
		hIcon
	};
	verify_Log(::RegisterClassExA(&wndClassEx) != 0);

	::RECT rect{ 0l, 0l, (long32_t)width, (long32_t)height };
	verify_Log(::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false) != 0);

	m_WindowHandle = (uint64_t)::CreateWindowA(
		title.c_str(),
		title.c_str(),
		WS_OVERLAPPEDWINDOW ^ extraWindowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		hInstance,
		static_cast<void *>(this));
	verify_Log(m_WindowHandle);

	m_WindowSize = vec2((float32_t)width, (float32_t)height);
	::ShowWindow((::HWND)m_WindowHandle, SW_SHOWDEFAULT);
	verify_Log(::UpdateWindow((::HWND)m_WindowHandle) != 0);
}

namespaceEnd(gear)

#endif
