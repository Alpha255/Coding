#include "AlphaBlend.h"
#include "D3DGraphic.h"
#include "Camera.h"

extern D3DGraphic* g_Renderer;

struct AlphaBlendResource
{
	Ref<ID3D11InputLayout> Layout;

	Ref<ID3D11Buffer> VertexBufferFloor;
	Ref<ID3D11Buffer> IndexBufferFloor;

	Ref<ID3D11Buffer> VertexBufferCube;
	Ref<ID3D11Buffer> IndexBufferCube;

	Ref<ID3D11Buffer> VertexBufferWindow;
	Ref<ID3D11Buffer> IndexBufferWindow;
	
	Ref<ID3D11VertexShader> VertexShader;
	Ref<ID3D11PixelShader> PixelShader;

	Ref<ID3D11Buffer> CBufferVS;
	Ref<ID3D11Buffer> CBufferPS;

	Ref<ID3D11ShaderResourceView> TexFloor;
	Ref<ID3D11ShaderResourceView> TexCube;
	Ref<ID3D11ShaderResourceView> TexWindow;

	Ref<ID3D11SamplerState> LinearSampler;

	Ref<ID3D11RasterizerState> NullCulling;
	Ref<ID3D11RasterizerState> BackfaceCulling;

	Ref<ID3D11BlendState> AlphaBlend;
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
	g_Renderer->CreateVertexBuffer(s_Resource.VertexBufferFloor, (uint32_t)(sizeof(Math::Geometry::Vertex) * floor.Vertices.size()), D3D11_USAGE_IMMUTABLE, &floor.Vertices[0]);
	g_Renderer->CreateIndexBuffer(s_Resource.IndexBufferFloor, (uint32_t)(sizeof(uint32_t) * floor.Indices.size()), D3D11_USAGE_IMMUTABLE, &floor.Indices[0]);

	Math::Geometry::Mesh cube;
	Math::Geometry::MakeCube(1.0f, cube);
	g_Renderer->CreateVertexBuffer(s_Resource.VertexBufferCube, (uint32_t)(sizeof(Math::Geometry::Vertex) * cube.Vertices.size()), D3D11_USAGE_IMMUTABLE, &cube.Vertices[0]);
	g_Renderer->CreateIndexBuffer(s_Resource.IndexBufferCube, (uint32_t)(sizeof(uint32_t) * cube.Indices.size()), D3D11_USAGE_IMMUTABLE, &cube.Indices[0]);

	Math::Geometry::Mesh window;
	Math::Geometry::MakeQuad(Vec3(0.0f, 0.0f, 0.0f), 1.0f, window);
	g_Renderer->CreateVertexBuffer(s_Resource.VertexBufferWindow, (uint32_t)(sizeof(Math::Geometry::Vertex) * window.Vertices.size()), D3D11_USAGE_IMMUTABLE, &window.Vertices[0]);
	g_Renderer->CreateIndexBuffer(s_Resource.IndexBufferWindow, (uint32_t)(sizeof(uint32_t) * window.Indices.size()), D3D11_USAGE_IMMUTABLE, &window.Indices[0]);

	g_Renderer->CreateShaderResourceView(s_Resource.TexCube, "toy_box_diffuse.dds");
	g_Renderer->CreateShaderResourceView(s_Resource.TexFloor, "wood.dds");
	g_Renderer->CreateShaderResourceView(s_Resource.TexWindow, "window.dds");
	g_Renderer->CreateSamplerState(s_Resource.LinearSampler, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, 0.0f, D3D11_COMPARISON_ALWAYS);

	g_Renderer->CreateConstantBuffer(s_Resource.CBufferVS, sizeof(ConstantsBufferVS), D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

	g_Renderer->CreateRasterizerState(s_Resource.NullCulling, D3D11_FILL_SOLID, D3D11_CULL_NONE);
	g_Renderer->CreateRasterizerState(s_Resource.BackfaceCulling, D3D11_FILL_SOLID, D3D11_CULL_BACK);

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_Renderer->CreateBlendState(s_Resource.AlphaBlend, &blendDesc);

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

	g_Renderer->SetVertexShader(s_Resource.VertexShader);
	g_Renderer->SetPixelShader(s_Resource.PixelShader);

	g_Renderer->SetSamplerStates(s_Resource.LinearSampler, 0U, D3DGraphic::ePixelShader);

	/// Draw floor
	g_Renderer->SetVertexBuffer(s_Resource.VertexBufferFloor, sizeof(Math::Geometry::Vertex), 0U, 0U);
	g_Renderer->SetIndexBuffer(s_Resource.IndexBufferFloor, DXGI_FORMAT_R32_UINT, 0U);
	s_CBufferVS.WVP = (m_Camera->GetWorldMatrix() * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix()).Transpose();
	g_Renderer->UpdateBuffer(s_Resource.CBufferVS, &s_CBufferVS, sizeof(ConstantsBufferVS));
	g_Renderer->SetConstantBuffer(s_Resource.CBufferVS, 0U, D3DGraphic::eVertexShader);
	g_Renderer->SetShaderResource(s_Resource.TexFloor, 0U, D3DGraphic::ePixelShader);
	g_Renderer->SetRasterizerState(s_Resource.NullCulling);
	g_Renderer->DrawIndexed(6U, 0U, 0);

	/// Draw cube0
	Matrix trans = Matrix::Translation(-2.0f, 0.0f, -0.51f);
	s_CBufferVS.WVP = (m_Camera->GetWorldMatrix() * trans * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix()).Transpose();
	g_Renderer->UpdateBuffer(s_Resource.CBufferVS, &s_CBufferVS, sizeof(ConstantsBufferVS));
	g_Renderer->SetVertexBuffer(s_Resource.VertexBufferCube, sizeof(Math::Geometry::Vertex), 0U, 0U);
	g_Renderer->SetIndexBuffer(s_Resource.IndexBufferCube, DXGI_FORMAT_R32_UINT, 0U);
	g_Renderer->SetRasterizerState(s_Resource.BackfaceCulling);
	g_Renderer->SetShaderResource(s_Resource.TexCube, 0U, D3DGraphic::ePixelShader);
	g_Renderer->DrawIndexed(36U, 0U, 0);

	/// Draw cube1
	trans = Matrix::Translation(2.0f, 1.0f, -0.51f);
	s_CBufferVS.WVP = (m_Camera->GetWorldMatrix() * trans * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix()).Transpose();
	g_Renderer->UpdateBuffer(s_Resource.CBufferVS, &s_CBufferVS, sizeof(ConstantsBufferVS));
	g_Renderer->DrawIndexed(36U, 0U, 0);

	/// Draw window0
	Matrix rotate = Matrix::RotationAxis(1.0f, 0.0f, 0.0f, 90.0f * DirectX::XM_PI / 180.0f);
	trans = Matrix::Translation(2.0f, 0.49f, -0.51f);
	s_CBufferVS.WVP = (m_Camera->GetWorldMatrix() * rotate * trans * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix()).Transpose();
	g_Renderer->UpdateBuffer(s_Resource.CBufferVS, &s_CBufferVS, sizeof(ConstantsBufferVS));
	g_Renderer->SetVertexBuffer(s_Resource.VertexBufferWindow, sizeof(Math::Geometry::Vertex), 0U, 0U);
	g_Renderer->SetIndexBuffer(s_Resource.IndexBufferWindow, DXGI_FORMAT_R32_UINT, 0U);
	g_Renderer->SetShaderResource(s_Resource.TexWindow, 0U, D3DGraphic::ePixelShader);
	g_Renderer->SetRasterizerState(s_Resource.NullCulling);
	if (m_bAlphaBlend)
	{
		g_Renderer->SetBlendState(s_Resource.AlphaBlend, Vec4(0.0f, 0.0f, 0.0f, 1.0f), 0xffffffff);
	}
	g_Renderer->DrawIndexed(6U, 0U, 0);

	/// Draw window1
	trans = Matrix::Translation(-2.0f, -0.51f, -0.51f);
	s_CBufferVS.WVP = (m_Camera->GetWorldMatrix() * rotate * trans * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix()).Transpose();
	g_Renderer->UpdateBuffer(s_Resource.CBufferVS, &s_CBufferVS, sizeof(ConstantsBufferVS));
	g_Renderer->DrawIndexed(6U, 0U, 0);

	/// Draw overlay windows
	float drawOrder[3] = { 0.8f, 0.3f, -0.2f };
	for (uint32_t i = 0U; i < 3U; ++i)
	{
		trans = Matrix::Translation(-0.5f * ((i + 1) % 2 == 0 ? 1.0f : 0.0f), drawOrder[i], -0.51f);
		s_CBufferVS.WVP = (m_Camera->GetWorldMatrix() * rotate * trans * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix()).Transpose();
		g_Renderer->UpdateBuffer(s_Resource.CBufferVS, &s_CBufferVS, sizeof(ConstantsBufferVS));
		g_Renderer->DrawIndexed(6U, 0U, 0);
	}

	ImGui::Checkbox("AlphaBlend", &m_bAlphaBlend);
}