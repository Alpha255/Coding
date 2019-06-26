#include "IRenderApplication.h"
#include "Definitions.h"
#include "ImGUI.h"
#include "Model.h"

void IRenderApplication::Initialize(const std::string &title, uint32_t width, uint32_t height, bool bFullScreen, uint32_t extraWindowStyle)
{
	m_IconID = IconID;
	Base::Initialize(title, width, height, bFullScreen, extraWindowStyle);

	REngine::Instance().Initialize(m_hWnd, width, height, bFullScreen);

	ImGUI::Instance().Initialize(m_hWnd);

	PrepareScene();
}

void IRenderApplication::HandleWindowMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam)
{
	Base::HandleWindowMessage(msg, wParam, lParam);

	ImGUI::Instance().HandleWindowMessage(m_hWnd, msg, wParam, lParam);
}

void IRenderApplication::HandleInput(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam)
{
	Base::HandleInput(msg, wParam, lParam);

	if (ImGUI::Instance().IsInitialized() && !ImGUI::Instance().IsFocus())
	{
		m_Camera.HandleWindowMessage(msg, wParam, lParam);
	}
}

void IRenderApplication::AutoFocus(const Geometry::Model &model, float scale)
{
	if (model.HasBoundingBox())
	{
		Geometry::Box boundingBox = model.GetBoundingBox();
		Vec3 center = boundingBox.GetCenter();
		Vec3 size = boundingBox.GetSize();
		if (size.z > 500.0f)
		{
			m_CameraParams.y = size.z * 2.0f;
		}

		assert(scale > 0.0f);

		m_Camera.SetProjParams(DirectX::XM_PIDIV4, (float)m_WindowSize.first / m_WindowSize.second, m_CameraParams.x, m_CameraParams.y);
		m_Camera.SetViewParams(Vec3(center.x + size.x * scale, center.y + size.y * scale, center.z + size.z * scale), Vec3(center.x, center.y, center.z));

		float min = std::min<float>(size.x, size.y);
		min = std::min<float>(min, size.z);
		m_Camera.SetScalers(0.01f, min / 3.0f);
	}
	else
	{
		m_CameraParams.y = 500.0f;
		m_Camera.SetProjParams(DirectX::XM_PIDIV4, (float)m_WindowSize.first / m_WindowSize.second, m_CameraParams.x, m_CameraParams.y);
		m_Camera.SetViewParams(Vec3(71.0f, 41.0f, 71.0f), Vec3());
		m_Camera.SetScalers(0.01f, 15.0f);
	}
}

void IRenderApplication::ResizeWindow(uint32_t width, uint32_t height)
{
	Base::ResizeWindow(width, height);

	REngine::Instance().Resize(width, height);

	m_Camera.SetProjParams(DirectX::XM_PIDIV4, (float)width / height, m_CameraParams.x, m_CameraParams.y);
}

void IRenderApplication::RenterToWindow()
{
	m_Camera.Update(m_Timer.GetElapsedTime());

	UpdateScene(m_Timer.GetElapsedTime(), m_Timer.GetTotalTime());

	ImGUI::Instance().RenderBegin();

	RenderScene();

	ImGUI::Instance().RenderEnd();

	if (ImGUI::Instance().Update())
	{
		REngine::Instance().Flush();
	}
}

void IRenderApplication::Finalize()
{
	REngine::Instance().Finalize();

	ImGUI::Instance().Finalize();
}