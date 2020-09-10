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

void WindowEvent::handle(uint32_t message, size_t wParam, intptr_t lParam)
{
	m_Event.Value.High = HIWORD(lParam);
	m_Event.Value.Low = LOWORD(lParam);

	switch (message)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_Event.Message = EMessage::Inactive;
		}
		else
		{
			m_Event.Message = EMessage::Active;
		}
		break;
	case WM_SIZE:
		if (SIZE_MAXIMIZED == wParam)
		{
			m_Event.Message = EMessage::SizeMaximized;
		}
		else if (SIZE_RESTORED == wParam)
		{
			m_Event.Message = EMessage::SizeRestored;
		}
		else if (SIZE_MINIMIZED == wParam)
		{
			m_Event.Message = EMessage::SizeMinimized;
		}
		break;
	case WM_ENTERSIZEMOVE:
		m_Event.Message = EMessage::EnterSizeMove;
		break;
	case WM_EXITSIZEMOVE:
		m_Event.Message = EMessage::ExitSizeMove;
		break;
	case WM_DESTROY:
		m_Event.Message = EMessage::Destroy;
		break;
	case WM_NCLBUTTONDBLCLK:
		m_Event.Message = EMessage::LButtonDoubleClick_NonclientArea;
		break;
	case WM_LBUTTONDOWN:
		m_Event.Message = EMessage::LButtonDown;
		break;
	case WM_LBUTTONUP:
		m_Event.Message = EMessage::LButtonUp;
		break;
	case WM_LBUTTONDBLCLK:
		m_Event.Message = EMessage::LButtonDoubleClick;
		break;
	case WM_RBUTTONDOWN:
		m_Event.Message = EMessage::RButtonDown;
		break;
	case WM_RBUTTONUP:
		m_Event.Message = EMessage::RButtonUp;
		break;
	case WM_RBUTTONDBLCLK:
		m_Event.Message = EMessage::RButtonDoubleClick;
		break;
	case WM_MBUTTONDOWN:
		m_Event.Message = EMessage::MButtonDown;
		break;
	case WM_MBUTTONUP:
		m_Event.Message = EMessage::MButtonUp;
		break;
	case WM_MBUTTONDBLCLK:
		m_Event.Message = EMessage::MButtonDoubleClick;
		break;
	case WM_MOUSEMOVE:
		m_Event.Message = EMessage::MouseMove;
		break;
	case WM_MOUSEWHEEL:
		m_Event.Message = EMessage::MouseWheel;
		m_Event.Word = GET_WHEEL_DELTA_WPARAM(wParam);
		break;
	case WM_KEYDOWN:
		m_Event.Message = EMessage::KeyDown;
		switch (wParam)
		{
		case 'W':        m_Event.Word = static_cast<uint32_t>(EKeyboardKey::W);        break;
		case 'A':        m_Event.Word = static_cast<uint32_t>(EKeyboardKey::A);        break;
		case 'S':        m_Event.Word = static_cast<uint32_t>(EKeyboardKey::S);        break;
		case 'D':        m_Event.Word = static_cast<uint32_t>(EKeyboardKey::D);        break;
		case 'Q':        m_Event.Word = static_cast<uint32_t>(EKeyboardKey::Q);        break;
		case 'E':        m_Event.Word = static_cast<uint32_t>(EKeyboardKey::E);        break;
		case VK_LEFT:    m_Event.Word = static_cast<uint32_t>(EKeyboardKey::Left);     break;
		case VK_RIGHT:   m_Event.Word = static_cast<uint32_t>(EKeyboardKey::Right);    break;
		case VK_UP:      m_Event.Word = static_cast<uint32_t>(EKeyboardKey::Up);       break;
		case VK_DOWN:    m_Event.Word = static_cast<uint32_t>(EKeyboardKey::Down);     break;
		case VK_HOME:    m_Event.Word = static_cast<uint32_t>(EKeyboardKey::Home);     break;
		case VK_CONTROL: m_Event.Word = static_cast<uint32_t>(EKeyboardKey::Ctrl);     break;
		case VK_MENU:    m_Event.Word = static_cast<uint32_t>(EKeyboardKey::Other);    break;
		case VK_SHIFT:   m_Event.Word = static_cast<uint32_t>(EKeyboardKey::Shift);    break;
		case VK_PRIOR:   m_Event.Word = static_cast<uint32_t>(EKeyboardKey::PageUp);   break;
		case VK_NEXT:    m_Event.Word = static_cast<uint32_t>(EKeyboardKey::PageDown); break;
		case VK_F1:      m_Event.Word = static_cast<uint32_t>(EKeyboardKey::F1);       break;
		default:         m_Event.Word = static_cast<uint32_t>(EKeyboardKey::Other);    break;
		}
		break;
	case WM_SYSKEYDOWN:
		break;
	case WM_KEYUP:
		m_Event.Message = EMessage::KeyUp;
		break;
	default:
		m_Event.Message = EMessage::Other;
		break;
	}
}

void WindowEvent::dispatch()
{
	::MSG message{};
	if (::PeekMessageW(&message, nullptr, 0u, 0u, PM_REMOVE))
	{
		::TranslateMessage(&message);
		::DispatchMessageW(&message);
	}
}

NAMESPACE_END(Gear)
