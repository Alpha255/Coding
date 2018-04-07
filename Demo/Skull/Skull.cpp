#include "Lighting.h"
#include "RefCountPtr.h"
#include "D3DGraphic.h"
#include "D3DModel.h"
#include "D3DMath.h"
#include "Camera.h"

#include <GUI/ocornut/imgui.h>
#include "GUI_AntTweakBar.h"

extern D3DGraphic* g_Renderer;

static SDKMesh s_Model;

bool ApplicationLighting::m_Wireframe = false;

void ApplicationLighting::SetupScene()
{
#if 1 
	const char *pMeshName = "skull.txt";
#else
	const char *pMeshName = "car.txt";
#endif
	s_Model.CreateFromTxt(pMeshName);
	Matrix modelWorld = Matrix::Scaling(0.8f, 0.8f, 0.8f) * Matrix::Translation(0.0f, -1.0f, 0.0f);
	s_Model.SetWorldMatrix(modelWorld);

	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	D3D11_VIEWPORT vp;
	vp.Width = (float)m_Width;
	vp.Height = (float)m_Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = vp.TopLeftY = 0.0f;
	g_Renderer->SetViewports(&vp);

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

	s_Model.SetLightCount(m_CurLightCount);

	s_Model.Draw(*m_Camera, m_Wireframe);

#ifdef UsingimGUI
	ImGui::Checkbox("Wireframe", &m_Wireframe);
	ImGui::SliderInt("Light Count", &m_CurLightCount, 0, 3);
#endif
}
