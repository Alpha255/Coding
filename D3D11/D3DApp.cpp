#include "D3DApp.h"
#include "resource.h"
#include "Timer.h"
#include "Camera.h"
#include "D3DEngine.h"

#include <d3d11.h>

D3DApp::D3DApp()
	: m_Camera(new Camera())
{
	m_IconID = IDI_ICON_APP;
}

LRESULT D3DApp::MsgProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam)
{
	assert(m_pTimer);

	//if (m_GUI.WinProc(hWnd, msg, wParam, lParam))
	//{
	//	return 1LL;
	//}

	HandleWindowMessage(msg, wParam, lParam);

	//if (!m_GUI.IsFocus())
	//{
	//	HandleInput(msg, wParam, lParam);
	//}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

void D3DApp::ResizeWindow(uint32_t width, uint32_t height)
{
	uint32_t dstWidth = std::max<uint32_t>(width, 32U);
	uint32_t dstHeight = std::max<uint32_t>(height, 32U);

	if (s_Renderer)
	{
		m_Width = dstWidth;
		m_Height = dstHeight;

		s_Renderer->Resize(dstWidth, dstHeight);
	}

	m_Camera->SetProjParams(DirectX::XM_PIDIV4, (float)width / height, 1.0f, 1000.0f);
}

void D3DApp::MouseWheel(WPARAM wParam)
{
	m_MouseWheelDelta = (short)HIWORD(wParam);

	/// Not perfect 
	float radius = m_Camera->GetViewRadius();
	float radiusCopy = radius;

	radius -= m_MouseWheelDelta * radius * 0.1f / 120.0f;
	radius = Math::Clamp(radius, radiusCopy / 2.0f, radiusCopy * 2.0f);

	m_Camera->SetViewRadius(radius);
}

void D3DApp::MouseMove(WPARAM wParam, int32_t x, int32_t y)
{
	if ((wParam & MK_LBUTTON) != 0)
	{
		m_Camera->Move(x - m_LastMousePos[0], y - m_LastMousePos[1]);
	}

	IApplication::MouseMove(wParam, x, y);
}

void D3DApp::InitRenderer()
{
	if (nullptr == s_Renderer)
	{
		s_Renderer = &D3DEngine::Instance();

		assert(s_Renderer);

		s_Renderer->Initialize(m_hWnd, m_Width, m_Height, m_bWindowed);

		///m_GUI.Init(m_hWnd);
	}
}

void D3DApp::Frame()
{
	m_Camera->Update();

	Update(m_pTimer->DeltaTime(), m_pTimer->TotalTime());

	s_Renderer->ClearRenderSurfaces();

	///m_GUI.RenderBegin();

	RenderScene();

	///m_GUI.RenderEnd();

	s_Renderer->Flush();
}

void D3DApp::RenderToWindow()
{
	if (!m_bInited)
	{
		PreInit();
		m_bInited = true;
	}

	Frame();
}

D3DApp::~D3DApp()
{
	SafeDelete(m_Camera);
}
