#include "Gear/gear.h"
#include "Gear/Window.h"

namespaceStart(Gear)

static ::LRESULT messageProc(::HWND hWnd, uint32_t message, ::WPARAM wParam, ::LPARAM lParam)
{
	if (message == WM_CREATE)
	{
		::CREATESTRUCTA *createStruct = reinterpret_cast<::CREATESTRUCTA *>(lParam);
		verify_Log(::SetWindowLongPtrA(hWnd, 0, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams)) == 0);
	}

	Window* window = reinterpret_cast<Window*>(::GetWindowLongPtrA(hWnd, 0));
	if (window)
	{
		window->processMessage(message, wParam, lParam);
	}

	return ::DefWindowProcA(hWnd, message, wParam, lParam);
}

Window::Window(const std::string& title, uint32_t width, uint32_t height, uint16_t iconID)
	: m_Width(width)
	, m_Height(height)
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

	uint32_t extraWindowStyle = 0u;
	::HWND windowHandle = ::CreateWindowA(
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
		static_cast<void*>(this));
	verify_Log(windowHandle);

	::ShowWindow(windowHandle, SW_SHOWDEFAULT);
	verify_Log(::UpdateWindow(windowHandle) != 0);

	m_Handle = reinterpret_cast<uint64_t>(windowHandle);
}

void Window::processMessage(uint32_t message, uint64_t wParam, int64_t lParam)
{
	switch (message)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_Message.State = eWindowState::eInactive;
		}
		else
		{
			m_Message.State = eWindowState::eActive;
		}
		break;
	case WM_SYSCOMMAND:
		if (SC_MAXIMIZE == wParam)
		{
		}
		else if (SC_RESTORE == wParam)
		{
		}
		break;
	case WM_SIZING:
	case WM_ENTERSIZEMOVE:
		break;
	case WM_EXITSIZEMOVE:
		break;
	case WM_DESTROY:
		m_Message.State = eWindowState::eDestroy;
		::PostQuitMessage(0);
		break;
	case WM_NCLBUTTONDBLCLK:
		break;
	case WM_GETMINMAXINFO:
	{
		::LPMINMAXINFO lpMinMaxInfo = (::LPMINMAXINFO)lParam;
		lpMinMaxInfo->ptMinTrackSize = { (long32_t)m_MinSize.x, (long32_t)m_MinSize.y };
		break;
	}
	case WM_LBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_LBUTTONDBLCLK:
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_RBUTTONUP:
		break;
	case WM_RBUTTONDBLCLK:
		break;
	case WM_MBUTTONDOWN:
		break;
	case WM_MBUTTONUP:
		break;
	case WM_MBUTTONDBLCLK:
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_MOUSEWHEEL:
		break;
	}
}

void Window::update()
{
	::MSG message{};
	if (::PeekMessageW(&message, nullptr, 0u, 0u, PM_REMOVE))
	{
		::TranslateMessage(&message);
		::DispatchMessageW(&message);
	}
}

namespaceEnd(Gear)
