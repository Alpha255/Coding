#include "AdaptiveTessellation.h"

#include "D3DGraphic.h"
#include "D3DModel.h"
#include "Camera.h"

extern D3DGraphic* g_Renderer;

struct D3DResource
{
	Ref<ID3D11VertexShader> VertexShader;
	Ref<ID3D11PixelShader> PixelShader;

	Ref<ID3D11Buffer> CBVS;

	Ref<ID3D11RasterizerState> WireframeNullCulling;
};

struct ConstantsBufferVS
{
	Matrix WVP;
};

static ObjMesh s_Mesh;
static D3DResource s_Resource;
static ConstantsBufferVS s_CBVS;

void AppAdaptiveTessellation::SetupScene()
{
	assert(g_Renderer && !m_bInited);

	s_Mesh.Create("AdaptiveTessellation.obj");

	static char *const s_ShaderName = "AdaptiveTessellation.hlsl";

	g_Renderer->CreateVertexShader(s_Resource.VertexShader.Reference(), s_ShaderName, "VS_Main");
	g_Renderer->CreatePixelShader(s_Resource.PixelShader.Reference(), s_ShaderName, "PS_Main");

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

	m_bInited = true;
}

void AppAdaptiveTessellation::RenderScene()
{
	g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget(), reinterpret_cast<const float*>(&Color::DarkBlue));
	g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);

	Matrix wvp = m_Camera->GetWorldMatrix() * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix();
	s_CBVS.WVP = wvp.Transpose();
	g_Renderer->UpdateBuffer(s_Resource.CBVS.Ptr(), &s_CBVS, sizeof(ConstantsBufferVS));

	g_Renderer->SetConstantBuffer(s_Resource.CBVS.Ptr(), 0U, D3DGraphic::eVertexShader);
	g_Renderer->SetVertexShader(s_Resource.VertexShader.Ptr());
	g_Renderer->SetPixelShader(s_Resource.PixelShader.Ptr());
	g_Renderer->SetInputLayout(s_Mesh.GetInputLayout());
	g_Renderer->SetVertexBuffer(s_Mesh.GetVertexBuffer(), sizeof(Math::Geometry::BasicVertex), 0U);
	g_Renderer->SetIndexBuffer(s_Mesh.GetIndexBuffer(), DXGI_FORMAT_R32_UINT);
	g_Renderer->SetRasterizerState(s_Resource.WireframeNullCulling.Ptr());

	g_Renderer->DrawIndexed(s_Mesh.GetIndexCount(), 0U, 0);
}