#include "Lighting.h"
#include "RefCountPtr.h"
#include "D3DGraphic.h"
#include "D3DModel.h"
#include "D3DMath.h"
#include "Camera.h"

#include <GUI/ocornut/imgui.h>
#include "GUI_AntTweakBar.h"

extern D3DGraphic* g_Renderer;

struct DemoLightingResource
{
	D3D11_VIEWPORT Viewport;

	SDKMesh Model;
};

static DemoLightingResource s_Resource;

bool ApplicationLighting::m_Wireframe = false;

void ApplicationLighting::SetupScene()
{
#if 1 
	const char *pMeshName = "skull.txt";
#else
	const char *pMeshName = "car.txt";
#endif
	s_Resource.Model.CreateFromTxt(pMeshName);
	Matrix modelWorld = Matrix::Scaling(0.8f, 0.8f, 0.8f) * Matrix::Translation(0.0f, -1.0f, 0.0f);
	s_Resource.Model.SetWorldMatrix(modelWorld);

	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	s_Resource.Viewport.Width = (float)m_Width;
	s_Resource.Viewport.Height = (float)m_Height;
	s_Resource.Viewport.MinDepth = 0.0f;
	s_Resource.Viewport.MaxDepth = 1.0f;
	s_Resource.Viewport.TopLeftX = s_Resource.Viewport.TopLeftY = 0.0f;
	g_Renderer->SetViewports(&s_Resource.Viewport);

#ifdef UsingAntTweakBar
	GUIAntTweakBar::WidgeDesc EnableWireframe;
	EnableWireframe.Type = GUIAntTweakBar::eCheckBox;
	EnableWireframe.Title = "Wireframe";
	EnableWireframe.GetVarFunc = GetWireframe;
	EnableWireframe.SetVarFunc = SetWireframe;
	m_GUI.AddWidget(EnableWireframe);
#endif
}

void ApplicationLighting::RenderScene()
{
	g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);
	
	if (m_CurLightCount != m_PreLightCount)
	{
#ifdef _DEBUG
		char debugOutput[MAX_PATH] = { 0 };
		sprintf_s(debugOutput, MAX_PATH, "\nLight count change to %d\n", m_CurLightCount);
		OutputDebugStringA(debugOutput);
#endif
		m_PreLightCount = m_CurLightCount;
	}

	s_Resource.Model.SetLightCount(m_CurLightCount);

	s_Resource.Model.Draw(*m_Camera, m_Wireframe);

#ifdef UsingimGUI
	ImGui::Checkbox("Wireframe", &m_Wireframe);
	ImGui::SliderInt("Light Count", &m_CurLightCount, 0, 3);
#endif
}
