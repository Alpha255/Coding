#include "IApplication.h"
#include "ImGUI.h"

static IApplication *s_Application = nullptr;

::LRESULT WINAPI WndProc(::HWND hWnd, uint32_t msg, ::WPARAM wParam, ::LPARAM lParam)
{
	assert(s_Application);
	return s_Application->MessageProcFunc(hWnd, msg, wParam, lParam);
}

::LRESULT IApplication::MessageProcFunc(::HWND hWnd, uint32_t msg, ::WPARAM wParam, ::LPARAM lParam)
{
	if (m_bRenderEngineInited && ImGUI::Instance().MessageProcFunc(hWnd, msg, wParam, lParam))
	{
		return 1LL;
	}

	HandleWindowMessage(msg, wParam, lParam);

	if (!m_bRenderEngineInited || !ImGUI::Instance().IsFocus())
	{
		HandleInput(msg, wParam, lParam);
	}

	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

void IApplication::MakeWindow(const wchar_t *pTitle, uint32_t width, uint32_t height, uint32_t windowStyle)
{
	::HINSTANCE hInst = ::GetModuleHandle(nullptr);
	assert(hInst && pTitle);

	::WNDCLASSEX wndClassEx = {};
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

	if (::RegisterClassEx(&wndClassEx))
	{
		::RECT rect{ 0, 0, (long)width, (long)height };
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

void IApplication::ResizeWindow(uint32_t width, uint32_t height)
{
	if (m_bRenderEngineInited)
	{
		REngine::Instance().Resize(width, height);
	}

	m_Camera.SetProjParams(DirectX::XM_PIDIV4, (float)width / height, 1.0f, 3000.0f);
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
		MouseButtonDown(wParam, (int32_t)LOWORD(lParam), (int32_t)HIWORD(lParam));
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		///MouseButtonUp(wParam, (int32_t)LOWORD(lParam), (int32_t)HIWORD(lParam));
		break;
	case WM_MOUSEMOVE:
		MouseMove(wParam, (int32_t)LOWORD(lParam), (int32_t)HIWORD(lParam));
		break;
	case WM_MOUSEWHEEL:
		MouseWheel(wParam);
		break;
	case WM_KEYDOWN:
		///Keyboard(wParam);
		break;
	}
}

void IApplication::RenderToWindow()
{
	m_Camera.Update();

	UpdateScene(m_Timer.GetDeltaTime(), m_Timer.GetTotalTime());

	ImGUI::Instance().RenderBegin(m_bDrawGUI);

	RenderScene();

	ImGUI::Instance().RenderEnd(m_bDrawGUI);

	REngine::Instance().Flush();

	UpdateFPS();
}

void IApplication::UpdateWindow()
{
	if (!m_bNeedResize)
	{
		return;
	}

	::RECT rect = {};
	::GetClientRect(m_hWnd, &rect);

	m_Width = std::max<uint32_t>((rect.right - rect.left), 32U);
	m_Height = std::max<uint32_t>((rect.bottom - rect.top), 32U);

	ResizeWindow(m_Width, m_Height);

	m_bNeedResize = false;
}

void IApplication::Startup(const wchar_t *pTitle, uint32_t width, uint32_t height, bool bFullScreen, uint32_t windowStyle)
{
	s_Application = this;

	m_bFullScreen = bFullScreen;

	MakeWindow(pTitle, width, height, windowStyle);

	if (!m_bRenderEngineInited)
	{
		REngine::Instance().Initialize(m_hWnd, m_Width, m_Height, m_bFullScreen);

		ImGUI::Instance().Initialize(m_hWnd);

		m_Camera.SetProjParams(DirectX::XM_PIDIV4, (float)m_Width / m_Height, 1.0f, 3000.0f);

		m_bRenderEngineInited = true;
	}

	InitScene();
}

void IApplication::MouseWheel(::WPARAM wParam)
{
	m_MouseWheelDelta = (int16_t)HIWORD(wParam);

	/// Not perfect 
	float radius = m_Camera.GetViewRadius();
	float radiusCopy = radius;

	radius -= m_MouseWheelDelta * radius * 0.1f / 120.0f;
	radius = Math::Clamp(radius, radiusCopy / 2.0f, radiusCopy * 2.0f);

	m_Camera.SetViewRadius(radius);
}

void IApplication::UpdateFPS()
{
	static uint32_t s_FrameNumber = 0U;
	static float s_LastUpdateTime = 0.0f;

	float totalTime = m_Timer.GetTotalTime();
	s_FrameNumber++;

	float elapseTime = totalTime - s_LastUpdateTime;
	if (elapseTime > 1.0f)
	{
		m_FPS = s_FrameNumber / elapseTime;

		s_LastUpdateTime = totalTime;
		s_FrameNumber = 0U;
	}
}

void IApplication::Running()
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

				RenderToWindow();
			}
			else
			{
				::Sleep(100);
			}
		}
	}
}

void IApplication::ShutDown()
{
	ImGUI::Instance().Destroy();
}