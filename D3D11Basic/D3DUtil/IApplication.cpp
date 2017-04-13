#include "IApplication.h"
#include "resource.h"
#include "D3DGraphic.h"
#include "Timer.h"

static IApplication* s_Application = nullptr;
D3DGraphic* g_Renderer = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return s_Application->MsgProc(hWnd, msg, wParam, lParam);
}

IApplication::IApplication(HINSTANCE hInstance, LPCWSTR lpTitle, uint32_t width, uint32_t height, bool bWindowed)
	: m_hWnd(nullptr)
	, m_bActive(true)
	, m_pTimer(new Timer())
{
	s_Application = this;
	memset(m_LastMousePos, 0, sizeof(int) * 2);
	memset(m_Size, 0, sizeof(uint32_t) * 2);
	MakeWindow(hInstance, lpTitle, width, height);

	D3DGraphic::CreateInstance();
	g_Renderer = D3DGraphic::GetInstance();
	g_Renderer->InitD3DEnvironment(m_hWnd, width, height, bWindowed);
}

void IApplication::MakeWindow(HINSTANCE hInstance, LPCWSTR lpTitle, uint32_t width, uint32_t height)
{
	assert(hInstance && lpTitle);

	WNDCLASSEX wndClassEx;
	memset(&wndClassEx, 0, sizeof(WNDCLASSEX));
	wndClassEx.cbClsExtra = 0;
	wndClassEx.cbSize = sizeof(WNDCLASSEX);
	wndClassEx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClassEx.hCursor = LoadCursor(0, IDC_ARROW);
	wndClassEx.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_APP));
	wndClassEx.hIconSm = wndClassEx.hIcon;
	wndClassEx.hInstance = hInstance;
	wndClassEx.lpfnWndProc = WndProc;
	wndClassEx.lpszClassName = lpTitle;
	wndClassEx.lpszMenuName = nullptr;
	wndClassEx.style = CS_HREDRAW | CS_VREDRAW;

	if (RegisterClassEx(&wndClassEx))
	{
		RECT rect{ 0, 0, (long)width, (long)height };
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
		m_hWnd = CreateWindow(lpTitle, lpTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			rect.right - rect.left, rect.bottom - rect.top, 0, 0, hInstance, nullptr);
		assert(m_hWnd);

		m_Size[0] = width;
		m_Size[1] = height;

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
		ResizeWindow(rect.right - rect.left, rect.bottom - rect.top);
	}
	m_bActive = true;
	break;
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
		m_Size[0] = dstWidth;
		m_Size[1] = dstHeight;

		g_Renderer->ResizeBackBuffer(dstWidth, dstHeight);
	}
}

int IApplication::MainLoop()
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

			if (m_bActive)
			{
				UpdateScene(m_pTimer->DeltaTime(), m_pTimer->TotalTime());  

				RenderScene();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (int)msg.wParam;
}

IApplication::~IApplication()
{
	D3DGraphic::DestoryInstance();
	SafeDelete(m_pTimer);
}