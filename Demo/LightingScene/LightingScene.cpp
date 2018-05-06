#include "LightingScene.h"
#include "D3DEngine.h"
#include "Camera.h"
#include "D3DGUI_imGui.h"

void AppLightingScene::Initialize()
{
	m_HouseTexDiffuse.Create("housediffuse.dds");
	m_HouseTexNormal.Create("Medieval_House_Nor.dds");
	m_HouseTexSpecular.Create("Medieval_House_Spec.dds");
	m_House.Create("house_01.obj");

	m_VertexShader.Create("LightingScene.hlsl", "VSMain");
	m_PixelShader.Create("LightingScene.hlsl", "PSMain");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(Matrix), D3DBuffer::eGpuReadCpuWrite, nullptr);

	m_Camera->SetViewRadius(500.0f);
}

void AppLightingScene::RenderScene()
{
	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);

	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);
	Matrix wvp = Matrix::Transpose(m_Camera->GetWVPMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));

	D3DEngine::Instance().BindMesh(m_House);

	D3DEngine::Instance().SetShaderResourceView(m_HouseTexDiffuse, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetShaderResourceView(m_HouseTexNormal, 1U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetShaderResourceView(m_HouseTexSpecular, 2U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);

	if (m_Wireframe)
	{
		D3DEngine::Instance().SetRasterizerState(D3DStaticState::Wireframe);
	}

	D3DEngine::Instance().DrawIndexed(m_House.IndexCount, 0U, 0, eTriangleList);

	ImGui::Checkbox("Wireframe", &m_Wireframe);
}
