#include "Cubemap.h"

#include "D3DGraphic.h"
#include "Camera.h"
#include "SkyBox.h"

extern D3DGraphic* g_Renderer;

struct DemoCubeResource
{
	SkyBox Sky;
	Ref<ID3D11RasterizerState> NoBackFaceCulling;
};

static DemoCubeResource s_Resource;
static Camera s_CubemapCamera;
//static float s_Radius = 5.0f;
//static float s_Phi = DirectX::XM_PIDIV4;
//static float s_Theta = 1.5f * DirectX::XM_PI;

void ApplicationCubemap::SetupScene()
{
	//Math::Geometry::Mesh box;
	//Math::Geometry::MakeBox(1.0f, 1.0f, 1.0f, box);

	//Math::Geometry::Mesh grid;
	//Math::Geometry::MakeGrid(20.0f, 30.0f, 60U, 40U, grid);

	//Math::Geometry::Mesh sphere;
	//Math::Geometry::MakeSphere(0.5f, 20U, 20U, sphere);

	//Math::Geometry::Mesh cylinder;
	//Math::Geometry::MakeCylinder(0.5f, 0.3f, 3.0f, 20U, 20U, cylinder);

	s_Resource.Sky.Create("sunsetcube1024.dds", 100.0f);

	s_CubemapCamera.Translation(0.0f, 2.0f, -15.0f);

	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	g_Renderer->CreateRasterizerState(s_Resource.NoBackFaceCulling.Reference(), D3D11_FILL_SOLID, D3D11_CULL_NONE);

	D3D11_VIEWPORT viewport;
	viewport.Width = (float)m_Width;
	viewport.Height = (float)m_Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = viewport.TopLeftY = 0.0f;
	g_Renderer->SetViewports(&viewport);

	m_bInited = true;
}

void ApplicationCubemap::RenderScene()
{
	g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);

	g_Renderer->SetRasterizerState(s_Resource.NoBackFaceCulling.Ptr());

	s_Resource.Sky.Draw(s_CubemapCamera);
}

void ApplicationCubemap::UpdateScene(float /*elapsedTime*/, float /*totalTime*/)
{
}

void ApplicationCubemap::ResizeWindow(uint32_t width, uint32_t height)
{
	s_CubemapCamera.SetProjParams(DirectX::XM_PIDIV4, (float)width / height, 1.0f, 1000.0f);

	Base::ResizeWindow(width, height);
}

void ApplicationCubemap::MouseMove(WPARAM wParam, int x, int y)
{
	//float x = s_Radius * sinf(s_Phi) * cosf(s_Theta);
	//float z = s_Radius * sinf(s_Phi) * sinf(s_Theta);
	//float y = s_Radius * cosf(s_Phi);

	//Vec3 eyePos(x, y, z);
	//Vec3 lookAt(0.0f, 0.0f, 0.0f);

	//s_CubemapCamera.SetViewParams(eyePos, lookAt);
	if ((wParam & MK_LBUTTON) != 0)
	{
		float dx = DirectX::XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos[0]));
		float dy = DirectX::XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos[1]));

		s_CubemapCamera.Rotate(1.0f, 0.0f, 0.0f, dy);
		s_CubemapCamera.Rotate(0.0f, 1.0f, 0.0f, dx);
	}
}