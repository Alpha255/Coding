#include "Lighting.h"
#include "RefCountPtr.h"
#include "D3DGraphic.h"
#include "D3DModel.h"
#include "D3DMath.h"
#include "Camera.h"

extern D3DGraphic* g_Renderer;

struct DemoLightingResource
{
	Ref<ID3D11VertexShader> VertexShader;
	Ref<ID3D11PixelShader> PixelShader;
	Ref<ID3DBlob> VSBlob;

	Ref<ID3D11Buffer> CBufferVS;
	Ref<ID3D11Buffer> CBufferPS;

	D3D11_VIEWPORT Viewport;

	SimpleMesh Model;
};

struct ConstantsBufferVS
{
	Matrix World;
	Matrix WorldInverseTrans;
	Matrix WVP;
};

struct ConstantsBufferPS
{
	Vec4 ViewPoint;

	Lighting::DirectionalLight DirLight[3];
#if 1
	Lighting::Material MaterialSkull;
#else
	Lighting::Material MaterialCar;
#endif
};

static DemoLightingResource s_Resource;
static ConstantsBufferVS s_CBufVS;
static ConstantsBufferPS s_CBufPS;
static float s_Radius = 15.0f;
static float s_Phi = DirectX::XM_PI * 0.1f;
static float s_Theta = DirectX::XM_PI * 1.5f;
static Camera s_Camera;
static char* const s_ShaderName = "Lighting.hlsl";

ApplicationLighting::ApplicationLighting()
{
	s_CBufVS.World = Matrix::Scaling(0.5f, 0.5f, 0.5f) * Matrix::Translation(0.0f, 1.0f, 0.0f);
	s_CBufVS.WorldInverseTrans = s_CBufVS.World.InverseTranspose();

	s_CBufPS.DirLight[0].Ambient = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
	s_CBufPS.DirLight[0].Diffuse = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
	s_CBufPS.DirLight[0].Specular = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
	s_CBufPS.DirLight[0].Direction = Vec4(0.57735f, -0.57735f, 0.57735f, 0.0f);

	s_CBufPS.DirLight[1].Ambient = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	s_CBufPS.DirLight[1].Diffuse = Vec4(0.20f, 0.20f, 0.20f, 1.0f);
	s_CBufPS.DirLight[1].Specular = Vec4(0.25f, 0.25f, 0.25f, 1.0f);
	s_CBufPS.DirLight[1].Direction = Vec4(-0.57735f, -0.57735f, 0.57735f, 0.0f);

	s_CBufPS.DirLight[2].Ambient = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	s_CBufPS.DirLight[2].Diffuse = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
	s_CBufPS.DirLight[2].Specular = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	s_CBufPS.DirLight[2].Direction = Vec4(0.0f, -0.707f, -0.707f, 0.0f);

	s_CBufPS.MaterialSkull.Ambient = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
	s_CBufPS.MaterialSkull.Diffuse = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
	s_CBufPS.MaterialSkull.Specular = Vec4(0.8f, 0.8f, 0.8f, 16.0f);
}

void ApplicationLighting::SetupScene()
{
	g_Renderer->CreateVertexShader(s_Resource.VertexShader.Reference(), s_Resource.VSBlob.Reference(), s_ShaderName, "VSMain");
	g_Renderer->CreatePixelShader(s_Resource.PixelShader.Reference(), s_ShaderName, "PSMain");

#if 1 
	const char *pMesh = "skull.txt";
#else
	const char *pMesh = "car.txt";
#endif
	s_Resource.Model.CreateFromTxt(pMesh, s_Resource.VSBlob.Ptr());

	g_Renderer->CreateConstantBuffer(s_Resource.CBufferVS.Reference(), sizeof(ConstantsBufferVS),
		D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);
	g_Renderer->CreateConstantBuffer(s_Resource.CBufferPS.Reference(), sizeof(ConstantsBufferPS),
		D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

	g_Renderer->SetVertexShader(s_Resource.VertexShader.Ptr());
	g_Renderer->SetPixelShader(s_Resource.PixelShader.Ptr());

	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	s_Resource.Viewport.Width = (float)m_Width;
	s_Resource.Viewport.Height = (float)m_Height;
	s_Resource.Viewport.MinDepth = 0.0f;
	s_Resource.Viewport.MaxDepth = 1.0f;
	s_Resource.Viewport.TopLeftX = s_Resource.Viewport.TopLeftY = 0.0f;
	g_Renderer->SetViewports(&s_Resource.Viewport);

	m_bInited = true;
}

void ApplicationLighting::RenderScene()
{
	g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);

	s_CBufVS.WVP = s_CBufVS.World * s_Camera.GetViewMatrix() * s_Camera.GetProjMatrix();

	g_Renderer->UpdateConstantBuffer(s_Resource.CBufferVS.Ptr(), &s_CBufVS, sizeof(ConstantsBufferVS));
	g_Renderer->UpdateConstantBuffer(s_Resource.CBufferPS.Ptr(), &s_CBufPS, sizeof(ConstantsBufferPS));

	g_Renderer->SetConstantBuffer(s_Resource.CBufferVS.Ptr(), 0U, D3DGraphic::eVertexShader);
	g_Renderer->SetConstantBuffer(s_Resource.CBufferPS.Ptr(), 0U, D3DGraphic::ePixelShader);

	s_Resource.Model.Draw();
}

void ApplicationLighting::UpdateScene(float /*elapsedTime*/, float /*totalTime*/)
{
	float x = s_Radius * sinf(s_Phi) * cosf(s_Theta);
	float z = s_Radius * sinf(s_Phi) * sinf(s_Theta);
	float y = s_Radius * cosf(s_Phi);

	s_CBufPS.ViewPoint = Vec4(x, y, z, 0.0f);

	Vec3 eyePos(x, y, z);
	Vec3 lookAt(0.0f, 0.0f, 0.0f);
	s_Camera.SetViewParams(eyePos, lookAt);
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
