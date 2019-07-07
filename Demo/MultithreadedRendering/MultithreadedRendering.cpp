#include "MultithreadedRendering.h"
#include "ImGUI.h"

void MultithreadedRendering::RenderThreadFunc(const Scene::eDrawType &drawType)
{
	m_Scene.DrawByPart(m_Camera, m_WindowSize.first, m_WindowSize.second, drawType, m_Context[drawType]);

	m_Context[drawType].FinishCommandList(true, m_CommandList[drawType]);
}

void MultithreadedRendering::PrepareScene()
{
	m_Scene.Initialize();

	AutoFocus(m_Scene.SquidRoom, 0.3f);

	for (uint32_t i = 0U; i < Scene::eTypeCount; ++i)
	{
		m_Threads[i].Bind(std::bind(&MultithreadedRendering::RenderThreadFunc, this, std::placeholders::_1));
		m_Context[i].CreateAsDeferredContext();
	}
}

void MultithreadedRendering::RenderScene()
{
	if (m_RenderingMode == eSingelThread)
	{
		for (uint32_t i = 0U; i < Scene::eTypeCount; ++i)
		{
			m_Threads[i].Suspend();
		}

		m_Scene.Draw(m_Camera, m_WindowSize.first, m_WindowSize.second);
	}
	else if (m_RenderingMode == eMultiThreadByScene && m_bActive)
	{
		if (m_ThreadTaskDone)
		{
			for (uint32_t i = 0U; i < Scene::eTypeCount; ++i)
			{
				m_Threads[i].Start((Scene::eDrawType)i);
			}
		}

		bool bTaskDone = true;
		for (uint32_t i = 0U; i < Scene::eTypeCount; ++i)
		{
			bTaskDone &= m_Threads[i].IsDone();
		}
		m_ThreadTaskDone = bTaskDone;

		if (m_ThreadTaskDone)
		{
			for (uint32_t i = 0U; i < Scene::eTypeCount; ++i)
			{
				REngine::Instance().ExecuteCommandList(true, m_CommandList[i]);
			}
		}

		REngine::Instance().ResetDefaultRenderSurfaces(Color::DarkBlue, nullptr, false);
	}

	ImGui::Text("FPS: %.2f", m_FPS);
	ImGui::Combo("RenderingMode", &m_RenderingMode, "SingleThread\0Multithread");
}
