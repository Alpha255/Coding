#include "MultithreadedRendering.h"
#include "ImGUI.h"

void MultithreadedRendering::PrepareScene()
{
	m_Scene.Initialize();

	AutoFocus(m_Scene.SquidRoom, 0.3f);

	m_ThreadTest.Start();
}

void MultithreadedRendering::RenderScene()
{
	m_Scene.Draw(m_Camera, m_WindowSize.first, m_WindowSize.second);

	ImGui::Text("FPS: %.2f", m_FPS);
	ImGui::Combo("RenderingMode", &m_RenderingMode, "SingleThread\0MultithreadedPerScene\0MultithreadedPerChunk");
}
