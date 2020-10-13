#include "Gear/gear.h"
#include "Gear/Window.h"
#include "Applications/Resource.h"

NAMESPACE_START(Gear)

#define MINIMAL_WINDOW_SIZE 32

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

Window::Window(uint64_t instance, const std::string& title, const Math::Vec2& size, const Math::Vec2& minSize)
	: m_Size(size)
	, m_MinSize(minSize)
{
	m_MinSize = Math::max(m_MinSize, Math::Vec2(MINIMAL_WINDOW_SIZE, MINIMAL_WINDOW_SIZE));
	m_Size = Math::max(m_Size, m_MinSize);

	assert(instance);
	::HINSTANCE hInstance = reinterpret_cast<::HINSTANCE>(instance);

	::HICON icon = ::LoadIcon(hInstance, MAKEINTRESOURCE(ICON_DIRECTX));
	VERIFY_SYSTEM(icon);
	::WNDCLASSEXA wndClassEx
	{
		sizeof(::WNDCLASSEXA),
		CS_HREDRAW | CS_VREDRAW,
		messageProc,
		0,
		sizeof(void *),
		hInstance,
		icon,
		::LoadCursor(0, IDC_ARROW),
		(::HBRUSH)::GetStockObject(BLACK_BRUSH),
		nullptr,
		title.c_str(),
		icon
	};
	VERIFY_SYSTEM(::RegisterClassExA(&wndClassEx) != 0);

	::RECT rect
	{ 
		0l, 
		0l, 
		static_cast<long32_t>(m_Size.x), 
		static_cast<long32_t>(m_Size.y) 
	};
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
		hInstance,
		static_cast<void*>(this));
	VERIFY_SYSTEM(windowHandle);

	::ShowWindow(windowHandle, SW_SHOWDEFAULT);
	VERIFY_SYSTEM(::UpdateWindow(windowHandle) != 0);

	m_Handle = reinterpret_cast<uint64_t>(windowHandle);
}

void Window::updateSize()
{
	if (m_Handle)
	{
		::RECT rect{};
		VERIFY_SYSTEM(::GetClientRect(reinterpret_cast<::HWND>(m_Handle), &rect) != 0);
		m_Size.x = static_cast<float32_t>(rect.right - rect.left);
		m_Size.y = static_cast<float32_t>(rect.bottom - rect.top);
	}
}

void Window::processMessage(uint32_t message, size_t wParam, intptr_t lParam)
{
	switch (message)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_Active = false;
		}
		else
		{
			m_Active = true;
		}
		break;
	case WM_SIZE:
		if (m_State == EState::Minimized && SIZE_RESTORED == wParam)
		{
			m_State = EState::Restored;
			m_Active = true;
		}
		else if (SIZE_MINIMIZED == wParam)
		{
			m_State = EState::Minimized;
			m_Active = false;
		}
		else if (SIZE_MAXIMIZED == wParam)
		{
			m_State = EState::Maximized;
		}
		updateSize();
		break;
	case WM_QUIT:
	case WM_DESTROY:
		m_State = EState::Destroyed;
		::PostQuitMessage(0);
		break;
	case WM_NCLBUTTONDBLCLK:
		m_State = m_State == EState::Maximized ? EState::Restored : EState::Maximized;
		updateSize();
		break;
	case WM_LBUTTONDOWN:
		m_InputState.MouseState = InputState::EMouseState::LButtonDown;
		break;
	case WM_LBUTTONUP:
		m_InputState.MouseState = InputState::EMouseState::LButtonUp;
		break;
	case WM_LBUTTONDBLCLK:
		m_InputState.MouseState = InputState::EMouseState::LButtonDoubleClick;
		break;
	case WM_RBUTTONDOWN:
		m_InputState.MouseState = InputState::EMouseState::RButtonDown;
		break;
	case WM_RBUTTONUP:
		m_InputState.MouseState = InputState::EMouseState::RButtonUp;
		break;
	case WM_RBUTTONDBLCLK:
		m_InputState.MouseState = InputState::EMouseState::RButtonDoubleClick;
		break;
	case WM_MBUTTONDOWN:
		m_InputState.MouseState = InputState::EMouseState::MButtonDown;
		break;
	case WM_MBUTTONUP:
		m_InputState.MouseState = InputState::EMouseState::MButtonUp;
		break;
	case WM_MBUTTONDBLCLK:
		m_InputState.MouseState = InputState::EMouseState::MButtonDoubleClick;
		break;
	case WM_MOUSEMOVE:
		m_InputState.MouseState = InputState::EMouseState::Move;
		m_InputState.MousePosition.x = static_cast<float32_t>(GET_X_LPARAM(lParam));
		m_InputState.MousePosition.y = static_cast<float32_t>(GET_Y_LPARAM(lParam));
		break;
	case WM_MOUSEWHEEL:
		m_InputState.MouseState = InputState::EMouseState::Wheel;
		m_InputState.MouseWheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? 1 : -1;
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'W':        m_InputState.KeyboardKey = InputState::EKeyboardKey::W;        break;
		case 'A':        m_InputState.KeyboardKey = InputState::EKeyboardKey::A;        break;
		case 'S':        m_InputState.KeyboardKey = InputState::EKeyboardKey::S;        break;
		case 'D':        m_InputState.KeyboardKey = InputState::EKeyboardKey::D;        break;
		case 'Q':        m_InputState.KeyboardKey = InputState::EKeyboardKey::Q;        break;
		case 'E':        m_InputState.KeyboardKey = InputState::EKeyboardKey::E;        break;
		case VK_LEFT:    m_InputState.KeyboardKey = InputState::EKeyboardKey::Left;     break;
		case VK_RIGHT:   m_InputState.KeyboardKey = InputState::EKeyboardKey::Right;    break;
		case VK_UP:      m_InputState.KeyboardKey = InputState::EKeyboardKey::Up;       break;
		case VK_DOWN:    m_InputState.KeyboardKey = InputState::EKeyboardKey::Down;     break;
		case VK_HOME:    m_InputState.KeyboardKey = InputState::EKeyboardKey::Home;     break;
		case VK_CONTROL: m_InputState.KeyboardKey = InputState::EKeyboardKey::Ctrl;     break;
		case VK_MENU:    m_InputState.KeyboardKey = InputState::EKeyboardKey::Other;    break;
		case VK_SHIFT:   m_InputState.KeyboardKey = InputState::EKeyboardKey::Shift;    break;
		case VK_PRIOR:   m_InputState.KeyboardKey = InputState::EKeyboardKey::PageUp;   break;
		case VK_NEXT:    m_InputState.KeyboardKey = InputState::EKeyboardKey::PageDown; break;
		case VK_F1:      m_InputState.KeyboardKey = InputState::EKeyboardKey::F1;       break;
		default:         m_InputState.KeyboardKey = InputState::EKeyboardKey::Other;    break;
		}
		break;
	case WM_SYSKEYDOWN:
		if (wParam == VK_RETURN && lParam & (1 << 29))
		{
			m_State = m_State == EState::Fullscreen ? EState::Restored : EState::Fullscreen;
			updateSize();
			LOG_INFO("Window size %d x %d", static_cast<uint32_t>(m_Size.x), static_cast<uint32_t>(m_Size.y));
		}
		break;
	case WM_KEYUP:
		m_InputState.KeyboardKey = InputState::EKeyboardKey::None;
		break;
	case WM_GETMINMAXINFO:
	{
		::LPMINMAXINFO lpMinMaxInfo = reinterpret_cast<::LPMINMAXINFO>(lParam);
		lpMinMaxInfo->ptMinTrackSize = 
		{ 
			static_cast<long32_t>(m_MinSize.x), 
			static_cast<long32_t>(m_MinSize.y) 
		};
	}
		break;
	case WM_CHAR:
		m_InputState.InputChar = static_cast<char8_t>(wParam);
		break;
	default:
		m_InputState.reset();
		break;
	}
}

void Window::update()
{
	m_InputState.reset();

	::MSG message{};
	if (::PeekMessageW(&message, nullptr, 0u, 0u, PM_REMOVE))
	{
		::TranslateMessage(&message);
		::DispatchMessageW(&message);
	}
}

NAMESPACE_END(Gear)
