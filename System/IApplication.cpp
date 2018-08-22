#include "IApplication.h"
#include "Timer.h"

static IApplication* s_Application = nullptr;

::LRESULT WINAPI WndProc(::HWND hWnd, uint32_t msg, ::WPARAM wParam, ::LPARAM lParam)
{
	assert(s_Application);
	return s_Application->MsgProc(hWnd, msg, wParam, lParam);
}

IApplication::IApplication()
	: m_Timer(new Timer())
{
}

void IApplication::MakeWindow(const wchar_t *pTitle, uint32_t width, uint32_t height, uint32_t windowStyle)
{
	::HINSTANCE hInst = ::GetModuleHandle(nullptr);
	assert(hInst && pTitle);

	::WNDCLASSEX wndClassEx;
	memset(&wndClassEx, 0, sizeof(::WNDCLASSEX));
	wndClassEx.cbClsExtra = 0;
	wndClassEx.cbSize = sizeof(::WNDCLASSEX);
	wndClassEx.hbrBackground = (::HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClassEx.hCursor = ::LoadCursor(0, IDC_ARROW);
	wndClassEx.hIcon = ::LoadIcon(hInst, MAKEINTRESOURCE(m_IconID));
	wndClassEx.hIconSm = wndClassEx.hIcon;
	wndClassEx.hInstance = hInst;
	wndClassEx.lpfnWndProc = WndProc;
	wndClassEx.lpszClassName = pTitle;
	wndClassEx.lpszMenuName = nullptr;
	wndClassEx.style = CS_HREDRAW | CS_VREDRAW;

	if (RegisterClassEx(&wndClassEx))
	{
		RECT rect{ 0, 0, (long)width, (long)height };
		::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
		m_hWnd = ::CreateWindow(pTitle, pTitle, WS_OVERLAPPEDWINDOW ^ windowStyle, CW_USEDEFAULT, CW_USEDEFAULT,
			rect.right - rect.left, rect.bottom - rect.top, 0, 0, hInst, nullptr);
		assert(m_hWnd);

		m_Width = width;
		m_Height = height;

		::ShowWindow(m_hWnd, SW_SHOWDEFAULT);
		::UpdateWindow(m_hWnd);
	}
	else
	{
		assert(!"Failed to create window!!!");
	}
}

void IApplication::HandleWindowMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM /*lParam*/)
{
	switch (msg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_bActive = false;
			m_Timer->Stop();
		}
		else
		{
			m_bActive = true;
			m_Timer->Start();
		}
		break;
	case WM_SYSCOMMAND:
		if (SC_MAXIMIZE == wParam || SC_RESTORE == wParam)
		{
			m_bNeedResize = true;
		}
		break;
	case WM_ENTERSIZEMOVE:
		m_bActive = false;
		m_Timer->Stop();
		m_bNeedResize = true;
		break;
	case WM_EXITSIZEMOVE:
		m_Timer->Start();
		m_bActive = true;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	case WM_NCLBUTTONDBLCLK:
		if (wParam == HTCAPTION)
		{
			m_bNeedResize = true;
		}
		break;
	///case WM_GETMINMAXINFO:
		///::MINMAXINFO minmaxInfo;
		///memcpy(&minmaxInfo, (const void *)lParam, sizeof(::MINMAXINFO));
		///break;
	}
}

void IApplication::HandleInput(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		MouseButtonDown(wParam, (int)LOWORD(lParam), (int)HIWORD(lParam));
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		///MouseButtonUp(wParam, (int)LOWORD(lParam), (int)HIWORD(lParam));
		break;
	case WM_MOUSEMOVE:
		MouseMove(wParam, (int)LOWORD(lParam), (int)HIWORD(lParam));
		break;
	case WM_MOUSEWHEEL:
		MouseWheel(wParam);
		break;
	case WM_KEYDOWN:
		///Keyboard(wParam);
		break;
	}
}

void IApplication::UpdateWindow()
{
	if (!m_bNeedResize)
	{
		return;
	}

	::RECT rect;
	::GetClientRect(m_hWnd, &rect);

	m_Width = std::max<uint32_t>((rect.right - rect.left), 32U);
	m_Height = std::max<uint32_t>((rect.bottom - rect.top), 32U);

	ResizeWindow(m_Width, m_Height);

	m_bNeedResize = false;
}

void IApplication::Startup(const wchar_t *pTitle, uint32_t width, uint32_t height, bool bWindowed, uint32_t windowStyle)
{
	s_Application = this;

	m_bWindowed = bWindowed;

	MakeWindow(pTitle, width, height, windowStyle);

	InitRenderer();
}

void IApplication::Running()
{
	::MSG msg = { 0 };

	m_Timer->Reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			m_Timer->Tick();

			if (m_bActive)
			{
				UpdateWindow();

				RenderToWindow();
			}
			else
			{
				::Sleep(100);
			}
		}
	}
}