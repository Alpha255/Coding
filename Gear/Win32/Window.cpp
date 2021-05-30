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
		assert(createStruct);
		VERIFY_SYSTEM(::SetWindowLongPtrA(hWnd, 0, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams)) == 0);
	}

	if (Window* window = reinterpret_cast<Window*>(::GetWindowLongPtrA(hWnd, 0)))
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
		static_cast<::HBRUSH>(::GetStockObject(BLACK_BRUSH)),
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
		::RECT rect;
		VERIFY_SYSTEM(::GetClientRect(reinterpret_cast<::HWND>(m_Handle), &rect) != 0);
		m_Size.x = static_cast<float32_t>(rect.right - rect.left);
		m_Size.y = static_cast<float32_t>(rect.bottom - rect.top);
	}
}

void Window::processMessage(uint32_t message, size_t wParam, intptr_t lParam)
{
	InputState inputState;

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
		inputState.MouseState = EMouseState::LButtonDown;
		break;
	case WM_LBUTTONUP:
		inputState.MouseState = EMouseState::LButtonUp;
		break;
	case WM_LBUTTONDBLCLK:
		inputState.MouseState = EMouseState::LButtonDBClick;
		break;
	case WM_RBUTTONDOWN:
		inputState.MouseState = EMouseState::RButtonDown;
		break;
	case WM_RBUTTONUP:
		inputState.MouseState = EMouseState::RButtonUp;
		break;
	case WM_RBUTTONDBLCLK:
		inputState.MouseState = EMouseState::RButtonDBClick;
		break;
	case WM_MBUTTONDOWN:
		inputState.MouseState = EMouseState::MButtonDown;
		break;
	case WM_MBUTTONUP:
		inputState.MouseState = EMouseState::MButtonUp;
		break;
	case WM_MBUTTONDBLCLK:
		inputState.MouseState = EMouseState::MButtonDBClick;
		break;
	case WM_MOUSEMOVE:
		inputState.MouseState = EMouseState::Move;
		inputState.MousePosition.x = static_cast<float32_t>(GET_X_LPARAM(lParam));
		inputState.MousePosition.y = static_cast<float32_t>(GET_Y_LPARAM(lParam));
		break;
	case WM_MOUSEWHEEL:
		inputState.MouseState = EMouseState::Wheel;
		inputState.MouseWheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? 1 : -1;
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'W':        inputState.KeyboardKey = EKeyboardKey::W;        break;
		case 'A':        inputState.KeyboardKey = EKeyboardKey::A;        break;
		case 'S':        inputState.KeyboardKey = EKeyboardKey::S;        break;
		case 'D':        inputState.KeyboardKey = EKeyboardKey::D;        break;
		case 'Q':        inputState.KeyboardKey = EKeyboardKey::Q;        break;
		case 'E':        inputState.KeyboardKey = EKeyboardKey::E;        break;
		case VK_LEFT:    inputState.KeyboardKey = EKeyboardKey::Left;     break;
		case VK_RIGHT:   inputState.KeyboardKey = EKeyboardKey::Right;    break;
		case VK_UP:      inputState.KeyboardKey = EKeyboardKey::Up;       break;
		case VK_DOWN:    inputState.KeyboardKey = EKeyboardKey::Down;     break;
		case VK_HOME:    inputState.KeyboardKey = EKeyboardKey::Home;     break;
		case VK_CONTROL: inputState.KeyboardKey = EKeyboardKey::Ctrl;     break;
		case VK_MENU:    inputState.KeyboardKey = EKeyboardKey::Other;    break;
		case VK_SHIFT:   inputState.KeyboardKey = EKeyboardKey::Shift;    break;
		case VK_PRIOR:   inputState.KeyboardKey = EKeyboardKey::PageUp;   break;
		case VK_NEXT:    inputState.KeyboardKey = EKeyboardKey::PageDown; break;
		case VK_F1:      inputState.KeyboardKey = EKeyboardKey::F1;       break;
		default:         inputState.KeyboardKey = EKeyboardKey::Other;    break;
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
		inputState.KeyboardKey = EKeyboardKey::None;
		break;
	case WM_GETMINMAXINFO:
	{
		::LPMINMAXINFO lpMinMaxInfo = reinterpret_cast<::LPMINMAXINFO>(lParam);
		assert(lpMinMaxInfo);
		lpMinMaxInfo->ptMinTrackSize = 
		{ 
			static_cast<long32_t>(m_MinSize.x), 
			static_cast<long32_t>(m_MinSize.y) 
		};
	}
		break;
	case WM_CHAR:
		inputState.InputChar = static_cast<char8_t>(wParam);
		break;
	}

	/// RAWINPUT

	if (m_Active && inputState.isValid())
	{
		InputTrigger::instance().process(inputState);
	}
}

void Window::update()
{
	::MSG msg;
	if (::PeekMessageW(&msg, nullptr, 0u, 0u, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessageW(&msg);
	}
}

InputTrigger& InputTrigger::instance()
{
	static InputTrigger s_Instance;
	return s_Instance;
}

NAMESPACE_END(Gear)
