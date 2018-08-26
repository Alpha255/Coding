#include "RenderApp.h"
#include "resource.h"
#include "Timer.h"
#include "Camera.h"
#include "ImGUI.h"

#ifdef UsingD3D11
#include "D3DEngine.h"
typedef D3DEngine Renderer;
#elif defined(UsingVulkan)
#include "VulkanEngine.h"
typedef VkEngine Renderer;
#else
#error "Undefine Renderer"
#endif

RenderApp::RenderApp()
	: m_Camera(new Camera())
{
	m_IconID = IDI_ICON_APP;
}

LRESULT RenderApp::MsgProc(::HWND hWnd, uint32_t msg, ::WPARAM wParam, ::LPARAM lParam)
{
	assert(m_Timer);

	if (m_bRenderedInited && ImGUI::Instance().WinProc(hWnd, msg, wParam, lParam))
	{
		return 1LL;
	}

	HandleWindowMessage(msg, wParam, lParam);

	if (!m_bRenderedInited || !ImGUI::Instance().IsFocus())
	{
		HandleInput(msg, wParam, lParam);
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

void RenderApp::ResizeWindow(uint32_t width, uint32_t height)
{
	if (m_bRenderedInited)
	{
		Renderer::Instance().Resize(width, height);
	}

	m_Camera->SetProjParams(DirectX::XM_PIDIV4, (float)width / height, 1.0f, 3000.0f);
}

void RenderApp::MouseWheel(::WPARAM wParam)
{
	m_MouseWheelDelta = (short)HIWORD(wParam);

	/// Not perfect 
	float radius = m_Camera->GetViewRadius();
	float radiusCopy = radius;

	radius -= m_MouseWheelDelta * radius * 0.1f / 120.0f;
	radius = Math::Clamp(radius, radiusCopy / 2.0f, radiusCopy * 2.0f);

	m_Camera->SetViewRadius(radius);
}

void RenderApp::MouseMove(::WPARAM wParam, int32_t x, int32_t y)
{
	if ((wParam & MK_LBUTTON) != 0)
	{
		m_Camera->Move(x - m_LastMousePos[0], y - m_LastMousePos[1]);
	}

	IApplication::MouseMove(wParam, x, y);
}

void RenderApp::InitRenderer()
{
	if (!m_bRenderedInited)
	{
		Renderer::Instance().Initialize(m_hWnd, m_Width, m_Height, m_bWindowed);

		ImGUI::Instance().Initialize(m_hWnd);

		m_Camera->SetProjParams(DirectX::XM_PIDIV4, (float)m_Width / m_Height, 1.0f, 3000.0f);

		m_bRenderedInited = true;
	}
}

void RenderApp::UpdateFPS()
{
	static uint32_t s_FrameNumber = 0U;
	static float s_LastUpdateTime = 0.0f;

	float totalTime = m_Timer->TotalTime();
	s_FrameNumber++;

	float elapseTime = totalTime - s_LastUpdateTime;
	if (elapseTime > 1.0f)
	{
		m_FPS = s_FrameNumber / elapseTime;

		s_LastUpdateTime = totalTime;
		s_FrameNumber = 0U;
	}
}

void RenderApp::Frame()
{
	m_Camera->Update();

	Update(m_Timer->DeltaTime(), m_Timer->TotalTime());

	ImGUI::Instance().RenderBegin(m_bRenderGUI);

	RenderScene();

	ImGUI::Instance().RenderEnd(m_bRenderGUI);

	Renderer::Instance().Flush();

	UpdateFPS();
}

void RenderApp::RenderToWindow()
{
	if (!m_bSceneInited)
	{
		Initialize();
		m_bSceneInited = true;
	}

	Frame();
}

RenderApp::~RenderApp()
{
	ImGUI::Destroy();
}
