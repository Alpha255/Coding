#include "LightingBasic.h"
#include "D3DGraphic.h"
#include "Camera.h"

extern D3DGraphic* g_Renderer;

struct D3DResource
{
	Ref<ID3D11Buffer> VertexBuffer;
	Ref<ID3D11Buffer> IndexBuffer;

	Ref<ID3D11InputLayout> Layout;

	Ref<ID3D11Buffer> ConstantsBufferVS;
	Ref<ID3D11Buffer> ConstantsBufferPS;

	Ref<ID3D11VertexShader> VertexShader;
	Ref<ID3D11PixelShader> PixelShader;
};

struct CBufferVS
{
	Matrix World;
	Matrix WorldInverseTranspose;
	Matrix WVP;

	CBufferVS()
	{
		World.Identity();
		WorldInverseTranspose.Identity();
		WVP.Identity();
	}
};

struct CBufferPS
{
	Vec4 EyePos;

	Lighting::Material Mat;

	Lighting::DirectionalLight DirLight;

	CBufferPS()
	{
		EyePos = Vec4(0.0f, 0.0f, 0.0f, 0.0f);

		Mat.Ambient = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
		Mat.Diffuse = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
		Mat.Specular = Vec4(0.8f, 0.8f, 0.8f, 16.0f);
		Mat.Reflect = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

		DirLight.Ambient = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
		DirLight.Diffuse = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
		DirLight.Specular = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
		DirLight.Direction = Vec4(0.57735f, -0.57735f, 0.57735f, 0.0f);
	}
};

static D3DResource s_Resource;
static CBufferVS s_CBufferVS;
static CBufferPS s_CBufferPS;

void AppLightingBasic::SetupScene()
{
	assert(g_Renderer);

	Math::Geometry::Mesh sphere;
	Math::Geometry::MakeFlatGeoSphere(0.5f, 2U, sphere);
	m_IndexCount = (uint32_t)sphere.Indices.size();

	g_Renderer->CreateVertexBuffer(s_Resource.VertexBuffer, (uint32_t)(sizeof(Math::Geometry::Vertex) * sphere.Vertices.size()), D3D11_USAGE_IMMUTABLE, &sphere.Vertices[0]);
	g_Renderer->CreateIndexBuffer(s_Resource.IndexBuffer, (uint32_t)(sizeof(uint32_t) * sphere.Indices.size()), D3D11_USAGE_IMMUTABLE, &sphere.Indices[0]);

	g_Renderer->CreateConstantBuffer(s_Resource.ConstantsBufferVS, (uint32_t)sizeof(CBufferVS), D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);
	g_Renderer->CreateConstantBuffer(s_Resource.ConstantsBufferPS, (uint32_t)sizeof(CBufferPS), D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	g_Renderer->CreateVertexShaderAndInputLayout(s_Resource.VertexShader, s_Resource.Layout, layout, ARRAYSIZE(layout), "LightingBasic.hlsl", "VSMain");
	g_Renderer->CreatePixelShader(s_Resource.PixelShader, "LightingBasic.hlsl", "PSMain");

	m_Camera->SetViewRadius(2.5f);

	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	D3D11_VIEWPORT vp;
	vp.Width = (float)m_Width;
	vp.Height = (float)m_Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = vp.TopLeftY = 0.0f;
	g_Renderer->SetViewports(&vp);
}

void AppLightingBasic::RenderScene()
{
	g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget(), reinterpret_cast<const float*>(&Color::DarkBlue));
	g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);

	g_Renderer->SetInputLayout(s_Resource.Layout);
	g_Renderer->SetVertexBuffer(s_Resource.VertexBuffer, sizeof(Math::Geometry::Vertex), 0U);
	g_Renderer->SetIndexBuffer(s_Resource.IndexBuffer, DXGI_FORMAT_R32_UINT);

	g_Renderer->SetVertexShader(s_Resource.VertexShader);
	g_Renderer->SetPixelShader(s_Resource.PixelShader);

	Matrix world = m_Camera->GetWorldMatrix();
	s_CBufferVS.World = world.Transpose();
	s_CBufferVS.WorldInverseTranspose = world.InverseTranspose();
	s_CBufferVS.WVP = (world * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix()).Transpose();
	g_Renderer->UpdateBuffer(s_Resource.ConstantsBufferVS, &s_CBufferVS, sizeof(CBufferVS));
	g_Renderer->SetConstantBuffer(s_Resource.ConstantsBufferVS, 0U, D3DGraphic::eVertexShader);

	g_Renderer->UpdateBuffer(s_Resource.ConstantsBufferPS, &s_CBufferPS, sizeof(CBufferPS));
	g_Renderer->SetConstantBuffer(s_Resource.ConstantsBufferVS, 0U, D3DGraphic::ePixelShader);

	g_Renderer->DrawIndexed(m_IndexCount, 0U, 0);

	ImGui::Combo("ShadingType", (int32_t*)&m_ShadingMode, "Flat\0Gouraud\0Phong\0Lambert\0BlinnPhong");
}