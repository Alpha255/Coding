#include "Gear/gear.h"
#include "Gear/Window.h"
#include "Applications/Resource.h"

NAMESPACE_START(Gear)

static ::LRESULT messageProc(::HWND hWnd, uint32_t message, ::WPARAM wParam, ::LPARAM lParam)
{
	if (message == WM_CREATE)
	{
		::CREATESTRUCTA* createStruct = reinterpret_cast<::CREATESTRUCTA*>(lParam);
		VERIFY_SYSTEM(::SetWindowLongPtrA(hWnd, 0, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams)) == 0);
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
	::HINSTANCE instance = ::GetModuleHandleA(nullptr);
	VERIFY_SYSTEM(instance);

	::HICON icon = ::LoadIcon(instance, MAKEINTRESOURCE(IconNvidia));
	VERIFY_SYSTEM(icon);
	::WNDCLASSEXA wndClassEx
	{
		sizeof(::WNDCLASSEXA),
		CS_HREDRAW | CS_VREDRAW,
		messageProc,
		0,
		sizeof(void *),
		instance,
		icon,
		::LoadCursor(0, IDC_ARROW),
		(::HBRUSH)::GetStockObject(BLACK_BRUSH),
		nullptr,
		title.c_str(),
		icon
	};
	VERIFY_SYSTEM(::RegisterClassExA(&wndClassEx) != 0);

	::RECT rect{ 0l, 0l, (long32_t)width, (long32_t)height };
	VERIFY_SYSTEM(::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false) != 0);

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
		instance,
		static_cast<void*>(this));
	VERIFY_SYSTEM(windowHandle);

	::ShowWindow(windowHandle, SW_SHOWDEFAULT);
	VERIFY_SYSTEM(::UpdateWindow(windowHandle) != 0);

	m_Handle = reinterpret_cast<uint64_t>(windowHandle);
}

void WindowMessager::handle(uint32_t message, size_t wParam, intptr_t lParam)
{
	m_Message.MousePosition.x = static_cast<float32_t>(GET_X_LPARAM(lParam));
	m_Message.MousePosition.y = static_cast<float32_t>(GET_Y_LPARAM(lParam));

	switch (message)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_Message.Message = EMessage::Inactive;
		}
		else
		{
			m_Message.Message = EMessage::Active;
		}
		break;
	case WM_SIZE:
		if (SIZE_MAXIMIZED == wParam)
		{
			m_Message.Message = EMessage::SizeMaximized;
		}
		else if (SIZE_RESTORED == wParam)
		{
			m_Message.Message = EMessage::SizeRestored;
		}
		else if (SIZE_MINIMIZED == wParam)
		{
			m_Message.Message = EMessage::SizeMinimized;
		}
		break;
	case WM_ENTERSIZEMOVE:
		m_Message.Message = EMessage::EnterSizeMove;
		break;
	case WM_EXITSIZEMOVE:
		m_Message.Message = EMessage::ExitSizeMove;
		break;
	case WM_DESTROY:
		m_Message.Message = EMessage::Destroy;
		break;
	case WM_NCLBUTTONDBLCLK:
		m_Message.Message = EMessage::LButtonDoubleClick_NonclientArea;
		break;
	case WM_LBUTTONDOWN:
		m_Message.Message = EMessage::LButtonDown;
		break;
	case WM_LBUTTONUP:
		m_Message.Message = EMessage::LButtonUp;
		break;
	case WM_LBUTTONDBLCLK:
		m_Message.Message = EMessage::LButtonDoubleClick;
		break;
	case WM_RBUTTONDOWN:
		m_Message.Message = EMessage::RButtonDown;
		break;
	case WM_RBUTTONUP:
		m_Message.Message = EMessage::RButtonUp;
		break;
	case WM_RBUTTONDBLCLK:
		m_Message.Message = EMessage::RButtonDoubleClick;
		break;
	case WM_MBUTTONDOWN:
		m_Message.Message = EMessage::MButtonDown;
		break;
	case WM_MBUTTONUP:
		m_Message.Message = EMessage::MButtonUp;
		break;
	case WM_MBUTTONDBLCLK:
		m_Message.Message = EMessage::MButtonDoubleClick;
		break;
	case WM_MOUSEMOVE:
		m_Message.Message = EMessage::MouseMove;
		break;
	case WM_MOUSEWHEEL:
		m_Message.Message = EMessage::MouseWheel;
		m_Message.MouseWheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		break;
	case WM_KEYDOWN:
		m_Message.Message = EMessage::KeyDown;
		switch (wParam)
		{
		case 'W':        m_Message.KeyboardKey = EKeyboardKey::W;        break;
		case 'A':        m_Message.KeyboardKey = EKeyboardKey::A;        break;
		case 'S':        m_Message.KeyboardKey = EKeyboardKey::S;        break;
		case 'D':        m_Message.KeyboardKey = EKeyboardKey::D;        break;
		case 'Q':        m_Message.KeyboardKey = EKeyboardKey::Q;        break;
		case 'E':        m_Message.KeyboardKey = EKeyboardKey::E;        break;
		case VK_LEFT:    m_Message.KeyboardKey = EKeyboardKey::Left;     break;
		case VK_RIGHT:   m_Message.KeyboardKey = EKeyboardKey::Right;    break;
		case VK_UP:      m_Message.KeyboardKey = EKeyboardKey::Up;       break;
		case VK_DOWN:    m_Message.KeyboardKey = EKeyboardKey::Down;     break;
		case VK_HOME:    m_Message.KeyboardKey = EKeyboardKey::Home;     break;
		case VK_CONTROL: m_Message.KeyboardKey = EKeyboardKey::Ctrl;     break;
		case VK_MENU:    m_Message.KeyboardKey = EKeyboardKey::Other;    break;
		case VK_SHIFT:   m_Message.KeyboardKey = EKeyboardKey::Shift;    break;
		case VK_PRIOR:   m_Message.KeyboardKey = EKeyboardKey::PageUp;   break;
		case VK_NEXT:    m_Message.KeyboardKey = EKeyboardKey::PageDown; break;
		case VK_F1:      m_Message.KeyboardKey = EKeyboardKey::F1;       break;
		default:         m_Message.KeyboardKey = EKeyboardKey::Other;    break;
		}
		break;
	case WM_SYSKEYDOWN:
		break;
	case WM_KEYUP:
		m_Message.Message = EMessage::KeyUp;
		break;
	default:
		m_Message.Message = EMessage::Other;
		break;
	}
}

void WindowMessager::dispatch()
{
	::MSG message{};
	if (::PeekMessageW(&message, nullptr, 0u, 0u, PM_REMOVE))
	{
		::TranslateMessage(&message);
		::DispatchMessageW(&message);
	}
}

NAMESPACE_END(Gear)
