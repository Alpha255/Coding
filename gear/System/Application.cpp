#include "application.h"
#include "gear/System/Resource.h"

namespaceStart(gear)

::LRESULT application::messageProc(::HWND hWnd, uint32_t msg, ::WPARAM wParam, ::LPARAM lParam)
{
	if (msg == WM_CREATE)
	{
		::CREATESTRUCTA *pCreateStruct = reinterpret_cast<::CREATESTRUCTA *>(lParam);
		verifyWin(::SetWindowLongPtrA(hWnd, 0, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams)) == 0);
	}

	application *pApp = reinterpret_cast<application *>(::GetWindowLongPtrA(hWnd, 0));
	if (pApp)
	{
		pApp->handleMessage(msg, wParam, lParam);
	}

	return ::DefWindowProcA(hWnd, msg, wParam, lParam);
}

void application::makeWindow(const std::string &title, uint32_t width, uint32_t height, uint32_t extraWindowStyle)
{
	::HINSTANCE hInstance = ::GetModuleHandleW(nullptr);
	verifyWin(hInstance);

	::HICON hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(m_IconID));
	verifyWin(hIcon);
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
	verifyWin(::RegisterClassExA(&wndClassEx) != 0);

	::RECT rect{ 0l, 0l, (long32_t)width, (long32_t)height };
	verifyWin(::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false) != 0);

	m_hWnd = ::CreateWindowA(
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
	verifyWin(m_hWnd);

	m_WindowSize = vec2((float32_t)width, (float32_t)height);
	::ShowWindow(m_hWnd, SW_SHOWDEFAULT);
	verifyWin(::UpdateWindow(m_hWnd) != 0);
}

void application::handleMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam)
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
	case WM_GETMINMAXINFO:
	{
		::LPMINMAXINFO lpMinMaxInfo = (::LPMINMAXINFO)lParam;
		lpMinMaxInfo->ptMinTrackSize = { 640l, 480l };
	}
		break;
	}

	handleInput(msg, wParam, lParam);

	m_Camera.handleMessage(msg, wParam, lParam);
}

void application::updateWindow()
{
	if (!m_bNeedResize)
	{
		return;
	}

	::RECT rect = {};
	verifyWin(::GetClientRect(m_hWnd, &rect) != 0);

	m_WindowSize = { (float32_t)(rect.right - rect.left), (float32_t)(rect.bottom - rect.top) };

	resizeWindow();

	m_bNeedResize = false;
}

void application::initialize(const std::string &title, uint32_t width, uint32_t height, bool8_t bFullScreen, uint32_t extraWindowStyle)
{
	m_IconID = IconVulkan;

	m_bFullScreen = bFullScreen;

	makeWindow(title, width, height, extraWindowStyle);

	assetBucket::instance().initialize();

	postInitialize();
}

void application::updateFPS()
{
	float32_t totalTime = m_Timer.getTotalTime();
	++m_FrameCount;

	float32_t elapsedTime = totalTime - m_LastUpdateTime;
	if (elapsedTime > 1.0f)
	{
		m_FPS = m_FrameCount / elapsedTime;
		m_LastUpdateTime = totalTime;
		m_FrameCount = 0U;
	}
}

void application::loop()
{
	::MSG msg = {};

	m_Timer.reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessageW(&msg, nullptr, 0u, 0u, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
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
				::Sleep(100u);
			}
		}
	}
}

namespaceEnd(gear)
