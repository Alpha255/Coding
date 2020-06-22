#include "Gear/gear.h"
#include "Gear/Window.h"
#include "Applications/Resource.h"

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

Window::Window(const std::string& title, uint32_t width, uint32_t height)
	: m_Width(width)
	, m_Height(height)
{
	::HINSTANCE hInstance = ::GetModuleHandleW(nullptr);
	verify_Log(hInstance);

	::HICON hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IconNvidia));
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
	static auto getMousePos = [this](int64_t lParam)
	{
		Vec2 curPos = Vec2((float32_t)LOWORD(lParam), (float32_t)HIWORD(lParam));
		m_Message.Mouse.DeltaPos = curPos - m_Message.Mouse.Pos;
		m_Message.Mouse.Pos = curPos;
	};

	static auto getWindowSize = [this]()
	{
		if (m_Handle)
		{
			::RECT rect{};
			::GetClientRect((::HWND)m_Handle, &rect);
			m_Width = static_cast<uint32_t>(rect.right - rect.left);
			m_Height = static_cast<uint32_t>(rect.bottom - rect.top);
		}
	};

	static bool8_t resizing = false;
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
	case WM_SIZE:
		if (resizing || SIZE_MAXIMIZED == wParam || SIZE_RESTORED == wParam)
		{
			m_Message.State = eWindowState::eResized;
			getWindowSize();
			m_Message.Minimized = false;
		}
		else if (SIZE_MINIMIZED == wParam)
		{
			m_Message.Minimized = true;
		}
		break;
	case WM_ENTERSIZEMOVE:
		resizing = true;
		break;
	case WM_EXITSIZEMOVE:
		resizing = false;
		break;
	case WM_DESTROY:
		m_Message.State = eWindowState::eDestroy;
		::PostQuitMessage(0);
		break;
	case WM_NCLBUTTONDBLCLK:
		m_Message.State = eWindowState::eResized;
		getWindowSize();
		break;
	case WM_GETMINMAXINFO:
	{
		::LPMINMAXINFO lpMinMaxInfo = (::LPMINMAXINFO)lParam;
		lpMinMaxInfo->ptMinTrackSize = { (long32_t)m_MinSize.x, (long32_t)m_MinSize.y };
		break;
	}
	case WM_LBUTTONDOWN:
		m_Message.Mouse.Left.KeyDown = true;
		getMousePos(lParam);
		break;
	case WM_LBUTTONUP:
		m_Message.Mouse.Left.KeyDown = false;
		break;
	case WM_LBUTTONDBLCLK:
		m_Message.Mouse.Left.KeyDown = true;
		m_Message.Mouse.Left.DoubleClick = true;
		getMousePos(lParam);
		break;
	case WM_RBUTTONDOWN:
		m_Message.Mouse.Right.KeyDown = true;
		getMousePos(lParam);
		break;
	case WM_RBUTTONUP:
		m_Message.Mouse.Right.KeyDown = false;
		break;
	case WM_RBUTTONDBLCLK:
		m_Message.Mouse.Right.KeyDown = true;
		m_Message.Mouse.Right.DoubleClick = true;
		getMousePos(lParam);
		break;
	case WM_MBUTTONDOWN:
		m_Message.Mouse.Middle.KeyDown = true;
		getMousePos(lParam);
		break;
	case WM_MBUTTONUP:
		m_Message.Mouse.Middle.KeyDown = false;
		break;
	case WM_MBUTTONDBLCLK:
		m_Message.Mouse.Middle.KeyDown = true;
		m_Message.Mouse.Middle.DoubleClick = true;
		getMousePos(lParam);
		break;
	case WM_MOUSEMOVE:
		getMousePos(lParam);
		break;
	case WM_MOUSEWHEEL:
		m_Message.Mouse.WheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
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
		case 'W':
			m_Message.Key = eKeyboardKey::eKey_W;
			break;
		case 'A':
			m_Message.Key = eKeyboardKey::eKey_A;
			break;
		case 'S':
			m_Message.Key = eKeyboardKey::eKey_S;
			break;
		case 'D':
			m_Message.Key = eKeyboardKey::eKey_D;
			break;
		case 'Q':
			m_Message.Key = eKeyboardKey::eKey_Q;
			break;
		case 'E':
			m_Message.Key = eKeyboardKey::eKey_E;
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
	if (m_Message.State == eWindowState::eResized)
	{
		m_Message.State = eWindowState::eActive;
	}

	::MSG message{};
	if (::PeekMessageW(&message, nullptr, 0u, 0u, PM_REMOVE))
	{
		::TranslateMessage(&message);
		::DispatchMessageW(&message);
	}
}

namespaceEnd(Gear)
