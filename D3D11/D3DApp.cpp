#include "D3DApp.h"
#include "resource.h"
#include "Timer.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "D3DGUI_imGui.h"

#include <d3d11.h>

D3DApp::D3DApp()
	: m_Camera(new Camera())
{
	m_IconID = IDI_ICON_APP;
}

LRESULT D3DApp::MsgProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam)
{
	assert(m_pTimer);

	if (s_GUI && s_GUI->WinProc(hWnd, msg, wParam, lParam))
	{
		return 1LL;
	}

	HandleWindowMessage(msg, wParam, lParam);

	if (!s_GUI || !s_GUI->IsFocus())
	{
		HandleInput(msg, wParam, lParam);
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

void D3DApp::ResizeWindow(uint32_t width, uint32_t height)
{
	if (s_Renderer)
	{
		s_Renderer->Resize(width, height);
	}

	m_Camera->SetProjParams(DirectX::XM_PIDIV4, (float)width / height, 1.0f, 3000.0f);
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

		s_GUI = &D3DGUI_imGui::Instance();
		assert(s_GUI);
		s_GUI->Initialize(m_hWnd);

		m_Camera->SetProjParams(DirectX::XM_PIDIV4, (float)m_Width / m_Height, 1.0f, 3000.0f);
	}
}

void D3DApp::UpdateFPS()
{
	static uint32_t s_FrameNumber = 0U;
	static float s_LastUpdateTime = 0.0f;

	float totalTime = m_pTimer->TotalTime();
	s_FrameNumber++;

	float elapseTime = totalTime - s_LastUpdateTime;
	if (elapseTime > 1.0f)
	{
		m_FPS = s_FrameNumber / elapseTime;

		s_LastUpdateTime = totalTime;
		s_FrameNumber = 0U;
	}
}

void D3DApp::Frame()
{
	m_Camera->Update();

	Update(m_pTimer->DeltaTime(), m_pTimer->TotalTime());

	s_Renderer->ResetRenderSurfaces();

	s_Renderer->ClearRenderSurfaces();

	s_Renderer->SetViewport(D3DViewport(0.0f, 0.0f, (float)m_Width, (float)m_Height));

	s_GUI->RenderBegin();

	RenderScene();

	s_GUI->RenderEnd();

	s_Renderer->Flush();

	UpdateFPS();
}

void D3DApp::RenderToWindow()
{
	if (!m_bInited)
	{
		Initialize();
		m_bInited = true;
	}

	Frame();
}

D3DApp::~D3DApp()
{
	SafeDelete(m_Camera);

	D3DGUI_imGui::Destroy(s_GUI);
}
