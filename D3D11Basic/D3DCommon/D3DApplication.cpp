#include "D3DApplication.h"

#include "Timer.h"
#include "Camera.h"
#include "D3DGraphic.h"

D3DGraphic* g_Renderer = nullptr;

D3DApplication::D3DApplication()
	: m_Camera(new Camera())
{
	m_IconID = IDI_ICON_APP;
}

LRESULT D3DApplication::MsgProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam)
{
	assert(m_pTimer);

	if (m_GUI.WinProc(hWnd, msg, wParam, lParam))
	{
		return 1LL;
	}

	HandleWindowMessage(msg, wParam, lParam);

	if (!m_GUI.IsFocus())
	{
		HandleInput(msg, wParam, lParam);
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

void D3DApplication::ResizeWindow(uint32_t width, uint32_t height)
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

	m_Camera->SetProjParams(DirectX::XM_PIDIV4, (float)width / height, 1.0f, 1000.0f);
}

void D3DApplication::MouseWheel(WPARAM wParam)
{
	m_MouseWheelDelta = (short)HIWORD(wParam);

	/// Not perfect 
	float radius = m_Camera->GetViewRadius();
	float radiusCopy = radius;

	radius -= m_MouseWheelDelta * radius * 0.1f / 120.0f;
	radius = Math::Clamp(radius, radiusCopy / 2.0f, radiusCopy * 2.0f);

	m_Camera->SetViewRadius(radius);
}

void D3DApplication::MouseMove(WPARAM wParam, int32_t x, int32_t y)
{
	if ((wParam & MK_LBUTTON) != 0)
	{
		m_Camera->Move(x - m_LastMousePos[0], y - m_LastMousePos[1]);
	}

	IApplication::MouseMove(wParam, x, y);
}

void D3DApplication::UpdateScene(float, float)
{
	m_Camera->Update();
}

void D3DApplication::RenderToWindow()
{
	if (nullptr == g_Renderer)
	{
		D3DGraphic::CreateInstance();
		g_Renderer = D3DGraphic::GetInstance();
		g_Renderer->InitD3DEnvironment(m_hWnd, m_Width, m_Height, m_bWindowed);

		m_GUI.Init(m_hWnd);
	}

	if (!m_bInited)
	{
		SetupScene();
		m_bInited = true;
	}

	UpdateScene(m_pTimer->DeltaTime(), m_pTimer->TotalTime());  

	m_GUI.RenderBegin();

	RenderScene();

	m_GUI.RenderEnd();

	g_Renderer->Flip();
}

D3DApplication::~D3DApplication()
{
	SafeDelete(m_Camera);
	D3DGraphic::DestoryInstance();
}
