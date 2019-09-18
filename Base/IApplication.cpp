#include "IApplication.h"
#include "AssetManager.h"
#include <ShellScalingApi.h>

IApplication * IApplication::s_This = nullptr;

::LRESULT IApplication::MessageProcFunc(::HWND hWnd, uint32_t msg, ::WPARAM wParam, ::LPARAM lParam)
{
	s_This->HandleWindowMessage(msg, wParam, lParam);

	s_This->HandleInput(msg, wParam, lParam);

	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

void IApplication::MakeWindow(const std::string &title, uint32_t width, uint32_t height, uint32_t extraWindowStyle)
{
	::HINSTANCE hInst = ::GetModuleHandle(nullptr);
	assert(hInst);

	std::wstring wTitle(title.begin(), title.end());
	if (wTitle.length() == 0U)
	{
		wTitle = L"Application_NoName";
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
	wndClassEx.lpfnWndProc = MessageProcFunc;
	wndClassEx.lpszClassName = wTitle.c_str();
	wndClassEx.lpszMenuName = nullptr;
	wndClassEx.style = CS_HREDRAW | CS_VREDRAW;

	Verify(::RegisterClassEx(&wndClassEx) != 0);

	::RECT rect{ 0, 0, (long)width, (long)height };
	Verify(::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false) != 0);

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

	m_WindowSize.first = width;
	m_WindowSize.second = height;

	::ShowWindow(m_hWnd, SW_SHOWDEFAULT);
	Verify(::UpdateWindow(m_hWnd) != 0);
}

void IApplication::HandleWindowMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM /*lParam*/)
{
	switch (msg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_bActive = false;
			m_Timer.Stop();
		}
		else
		{
			m_bActive = true;
			m_Timer.Start();
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
		m_Timer.Stop();
		m_bNeedResize = true;
		break;
	case WM_EXITSIZEMOVE:
		m_Timer.Start();
		m_bActive = true;
		break;
	case WM_DESTROY:
		m_bActive = false;
		m_Timer.Stop();
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

void IApplication::UpdateWindow()
{
	if (!m_bNeedResize)
	{
		return;
	}

	::RECT rect = {};
	Verify(::GetClientRect(m_hWnd, &rect) != 0);

	m_WindowSize.first = std::max<uint32_t>((rect.right - rect.left), 32U);
	m_WindowSize.second = std::max<uint32_t>((rect.bottom - rect.top), 32U);

	ResizeWindow(m_WindowSize.first, m_WindowSize.second);

	m_bNeedResize = false;
}

void IApplication::Initialize(const std::string &title, uint32_t width, uint32_t height, bool bFullScreen, uint32_t extraWindowStyle)
{
	///::GetDpiForMonitor();
#if _WIN32_WINNT >= _WIN32_WINNT_WIN10
	Verify(::SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE) == S_OK);
#endif

	s_This = this;

	m_bFullScreen = bFullScreen;

	MakeWindow(title, width, height, extraWindowStyle);

	AssetManager::Instance().Initialize();
}

void IApplication::UpdateFPS()
{
	float totalTime = m_Timer.GetTotalTime();
	++m_FrameCount;

	float elapsedTime = totalTime - m_LastUpdateTime;
	if (elapsedTime > 1.0f)
	{
		m_FPS = m_FrameCount / elapsedTime;
		m_LastUpdateTime = totalTime;
		m_FrameCount = 0U;
	}
}

void IApplication::Loop()
{
	::MSG msg = { 0 };

	m_Timer.Reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			m_Timer.Tick();

			if (m_bActive)
			{
				UpdateWindow();

				RenterToWindow();

				UpdateFPS();
			}
			else
			{
				::Sleep(100U);
			}
		}
	}
}