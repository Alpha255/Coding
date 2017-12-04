#include "DisplacementMap.h"

#include <D3DGraphic.h>
#include <Camera.h>

extern D3DGraphic* g_Renderer;

struct ConstantsBuffer
{
	Matrix WVP;
	Matrix VP;
	Matrix World;
	Matrix WorldInverse;

	Vec3 EyePos;
	float HeightScale = 0.1f;

	float MinTessFactor = 1.0f;
	float MaxTessFactor = 5.0f;
	float MinTessDistance = 1.0f;
	float MaxTessDistance = 25.0f;
};

struct DemoDisplacementResource
{
	Ref<ID3D11InputLayout> InputLayout;

	Ref<ID3D11VertexShader> VertexShader;
	Ref<ID3D11HullShader> HullShader;
	Ref<ID3D11DomainShader> DomainShader;
	Ref<ID3D11PixelShader> PixelShader;

	Ref<ID3D11Buffer> ConstantsBuf;

	Ref<ID3D11Buffer> VertexBuffer;
	Ref<ID3D11Buffer> IndexBuffer;

	Ref<ID3D11RasterizerState> Wireframe;

	Ref<ID3D11ShaderResourceView> DiffuseTex;
	///Ref<ID3D11ShaderResourceView> NormalTex;
	Ref<ID3D11ShaderResourceView> HeightMapTex;

	Ref<ID3D11SamplerState> SamplerLinear;
};

static float s_Radius = 5.0f;
static float s_Phi = DirectX::XM_PIDIV4;
static float s_Theta = 1.5f * DirectX::XM_PI;
static DemoDisplacementResource s_Resource;
static Camera s_Camera;
static ConstantsBuffer s_CB;

void AppDisplacement::SetupScene()
{
	static char *const s_ShaderName = "DisplacementMap.hlsl";

	D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	g_Renderer->CreateVertexShaderAndInputLayout(s_Resource.VertexShader.Reference(), s_Resource.InputLayout.Reference(),
		layout, ARRAYSIZE(layout), s_ShaderName, "VS_Main");
	g_Renderer->CreateHullShader(s_Resource.HullShader.Reference(), s_ShaderName, "HS_Main");
	g_Renderer->CreateDomainShader(s_Resource.DomainShader.Reference(), s_ShaderName, "DS_Main");
	g_Renderer->CreatePixelShader(s_Resource.PixelShader.Reference(), s_ShaderName, "PS_Main");


	Math::Geometry::Mesh quad;
	Math::Geometry::MakeQuad(quad);
	g_Renderer->CreateVertexBuffer(s_Resource.VertexBuffer.Reference(), (uint32_t)(sizeof(Math::Geometry::Vertex) * quad.Vertices.size()),
		D3D11_USAGE_IMMUTABLE, &quad.Vertices[0]);
	g_Renderer->CreateIndexBuffer(s_Resource.IndexBuffer.Reference(), (uint32_t)(sizeof(uint32_t) * quad.Indices.size()),
		D3D11_USAGE_IMMUTABLE, &quad.Indices[0]);

	g_Renderer->CreateConstantBuffer(s_Resource.ConstantsBuf.Reference(), sizeof(ConstantsBuffer),
		D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

	g_Renderer->CreateRasterizerState(s_Resource.Wireframe.Reference(), D3D11_FILL_WIREFRAME);

	g_Renderer->CreateShaderResourceView(s_Resource.DiffuseTex.Reference(), "wall_diffuse.dds");
	///g_Renderer->CreateShaderResourceView(s_Resource.NormalTex.Reference(), "wall_normal.dds");
	g_Renderer->CreateShaderResourceView(s_Resource.HeightMapTex.Reference(), "wall_disp.dds");

	D3D11_SAMPLER_DESC sampDesc;
	memset(&sampDesc, 0, sizeof(D3D11_SAMPLER_DESC));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0.0f;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	g_Renderer->CreateSamplerState(s_Resource.SamplerLinear.Reference(), &sampDesc);

	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	D3D11_VIEWPORT vp{ 0 };
	vp.Width = (float)m_Width;
	vp.Height = (float)m_Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = vp.TopLeftY = 0.0f;
	g_Renderer->SetViewports(&vp);

	m_bInited = true;
}

void AppDisplacement::RenderScene()
{
	g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);

	Matrix world = s_Camera.GetWorldMatrix();
	Matrix view = s_Camera.GetViewMatrix();
    Matrix proj = s_Camera.GetProjMatrix();
	Vec4 eyePos = s_Camera.GetEyePos();

	s_CB.WVP = (world * view * proj).Transpose();
	s_CB.VP = (view * proj).Transpose();
	s_CB.World = world.Transpose();
	s_CB.WorldInverse = s_CB.World.Inverse();
	s_CB.EyePos = Vec3(eyePos.x, eyePos.y, eyePos.z);

	g_Renderer->UpdateBuffer(s_Resource.ConstantsBuf.Ptr(), &s_CB, sizeof(ConstantsBuffer));

	g_Renderer->SetVertexBuffer(s_Resource.VertexBuffer.Ptr(), sizeof(Math::Geometry::Vertex), 0U);
	g_Renderer->SetIndexBuffer(s_Resource.IndexBuffer.Ptr(), DXGI_FORMAT_R32_UINT);
	
	g_Renderer->SetInputLayout(s_Resource.InputLayout.Ptr());
	g_Renderer->SetVertexShader(s_Resource.VertexShader.Ptr());
	g_Renderer->SetHullShader(s_Resource.HullShader.Ptr());
	g_Renderer->SetDomainShader(s_Resource.DomainShader.Ptr());
	g_Renderer->SetPixelShader(s_Resource.PixelShader.Ptr());
	
	g_Renderer->SetConstantBuffer(s_Resource.ConstantsBuf.Ptr(), 0U, D3DGraphic::eVertexShader);
	g_Renderer->SetConstantBuffer(s_Resource.ConstantsBuf.Ptr(), 0U, D3DGraphic::eDomainShader);
	
	g_Renderer->SetSamplerStates(s_Resource.SamplerLinear.Ptr(), 0U, 1U, D3DGraphic::eDomainShader);
	g_Renderer->SetSamplerStates(s_Resource.SamplerLinear.Ptr(), 0U, 1U, D3DGraphic::ePixelShader);

	g_Renderer->SetShaderResource(s_Resource.DiffuseTex.Ptr(), 0U, 1U, D3DGraphic::ePixelShader);
	g_Renderer->SetShaderResource(s_Resource.HeightMapTex.Ptr(), 0U, 1U, D3DGraphic::eDomainShader);

	if (m_bWireframe)
	{
		g_Renderer->SetRasterizerState(s_Resource.Wireframe.Ptr());
	}

	g_Renderer->DrawIndexed(6U, 0U, 0, D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	/// Draw GUI
	g_Renderer->SetHullShader(nullptr);
	g_Renderer->SetDomainShader(nullptr);
	ImGui::Checkbox("Wireframe", &m_bWireframe);
	ImGui::SliderFloat("HeightScale", &s_CB.HeightScale, 0.1f, 1.0f);
	ImGui::SliderFloat("MinTessFactor", &s_CB.MinTessFactor, 1.0f, 10.0f);
	ImGui::SliderFloat("MaxTessFactor", &s_CB.MaxTessFactor, 1.0f, 10.0f);
}

void AppDisplacement::UpdateScene(float /*elapsedTime*/, float /*totalTime*/)
{
	float x = s_Radius * sinf(s_Phi) * cosf(s_Theta);
	float z = s_Radius * sinf(s_Phi) * sinf(s_Theta);
	float y = s_Radius * cosf(s_Phi);

	Vec3 eyePos(x, y, z);
	Vec3 lookAt(0.0f, 0.0f, 0.0f);

	s_Camera.SetViewParams(eyePos, lookAt);
}

void AppDisplacement::ResizeWindow(uint32_t width, uint32_t height)
{
	s_Camera.SetProjParams(DirectX::XM_PIDIV4, (float)width / height, 1.0f, 100.0f);

	return Base::ResizeWindow(width, height);
}

void AppDisplacement::MouseMove(WPARAM wParam, int x, int y)
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
		float dx = 0.005f * static_cast<float>(x - m_LastMousePos[0]);
		float dy = 0.005f * static_cast<float>(y - m_LastMousePos[1]);

		s_Radius += dx - dy;

		s_Radius = Math::Clamp(s_Radius, 3.0f, 15.0f);
	}

	m_LastMousePos[0] = x;
	m_LastMousePos[1] = y;
}