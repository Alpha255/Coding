#include "IApplication.h"
#include "main.h"

#include <assert.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return 1LL;
	}

	switch (msg)
	{
	case WM_SIZE:
		if (GetDevice() && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX11_InvalidateDeviceObjects();

			Resize((UINT)LOWORD(lParam), (UINT)HIWORD(lParam));

			ImGui_ImplDX11_CreateDeviceObjects();
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

IApplication::IApplication()
	: m_hWnd(nullptr)
	, m_Width(0U)
	, m_Height(0U)
{
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
	wndClassEx.hIcon = nullptr;
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

void IApplication::ResizeWindow(uint32_t width, uint32_t height)
{
	uint32_t dstWidth = max(width, 32U);
	uint32_t dstHeight = max(height, 32U);
}

void IApplication::Startup(LPCWSTR lpTitle, uint32_t width, uint32_t height, bool bWindowed)
{
	MakeWindow(lpTitle, width, height);

	CreateDeviceD3D(m_hWnd);

	ImGui_ImplDX11_Init(&m_hWnd, GetDevice(), GetContext());
}

void IApplication::Running()
{
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		RenderScene();
	}
}

void IApplication::ShutDown()
{
	ImGui_ImplDX11_Shutdown();
	CleanupDeviceD3D();
}

void IApplication::RenderScene()
{
	bool show_test_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.0f, 0.125f, 0.3f, 1.0f);

	ImGui_ImplDX11_NewFrame();

	// 1. Show a simple window.
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug".
	{
		static float f = 0.0f;
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", (float*)&clear_color);
		if (ImGui::Button("Test Window")) show_test_window ^= 1;
		if (ImGui::Button("Another Window")) show_another_window ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	// 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name the window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);
		ImGui::Text("Hello from another window!");
		ImGui::End();
	}

	// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow().
	if (show_test_window)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);     // Normally user code doesn't need/want to call it because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
		ImGui::ShowTestWindow(&show_test_window);
	}

	ClearRenderTarget((float*)&clear_color);

	ImGui::Render();

	Flip();
}