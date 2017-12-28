#include "AdaptiveTessellation.h"

#include "D3DGraphic.h"
#include "D3DModel.h"
#include "Camera.h"
#include "Tessellator.h"

extern D3DGraphic* g_Renderer;

struct D3DResource
{
	Ref<ID3D11VertexShader> VertexShader;
	Ref<ID3D11VertexShader> VertexShader_Tessed;
	Ref<ID3D11PixelShader> PixelShader;

	Ref<ID3D11Buffer> VertexBuffer;
	Ref<ID3D11Buffer> IndexBuffer;

	Ref<ID3D11InputLayout> Layout;

	Ref<ID3D11Buffer> CBVS;

	Ref<ID3D11RasterizerState> WireframeNullCulling;
};

struct ConstantsBufferVS
{
	Matrix WVP;
};

static D3DResource s_Resource;
static ConstantsBufferVS s_CBVS;
static Tessellator s_Tessellator;

void AppAdaptiveTessellation::SetupScene()
{
	assert(g_Renderer);

	ObjMesh mesh;
	mesh.Create("AdaptiveTessellation.obj");
	const std::vector<Math::Geometry::BasicVertex> &vertices = mesh.GetVertices();
	const std::vector<uint32_t> &indices = mesh.GetIndices();
	m_VertexCount = (uint32_t)vertices.size();
	m_IndexCount = (uint32_t)indices.size();
	std::vector<Vec4> copyVertices(m_VertexCount);
	for (uint32_t i = 0U; i < m_VertexCount; ++i)
	{
		memcpy(&copyVertices.at(i), &vertices.at(i), sizeof(Vec3));
		copyVertices.at(i).w = 1.0f;
	}

	static char *const s_ShaderName = "AdaptiveTessellation.hlsl";

	D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  0,  D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	g_Renderer->CreateVertexShaderAndInputLayout(s_Resource.VertexShader.Reference(), s_Resource.Layout.Reference(), 
		layoutDesc, ARRAYSIZE(layoutDesc), s_ShaderName, "VS_Main");
	g_Renderer->CreateVertexShader(s_Resource.VertexShader_Tessed.Reference(), s_ShaderName, "VS_Main_Tessed");
	g_Renderer->CreatePixelShader(s_Resource.PixelShader.Reference(), s_ShaderName, "PS_Main");

	g_Renderer->CreateVertexBuffer(s_Resource.VertexBuffer.Reference(), m_VertexCount * sizeof(Vec4),
		D3D11_USAGE_IMMUTABLE, &copyVertices[0], 0U, D3D11_BIND_SHADER_RESOURCE);
	g_Renderer->CreateIndexBuffer(s_Resource.IndexBuffer.Reference(), m_IndexCount * sizeof(uint32_t),
		D3D11_USAGE_IMMUTABLE, &indices[0]);

	g_Renderer->CreateConstantBuffer(s_Resource.CBVS.Reference(), sizeof(ConstantsBufferVS), 
		D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

	g_Renderer->CreateRasterizerState(s_Resource.WireframeNullCulling.Reference(), D3D11_FILL_WIREFRAME,
		D3D11_CULL_NONE);

	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	D3D11_VIEWPORT vp{ 0 };
	vp.Width = (float)m_Width;
	vp.Height = (float)m_Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = vp.TopLeftY = 0.0f;
	g_Renderer->SetViewports(&vp);

	m_Camera->SetViewRadius(350.0f);

	s_Tessellator.Init(m_VertexCount, s_Resource.VertexBuffer.Ptr());

	m_PartitioningMode = s_Tessellator.GetPartitioningMode();
}

void AppAdaptiveTessellation::RenderScene()
{
	g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget(), reinterpret_cast<const float*>(&Color::DarkBlue));
	g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);

	Matrix wvp = m_Camera->GetWorldMatrix() * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix();
	s_CBVS.WVP = wvp.Transpose();
	g_Renderer->UpdateBuffer(s_Resource.CBVS.Ptr(), &s_CBVS, sizeof(ConstantsBufferVS));

	g_Renderer->SetConstantBuffer(s_Resource.CBVS.Ptr(), 0U, D3DGraphic::eVertexShader);
	g_Renderer->SetPixelShader(s_Resource.PixelShader.Ptr());
	g_Renderer->SetInputLayout(s_Resource.Layout.Ptr());

	g_Renderer->SetRasterizerState(s_Resource.WireframeNullCulling.Ptr());

	if (m_bEnableTess && s_Tessellator.DoTessellationByEdge(*m_Camera))
	{
		g_Renderer->SetVertexShader(s_Resource.VertexShader_Tessed.Ptr());

		g_Renderer->SetShaderResource(s_Tessellator.GetSourceVertexBufferSRV(), 0U, 1U, D3DGraphic::eVertexShader);
		g_Renderer->SetShaderResource(s_Tessellator.GetTessedVertexBufferSRV(), 1U, 1U, D3DGraphic::eVertexShader);

		g_Renderer->SetIndexBuffer(s_Tessellator.GetTessedIndexBuffer(), DXGI_FORMAT_R32_UINT, 0U);

		g_Renderer->DrawIndexed(s_Tessellator.GetTessedIndexCount(), 0U, 0, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		g_Renderer->SetShaderResource(nullptr, 0U, 1U, D3DGraphic::eVertexShader);
		g_Renderer->SetShaderResource(nullptr, 1U, 1U, D3DGraphic::eVertexShader);

		g_Renderer->SetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0U);
	}
	else
	{
		g_Renderer->SetVertexShader(s_Resource.VertexShader.Ptr());
		g_Renderer->SetVertexBuffer(s_Resource.VertexBuffer.Ptr(), sizeof(Vec4), 0U);
		g_Renderer->SetIndexBuffer(s_Resource.IndexBuffer.Ptr(), DXGI_FORMAT_R32_UINT);

		g_Renderer->DrawIndexed(m_IndexCount, 0U, 0);
	}

	ImGui::Checkbox("Enable Tessellation", &m_bEnableTess);
	ImGui::Combo("PartitioningMode", &m_PartitioningMode, "Integer\0Pow2\0FractionalOdd\0FractionalEven\0");

	s_Tessellator.SetPartitioningMode((Tessellator::ePartitioningMode)m_PartitioningMode);
}

void AppAdaptiveTessellation::ResizeWindow(uint32_t width, uint32_t height)
{
	s_Tessellator.Resize(width, height);

	Base::ResizeWindow(width, height);
}
