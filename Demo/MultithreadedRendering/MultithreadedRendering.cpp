#include "MultithreadedRendering.h"
#include "ImGUI.h"

void MultithreadedRendering::RenderThreadFunc(void *pParams)
{
	uint32_t *param = (uint32_t *)(pParams);
	if (*param == 128U)
	{
		Base::Log("Yes!");
	}
	else
	{
		Base::Log("No!\n");
	}

	::Sleep(1000);
}

void MultithreadedRendering::PrepareScene()
{
	m_Scene.Initialize();

	AutoFocus(m_Scene.SquidRoom, 0.3f);

	m_Thread.Bind(std::bind(&MultithreadedRendering::RenderThreadFunc, this, std::placeholders::_1));
}

void MultithreadedRendering::RenderScene()
{
	m_Scene.Draw(m_Camera, m_WindowSize.first, m_WindowSize.second);

	ImGui::Text("FPS: %.2f", m_FPS);
	ImGui::Combo("RenderingMode", &m_RenderingMode, "SingleThread\0Multithread");

	if (m_RenderingMode == eSingelThread)
	{
		m_Thread.Suspend();
	}
	else if (m_RenderingMode == eMultiThreadByScene)
	{
		static uint32_t s_Param = 128U;
		m_Thread.Start(&s_Param);
	}
}
