#include "IApplication.h"
#include "resource.h"
#include "D3DGraphic.h"
#include "Timer.h"

#include "imGui_D3D11.h"

static IApplication* s_Application = nullptr;
D3DGraphic* g_Renderer = nullptr;

extern LRESULT imGUI_WinProc(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
#ifdef UsingimGUI
	if (imGUI_WinProc(hWnd, msg, wParam, lParam))
	{
		return 1LL;
	}
#endif

	return s_Application->MsgProc(hWnd, msg, wParam, lParam);
}

IApplication::IApplication()
	: m_hWnd(nullptr)
	, m_bActive(true)
	, m_bInited(false)
	, m_Width(0U)
	, m_Height(0U)
	, m_pTimer(new Timer())
{
	s_Application = this;
	memset(m_LastMousePos, 0, sizeof(int) * 2);
}

void IApplication::MakeWindow(LPCWSTR lpTitle, uint32_t width, uint32_t height)
{
	HINSTANCE hInst = ::GetModuleHandle(nullptr);
	assert(hInst && lpTitle);

	WNDCLASSEX wndClassEx;
	memset(&wndClassEx, 0, sizeof(WNDCLASSEX));
	wndClassEx.cbClsExtra = 0;
	wndClassEx.cbSize = sizeof(WNDCLASSEX);
	wndClassEx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClassEx.hCursor = LoadCursor(0, IDC_ARROW);
	wndClassEx.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_APP));
	wndClassEx.hIconSm = wndClassEx.hIcon;
	wndClassEx.hInstance = hInst;
	wndClassEx.lpfnWndProc = WndProc;
	wndClassEx.lpszClassName = lpTitle;
	wndClassEx.lpszMenuName = nullptr;
	wndClassEx.style = CS_HREDRAW | CS_VREDRAW;

	if (RegisterClassEx(&wndClassEx))
	{
		RECT rect{ 0, 0, (long)width, (long)height };
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
		m_hWnd = CreateWindow(lpTitle, lpTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			rect.right - rect.left, rect.bottom - rect.top, 0, 0, hInst, nullptr);
		assert(m_hWnd);

		m_Width = width;
		m_Height = height;

		ShowWindow(m_hWnd, SW_SHOWDEFAULT);
		UpdateWindow(m_hWnd);
	}
	else
	{
		assert(!"Failed to create window!!!");
	}
}

void IApplication::MouseButtonDown(WPARAM /*wParam*/, int x, int y)
{
	m_LastMousePos[0] = x;
	m_LastMousePos[1] = y;
}

LRESULT IApplication::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	assert(m_pTimer);

#ifdef UsingAntTweakBar
	if (m_GUI.WinProc(hWnd, msg, wParam, lParam))
	{
		return 0LL;
	}
#endif

	switch (msg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_bActive = false;
			m_pTimer->Stop();
		}
		else
		{
			m_bActive = true;
			m_pTimer->Start();
		}
		break;
	case WM_ENTERSIZEMOVE:
		m_bActive = false;
		m_pTimer->Stop();
		break;
	case WM_SIZE:
	case WM_EXITSIZEMOVE:
	{
		m_pTimer->Start();
		RECT rect;
		::GetClientRect(m_hWnd, &rect);

#ifdef UsingimGUI
		imGUI_D3D_Resize(true, true);
#endif
		
		ResizeWindow(rect.right - rect.left, rect.bottom - rect.top);
		m_bActive = true;
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_GETMINMAXINFO:
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		MouseButtonDown(wParam, (int)LOWORD(lParam), (int)HIWORD(lParam));
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		MouseButtonUp(wParam, (int)LOWORD(lParam), (int)HIWORD(lParam));
		break;
	case WM_MOUSEMOVE:
		MouseMove(wParam, (int)LOWORD(lParam), (int)HIWORD(lParam));
		break;
	case WM_MOUSEHWHEEL:
	case WM_KEYDOWN:
		Keyboard(wParam);
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void IApplication::ResizeWindow(uint32_t width, uint32_t height)
{
	uint32_t dstWidth = max(width, 32U);
	uint32_t dstHeight = max(height, 32U);
	if (g_Renderer)
	{
		m_Width = dstWidth;
		m_Height = dstHeight;

		g_Renderer->ResizeBackBuffer(dstWidth, dstHeight);

		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = viewport.TopLeftY = 0.0f;
		viewport.Width = (float)dstWidth;
		viewport.Height = (float)dstHeight;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		g_Renderer->SetViewports(&viewport);
	}
}

void IApplication::Startup(LPCWSTR lpTitle, uint32_t width, uint32_t height, bool bWindowed)
{
	MakeWindow(lpTitle, width, height);

	if (nullptr == g_Renderer)
	{
		D3DGraphic::CreateInstance();
		g_Renderer = D3DGraphic::GetInstance();
		g_Renderer->InitD3DEnvironment(m_hWnd, m_Width, m_Height, bWindowed);
	}

#ifdef UsingimGUI
	imGUI_D3D_Init(&m_hWnd);
#elif defined(UsingAntTweakBar)
	m_GUI.Init(g_Renderer->GetDevice(), m_Width, m_Height);
#endif
}

void IApplication::Running()
{
	MSG msg = { 0 };

	m_pTimer->Reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_pTimer->Tick();

			if (!m_bInited)
			{
				SetupScene();
			}

			if (m_bActive)
			{
				UpdateScene(m_pTimer->DeltaTime(), m_pTimer->TotalTime());  

#ifdef UsingimGUI
				imGUI_D3D_RenderBegin();
#endif

				RenderScene();

#ifdef UsingimGUI
				imGUI_D3D_RenderEnd();
#elif defined(UsingAntTweakBar)
				m_GUI.Draw();
#endif

				g_Renderer->Flip();
			}
			else
			{
				Sleep(100);
			}
		}
	}
}

void IApplication::ShutDown()
{
#ifdef UsingimGUI
	imGUI_D3D_Shutdown();
#endif

	D3DGraphic::DestoryInstance();
}

IApplication::~IApplication()
{
	SafeDelete(m_pTimer);
}