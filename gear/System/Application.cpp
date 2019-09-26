#include "application.h"
///#include <ShellScalingApi.h>

::LRESULT application::messageProc(::HWND hWnd, uint32_t msg, ::WPARAM wParam, ::LPARAM lParam)
{
	///s_This->HandleWindowMessage(msg, wParam, lParam);

	///s_This->HandleInput(msg, wParam, lParam);

	return ::DefWindowProcA(hWnd, msg, wParam, lParam);
}

void application::makeWindow(const std::string &title, uint32_t width, uint32_t height, uint32_t extraWindowStyle)
{
	::HINSTANCE hInst = ::GetModuleHandle(nullptr);
	assert(hInst);

	std::wstring wTitle(title.begin(), title.end());
	if (wTitle.length() == 0U)
	{
		wTitle = L"applicationNoName";
	}

	::WNDCLASSEX wndClassEx = {};
	memset(&wndClassEx, 0, sizeof(::WNDCLASSEX));
	wndClassEx.cbClsExtra = 0;
	wndClassEx.cbSize = sizeof(::WNDCLASSEX);
	wndClassEx.hbrBackground = (::HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClassEx.hCursor = ::LoadCursor(0, IDC_ARROW);
	wndClassEx.hIcon = ::LoadIcon(hInst, MAKEINTRESOURCE(m_IconID));
	wndClassEx.hIconSm = wndClassEx.hIcon;
	wndClassEx.hInstance = hInst;
	wndClassEx.lpfnWndProc = messageProc;
	wndClassEx.lpszClassName = wTitle.c_str();
	wndClassEx.lpszMenuName = nullptr;
	wndClassEx.style = CS_HREDRAW | CS_VREDRAW;

	verifyWin(::RegisterClassEx(&wndClassEx) != 0);

	::RECT rect{ 0, 0, (long32_t)width, (long32_t)height };
	verifyWin(::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false) != 0);

	m_hWnd = ::CreateWindow(
		wTitle.c_str(),
		wTitle.c_str(),
		WS_OVERLAPPEDWINDOW ^ extraWindowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		0,
		0,
		hInst,
		nullptr);
	assert(m_hWnd);

	m_WindowSize.x = (float32_t)width;
	m_WindowSize.y = (float32_t)height;

	::ShowWindow(m_hWnd, SW_SHOWDEFAULT);
	verifyWin(::UpdateWindow(m_hWnd) != 0);
}

void application::handleMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM /*lParam*/)
{
	switch (msg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_bActive = false;
			m_Timer.stop();
		}
		else
		{
			m_bActive = true;
			m_Timer.start();
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
		m_Timer.stop();
		m_bNeedResize = true;
		break;
	case WM_EXITSIZEMOVE:
		m_Timer.start();
		m_bActive = true;
		break;
	case WM_DESTROY:
		m_bActive = false;
		m_Timer.stop();
		::PostQuitMessage(0);
		break;
	case WM_NCLBUTTONDBLCLK:
		if (wParam == HTCAPTION)
		{
			m_bNeedResize = true;
		}
		break;
	}
}

void application::updateWindow()
{
	if (!m_bNeedResize)
	{
		return;
	}

	::RECT rect = {};
	verifyWin(::GetClientRect(m_hWnd, &rect) != 0);

	m_WindowSize.x = (float32_t)std::max<long32_t>((rect.right - rect.left), 32u);
	m_WindowSize.y = (float32_t)std::max<long32_t>((rect.bottom - rect.top), 32u);

	resizeWindow((uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y);

	m_bNeedResize = false;
}

void application::initialize(const std::string &title, uint32_t width, uint32_t height, bool bFullScreen, uint32_t extraWindowStyle)
{
#if defined(_WIN32_WINNT_WIN10)
	/// "DpiAwareness" already enabled in project file
	///::GetDpiForMonitor();
	///Verify(::SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE) == S_OK);
#endif

	m_bFullScreen = bFullScreen;

	makeWindow(title, width, height, extraWindowStyle);
}

void application::updateFPS()
{
	float totalTime = m_Timer.totalTime();
	++m_FrameCount;

	float elapsedTime = totalTime - m_LastUpdateTime;
	if (elapsedTime > 1.0f)
	{
		m_FPS = m_FrameCount / elapsedTime;
		m_LastUpdateTime = totalTime;
		m_FrameCount = 0U;
	}
}

void application::loop()
{
	::MSG msg = { 0 };

	m_Timer.reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			m_Timer.tick();

			if (m_bActive)
			{
				updateWindow();

				renterToWindow();

				updateFPS();
			}
			else
			{
				::Sleep(100U);
			}
		}
	}
}