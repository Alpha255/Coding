#include "AlphaBlend.h"
#include "D3DGraphic.h"
#include "Camera.h"

extern D3DGraphic* g_Renderer;

struct AlphaBlendResource
{
	Ref<ID3D11InputLayout> Layout;

	Ref<ID3D11Buffer> VertexBuffer;
	Ref<ID3D11Buffer> IndexBuffer;
	
	Ref<ID3D11VertexShader> VertexShader;
	Ref<ID3D11PixelShader> PixelShader;

	Ref<ID3D11Buffer> CBufferVS;
	Ref<ID3D11Buffer> CBufferPS;

	Ref<ID3D11ShaderResourceView> TexFloor;

	Ref<ID3D11SamplerState> LinearSampler;
};

struct ConstantsBufferVS
{
	Matrix WVP;
};

static AlphaBlendResource s_Resource;
static ConstantsBufferVS s_CBufferVS;

void AppAlphaBlend::SetupScene()
{
	assert(g_Renderer);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	g_Renderer->CreateVertexShaderAndInputLayout(s_Resource.VertexShader, s_Resource.Layout, layout, ARRAYSIZE(layout), "AlphaBlend.hlsl", "VSMain");
	g_Renderer->CreatePixelShader(s_Resource.PixelShader, "AlphaBlend.hlsl", "PSMain");

	Math::Geometry::Mesh floor;
	Math::Geometry::MakeQuad(Vec3(0.0f, 0.0f, 0.0f), 10.0f, floor);
	g_Renderer->CreateVertexBuffer(s_Resource.VertexBuffer, (uint32_t)(sizeof(Math::Geometry::Vertex) * floor.Vertices.size()), D3D11_USAGE_IMMUTABLE, &floor.Vertices[0]);
	g_Renderer->CreateIndexBuffer(s_Resource.IndexBuffer, (uint32_t)(sizeof(uint32_t) * floor.Indices.size()), D3D11_USAGE_IMMUTABLE, &floor.Indices[0]);

	g_Renderer->CreateShaderResourceView(s_Resource.TexFloor, "toy_box_diffuse.dds");
	g_Renderer->CreateSamplerState(s_Resource.LinearSampler, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, 0.0f, D3D11_COMPARISON_ALWAYS);

	g_Renderer->CreateConstantBuffer(s_Resource.CBufferVS, sizeof(ConstantsBufferVS), D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	D3D11_VIEWPORT vp{ 0 };
	vp.Width = (float)m_Width;
	vp.Height = (float)m_Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = vp.TopLeftY = 0.0f;
	g_Renderer->SetViewports(&vp);

	m_Camera->SetViewRadius(15.0f);
}

void AppAlphaBlend::RenderScene()
{
	g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget(), reinterpret_cast<const float*>(&Color::DarkBlue));
	g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);

	g_Renderer->SetInputLayout(s_Resource.Layout);

	g_Renderer->SetVertexBuffer(s_Resource.VertexBuffer, sizeof(Math::Geometry::Vertex), 0U, 0U);
	g_Renderer->SetIndexBuffer(s_Resource.IndexBuffer, DXGI_FORMAT_R32_UINT, 0U);

	s_CBufferVS.WVP = (m_Camera->GetWorldMatrix() * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix()).Transpose();
	g_Renderer->UpdateBuffer(s_Resource.CBufferVS, &s_CBufferVS, sizeof(ConstantsBufferVS));
	g_Renderer->SetConstantBuffer(s_Resource.CBufferVS, 0U, D3DGraphic::eVertexShader);

	g_Renderer->SetVertexShader(s_Resource.VertexShader);
	g_Renderer->SetPixelShader(s_Resource.PixelShader);

	g_Renderer->SetSamplerStates(s_Resource.LinearSampler, 0U, D3DGraphic::ePixelShader);
	g_Renderer->SetShaderResource(s_Resource.TexFloor, 0U, D3DGraphic::ePixelShader);

	g_Renderer->DrawIndexed(6U, 0U, 0);
}