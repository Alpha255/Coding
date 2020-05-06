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
			m_Message.State = eWindowState::eResized;
		}
		else if (SC_RESTORE == wParam)
		{
			m_Message.State = eWindowState::eRestore;
		}
		break;
	case WM_SIZING:
	case WM_ENTERSIZEMOVE:
		m_Message.State = eWindowState::eResizing;
		break;
	case WM_EXITSIZEMOVE:
		m_Message.State = eWindowState::eResized;
		break;
	case WM_DESTROY:
		m_Message.State = eWindowState::eDestroy;
		::PostQuitMessage(0);
		break;
	case WM_NCLBUTTONDBLCLK:
		m_Message.State = eWindowState::eResized;
		break;
	case WM_GETMINMAXINFO:
	{
		::LPMINMAXINFO lpMinMaxInfo = (::LPMINMAXINFO)lParam;
		lpMinMaxInfo->ptMinTrackSize = { (long32_t)m_MinSize.x, (long32_t)m_MinSize.y };
		break;
	}
	case WM_LBUTTONDOWN:
		m_Message.Mouse.Left.KeyDown = true;
		m_Message.Mouse.Pos = Vec2(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_LBUTTONUP:
		m_Message.Mouse.Left.KeyDown = false;
		break;
	case WM_LBUTTONDBLCLK:
		m_Message.Mouse.Left.KeyDown = true;
		m_Message.Mouse.Left.DoubleClick = true;
		break;
	case WM_RBUTTONDOWN:
		m_Message.Mouse.Right.KeyDown = true;
		m_Message.Mouse.Pos = Vec2(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_RBUTTONUP:
		m_Message.Mouse.Right.KeyDown = false;
		break;
	case WM_RBUTTONDBLCLK:
		m_Message.Mouse.Right.KeyDown = true;
		m_Message.Mouse.Right.DoubleClick = true;
		break;
	case WM_MBUTTONDOWN:
		m_Message.Mouse.Middle.KeyDown = true;
		break;
	case WM_MBUTTONUP:
		m_Message.Mouse.Middle.KeyDown = false;
		break;
	case WM_MBUTTONDBLCLK:
		m_Message.Mouse.Middle.KeyDown = true;
		m_Message.Mouse.Middle.DoubleClick = true;
		break;
	case WM_MOUSEMOVE:
		m_Message.Mouse.Pos = Vec2(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_MOUSEWHEEL:
		m_Message.Mouse.WheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:
			m_Message.Key = eKeyboardKey::eKey_Left;
			break;
		case VK_RIGHT:
			m_Message.Key = eKeyboardKey::eKey_Right;
			break;
		case VK_UP:
			m_Message.Key = eKeyboardKey::eKey_Up;
			break;
		case VK_DOWN:
			m_Message.Key = eKeyboardKey::eKey_Down;
			break;
		case VK_HOME:
			m_Message.Key = eKeyboardKey::eKey_Home;
			break;
		case VK_CONTROL:
			m_Message.Key = eKeyboardKey::eKey_Ctrl;
			break;
		case VK_SHIFT:
			m_Message.Key = eKeyboardKey::eKey_Shift;
			break;
		case VK_PRIOR:
			m_Message.Key = eKeyboardKey::eKey_PageUp;
			break;
		case VK_NEXT:
			m_Message.Key = eKeyboardKey::eKey_PageDown;
			break;
		case VK_F1:
			m_Message.Key = eKeyboardKey::eKey_F1;
			break;
		default:
			m_Message.Key = eKeyboardKey::eKey_Other;
			break;
		}
		break;
	case WM_KEYUP:
		m_Message.Key = eKeyboardKey::eKey_None;
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
