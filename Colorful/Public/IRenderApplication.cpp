#include "IRenderApplication.h"
#include "Definitions.h"

void IRenderApplication::Initialize(const std::string &title, uint32_t width, uint32_t height, bool bFullScreen, uint32_t extraWindowStyle)
{
	m_IconID = IconID;
	Base::Initialize(title, width, height, bFullScreen, extraWindowStyle);

	REngine::Instance().Initialize(m_hWnd, width, height, bFullScreen);

	PrepareScene();

	m_Camera.SetProjParams(DirectX::XM_PIDIV4, (float)width / height, m_CameraParams.first, m_CameraParams.second);
}

void IRenderApplication::HandleWindowMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam)
{
	Base::HandleWindowMessage(msg, wParam, lParam);
}

void IRenderApplication::HandleInput(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam)
{
	Base::HandleInput(msg, wParam, lParam);

	switch (msg)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		HandleMouseInput(wParam, lParam, eMouseButtonDown);
		break;
	case WM_MOUSEMOVE:
		HandleMouseInput(wParam, lParam, eMouseMove);
		break;
	case WM_MOUSEWHEEL:
		HandleMouseInput(wParam, lParam, eMouseWheel);
		break;
	}
}

void IRenderApplication::HandleMouseInput(::WPARAM wParam, ::LPARAM lParam, eMouseInput type)
{
	int32_t xPos = (int32_t)(LOWORD(lParam));
	int32_t yPos = (int32_t)(HIWORD(lParam));

	switch (type)
	{
	case eMouseMove:
		if ((wParam & MK_LBUTTON) != 0)
		{
			m_Camera.Move(xPos - m_MousePos.first, yPos - m_MousePos.second);
		}
		break;
	case eMouseButtonDown:
		break;
	case eMouseWheel:
	{
		int16_t mouseWheelDelta = (int16_t)HIWORD(wParam);

		/// Not perfect 
		float radius = m_Camera.GetViewRadius();
		float radiusCopy = radius;

		radius -= mouseWheelDelta * radius * 0.1f / 120.0f;
		radius = Math::Clamp(radius, radiusCopy / 2.0f, radiusCopy * 2.0f);

		m_Camera.SetViewRadius(radius);
	}
		break;
	}

	m_MousePos.first = xPos;
	m_MousePos.second = yPos;
}

void IRenderApplication::ResizeWindow(uint32_t width, uint32_t height)
{
	Base::ResizeWindow(width, height);

	REngine::Instance().Resize(width, height);

	m_Camera.SetProjParams(DirectX::XM_PIDIV4, (float)width / height, m_CameraParams.first, m_CameraParams.second);
}

void IRenderApplication::RenterToWindow()
{
	m_Camera.Update();

	UpdateScene(m_Timer.GetDeltaTime(), m_Timer.GetTotalTime());

	RenderScene();
}

void IRenderApplication::Finalize()
{

}