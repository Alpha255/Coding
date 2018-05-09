#include "LightingScene.h"
#include "D3DEngine.h"
#include "Camera.h"
#include "D3DGUI_imGui.h"

void AppLightingScene::Initialize()
{
	m_HouseTexDiffuse.Create("Medieval_House_Diff.dds");
	m_HouseTexNormal.Create("Medieval_House_Nor.dds");
	m_HouseTexSpecular.Create("Medieval_House_Spec.dds");
	m_House.Create("Medieval_House.obj");

	m_LampTexDiffuse.Create("Old_lantern_UV_Diffuse.dds");
	m_LampTexNormal.Create("Old_lantern_UV_Normal.dds");
	m_LampTexSpecular.Create("Old_lantern_UV_SPecular.dds");
	m_Lamp.Create("Gamelantern_updated.obj");

	m_Ground.CreateAsQuad(1000.0f);
	m_GroundTexDiffuse.Create("StoneFloor_Diffuse.dds");
	m_GroundTexNormal.Create("StoneFloor_Normal.dds");
	m_GroundTexSpecular.Create("StoneFloor_Specular.dds");

	m_VertexShader.Create("LightingScene.hlsl", "VSMain");
	m_PixelShader.Create("LightingScene.hlsl", "PSMain");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(Matrix), D3DBuffer::eGpuReadCpuWrite, nullptr);

	m_Camera->SetViewRadius(700.0f);
}

void AppLightingScene::RenderScene()
{
	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);

	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);
	Matrix wvp = Matrix::Transpose(m_Camera->GetWVPMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));

	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);

	if (m_Wireframe)
	{
		D3DEngine::Instance().SetRasterizerState(D3DStaticState::Wireframe);
	}

	D3DEngine::Instance().BindMesh(m_House);
	D3DEngine::Instance().SetShaderResourceView(m_HouseTexDiffuse, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetShaderResourceView(m_HouseTexNormal, 1U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetShaderResourceView(m_HouseTexSpecular, 2U, D3DShader::ePixelShader);
	D3DEngine::Instance().DrawIndexed(m_House.IndexCount, 0U, 0, eTriangleList);

	Matrix scale = Matrix::Scaling(70.0f);
	Matrix translation = Matrix::Translation(-39.0f, 125.0f, -150.0f);
	Matrix rotate = Matrix::RotationAxis(0.0f, 1.0f, 0.0f, -170.0f);
	wvp = Matrix::Transpose(m_Camera->GetWorldMatrix() * scale * rotate * translation * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));
	D3DEngine::Instance().BindMesh(m_Lamp);
	D3DEngine::Instance().SetShaderResourceView(m_LampTexDiffuse, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetShaderResourceView(m_LampTexNormal, 1U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetShaderResourceView(m_LampTexSpecular, 2U, D3DShader::ePixelShader);
	D3DEngine::Instance().DrawIndexed(m_Lamp.IndexCount, 0U, 0, eTriangleList);

	///translation = Matrix::Translation(-39.0f, 125.0f, -150.0f);
	rotate = Matrix::RotationAxis(1.0f, 0.0f, 0.0f, 90.0f);
	wvp = Matrix::Transpose(m_Camera->GetWorldMatrix() * rotate * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));
	D3DEngine::Instance().BindMesh(m_Ground);
	D3DEngine::Instance().SetShaderResourceView(m_GroundTexDiffuse, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetShaderResourceView(m_GroundTexNormal, 1U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetShaderResourceView(m_GroundTexSpecular, 2U, D3DShader::ePixelShader);
	D3DEngine::Instance().DrawIndexed(m_Ground.IndexCount, 0U, 0, eTriangleList);

	ImGui::Checkbox("Wireframe", &m_Wireframe);
}
