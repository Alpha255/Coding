#include "LightingBasic.h"
#include "D3DGraphic.h"
#include "Camera.h"

extern D3DGraphic* g_Renderer;

struct D3DResource
{
	Ref<ID3D11Buffer> VertexBuffer;
	Ref<ID3D11Buffer> IndexBuffer;

	Ref<ID3D11InputLayout> Layout;

	Ref<ID3D11Buffer> ConstantsBuffer;

	Ref<ID3D11VertexShader> VertexShader;
	Ref<ID3D11PixelShader> PixelShader;
};

struct ConstantsBufferVS
{
	Matrix World;
	Matrix WorldTranspose;
	Matrix WVP;
};

struct ConstantsBufferPS
{
	Vec4 Eye;

	Lighting::Material Mat;

	Lighting::DirectionalLight DirLight;
};

static D3DResource s_Resource;

void AppLightingBasic::SetupScene()
{
	assert(g_Renderer);

	Math::Geometry::Mesh sphere;
	Math::Geometry::MakeFlatGeoSphere(0.5f, 2U, sphere);
	m_IndexCount = (uint32_t)sphere.Indices.size();

	g_Renderer->CreateVertexBuffer(s_Resource.VertexBuffer, (uint32_t)(sizeof(Math::Geometry::Vertex) * sphere.Vertices.size()), D3D11_USAGE_IMMUTABLE, &sphere.Vertices[0]);
	g_Renderer->CreateIndexBuffer(s_Resource.IndexBuffer, (uint32_t)(sizeof(uint32_t) * sphere.Indices.size()), D3D11_USAGE_IMMUTABLE, &sphere.Indices[0]);

	g_Renderer->CreateConstantBuffer(s_Resource.ConstantsBuffer, (uint32_t)sizeof(Matrix), D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

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
}

void AppLightingBasic::RenderScene()
{
	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget(), reinterpret_cast<const float*>(&Color::DarkBlue));
	g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);

	g_Renderer->SetInputLayout(s_Resource.Layout);
	g_Renderer->SetVertexBuffer(s_Resource.VertexBuffer, sizeof(Math::Geometry::Vertex), 0U);
	g_Renderer->SetIndexBuffer(s_Resource.IndexBuffer, DXGI_FORMAT_R32_UINT);

	g_Renderer->SetVertexShader(s_Resource.VertexShader);
	g_Renderer->SetPixelShader(s_Resource.PixelShader);

	Matrix wvp = m_Camera->GetWorldMatrix() * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix();
	wvp = wvp.Transpose();
	g_Renderer->UpdateBuffer(s_Resource.ConstantsBuffer, &wvp, sizeof(Matrix));
	g_Renderer->SetConstantBuffer(s_Resource.ConstantsBuffer, 0U, D3DGraphic::eVertexShader);

	g_Renderer->DrawIndexed(m_IndexCount, 0U, 0);

	ImGui::Combo("MappingType", (int32_t*)&m_ShadingMode, "Flat\0Gouraud\0Phong\0Lambert\0BlinnPhong");
}