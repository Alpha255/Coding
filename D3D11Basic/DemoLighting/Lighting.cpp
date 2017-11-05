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

	SimpleMesh Model;
};

static DemoLightingResource s_Resource;
static float s_Radius = 15.0f;
static float s_Phi = DirectX::XM_PI * 0.1f;
static float s_Theta = DirectX::XM_PI * 1.5f;
static Camera s_Camera;

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

	GUIAntTweakBar::WidgeDesc EnableWireframe;
	EnableWireframe.Type = GUIAntTweakBar::eCheckBox;
	EnableWireframe.Title = "Wireframe";
	EnableWireframe.GetVarFunc = GetWireframe;
	EnableWireframe.SetVarFunc = SetWireframe;
	m_GUI.AddWidget(EnableWireframe);

	m_bInited = true;
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

	s_Resource.Model.Draw(s_Camera, m_Wireframe);

	///ImGui::Checkbox("Wireframe", &m_Wireframe);
}

void ApplicationLighting::UpdateScene(float /*elapsedTime*/, float /*totalTime*/)
{
	float x = s_Radius * sinf(s_Phi) * cosf(s_Theta);
	float z = s_Radius * sinf(s_Phi) * sinf(s_Theta);
	float y = s_Radius * cosf(s_Phi);

	Vec3 eyePos(x, y, z);
	Vec3 lookAt(0.0f, 0.0f, 0.0f);
	s_Camera.SetViewParams(eyePos, lookAt);

	if (::GetAsyncKeyState(VK_NUMPAD0) & 0x8000)
	{
		m_CurLightCount = 0;
	}
	if (::GetAsyncKeyState(VK_NUMPAD1) & 0x8000)
	{
		m_CurLightCount = 1;
	}
	if (::GetAsyncKeyState(VK_NUMPAD2) & 0x8000)
	{
		m_CurLightCount = 2;
	}
	if (::GetAsyncKeyState(VK_NUMPAD3) & 0x8000)
	{
		m_CurLightCount = 3;
	}
}

void ApplicationLighting::ResizeWindow(uint32_t width, uint32_t height)
{
	s_Camera.SetProjParams(DirectX::XM_PIDIV4, (float)width / height, 1.0f, 1000.0f);

	Base::ResizeWindow(width, height);
}

void ApplicationLighting::MouseMove(WPARAM wParam, int x, int y)
{
	if ((wParam & MK_LBUTTON) != 0)
	{
		float dx = DirectX::XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos[0]));
		float dy = DirectX::XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos[1]));

		s_Theta += dx;
		s_Phi += dy;

		s_Phi = Math::Clamp(s_Phi, 0.1f, DirectX::XM_PI - 0.1f);
	}
	else if ((wParam & MK_RBUTTON) != 0)
	{
		float dx = 0.01f * static_cast<float>(x - m_LastMousePos[0]);
		float dy = 0.01f * static_cast<float>(y - m_LastMousePos[1]);

		s_Radius += dx - dy;

		s_Radius = Math::Clamp(s_Radius, 3.0f, 200.0f);
	}

	m_LastMousePos[0] = x;
	m_LastMousePos[1] = y;
}
