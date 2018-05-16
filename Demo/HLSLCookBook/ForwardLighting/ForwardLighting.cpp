#include "ForwardLighting.h"
#include "D3DEngine.h"
#include "D3DGUI_imGui.h"
#include "Camera.h"

struct ConstantBufferVS
{
	Matrix WVP;
	Matrix WorldInverse;
};

struct ConstantBufferPS
{
	Vec4 AmbientLowerClr;
	Vec4 AmbientRange;
};

Vec4 g_AmbientLowerClr = Vec4(0.1f, 0.5f, 0.1f, 1.0f);
Vec4 g_AmbientUpperClr = Vec4(0.1f, 0.2f, 0.5f, 1.0f);
ConstantBufferPS g_CBufferPS;
ConstantBufferVS g_CBufferVS;

void AppForwardLighting::Initialize()
{
	m_StanfordBunny.Create("bun_zipper_res2.obj");

	m_VertexShader.Create("ForwardLighting.hlsl", "VSMain");
	m_PixelShader.Create("ForwardLighting.hlsl", "PSMain");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite, nullptr);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite, nullptr);

	m_Camera->SetViewRadius(10.0f);
}

void AppForwardLighting::RenderScene()
{
	if (m_Wireframe)
	{
		D3DEngine::Instance().SetRasterizerState(D3DStaticState::Wireframe);
	}

	D3DEngine::Instance().BindMesh(m_StanfordBunny);
	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);

	Matrix scale = Matrix::Scaling(15.0f);
	g_CBufferVS.WVP = Matrix::Transpose(m_Camera->GetWorldMatrix() * scale * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	g_CBufferVS.WorldInverse = Matrix::InverseTranspose(m_Camera->GetWorldMatrix());
	m_CBufferVS.Update(&g_CBufferVS, sizeof(ConstantBufferVS));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	g_CBufferPS.AmbientLowerClr = GammaToLinear(g_AmbientLowerClr);
	g_CBufferPS.AmbientRange = GammaToLinear(g_AmbientUpperClr) - GammaToLinear(g_AmbientLowerClr);
	m_CBufferPS.Update(&g_CBufferPS, sizeof(ConstantBufferPS));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

	D3DEngine::Instance().DrawIndexed(m_StanfordBunny.IndexCount, 0U, 0, eTriangleList);

	ImGui::Checkbox("Wireframe", &m_Wireframe);
	ImGui::SliderFloat3("AmbientLowerClr", (float *)&g_AmbientLowerClr, 0.0f, 1.0f);
	ImGui::SliderFloat3("AmbientUpperClr", (float *)&g_AmbientUpperClr, 0.0f, 1.0f);
	ImGui::Combo("SpecialEffect", &m_LightingType, "HemisphericAmbient");
}
