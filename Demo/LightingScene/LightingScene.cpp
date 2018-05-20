#include "LightingScene.h"
#include "D3DEngine.h"
#include "Camera.h"
#include "D3DGUI_imGui.h"
#include "D3DLighting.h"

struct ConstantBufferVS
{
	Matrix World;
	Matrix WorldInverse;
	Matrix WVP;
};

struct ConstantBufferPS
{
	Vec4 EyePos;
	PointLight Light;

	ConstantBufferPS()
	{
		Light.Ambient = Vec4(0.1f, 0.1f, 0.1f, 1.0f);
		Light.Diffuse = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
		Light.Specular = Vec4(0.7f, 0.7f, 0.7f, 64.0f);
		Light.Range = 300.0f;
	}
};

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

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite, nullptr);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite);

	m_Camera->SetViewRadius(700.0f);
}

void AppLightingScene::RenderScene()
{
	ConstantBufferVS CBufferVS;
	ConstantBufferPS CBufferPS;
	static Vec3 pos(-39.0f, 125.0f, -150.0f);

	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);

	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);

	CBufferPS.EyePos = m_Camera->GetEyePos();
	CBufferPS.Light.Position = pos;
	m_CBufferPS.Update(&CBufferPS, sizeof(ConstantBufferPS));

	if (m_Wireframe)
	{
		D3DEngine::Instance().SetRasterizerState(D3DStaticState::Wireframe);
	}

	CBufferVS.World = Matrix::Transpose(m_Camera->GetWorldMatrix());
	CBufferVS.WorldInverse = Matrix::InverseTranspose(m_Camera->GetWorldMatrix());
	CBufferVS.WVP = Matrix::Transpose(m_Camera->GetWVPMatrix());
	m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));
	D3DEngine::Instance().BindMesh(m_House);
	D3DEngine::Instance().SetShaderResourceView(m_HouseTexDiffuse, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetShaderResourceView(m_HouseTexNormal, 1U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetShaderResourceView(m_HouseTexSpecular, 2U, D3DShader::ePixelShader);
	D3DEngine::Instance().DrawIndexed(m_House.IndexCount, 0U, 0, eTriangleList);

	Matrix scale = Matrix::Scaling(70.0f);
	Matrix translation = Matrix::Translation(-39.0f, 125.0f, -150.0f);
	Matrix rotate = Matrix::RotationAxis(0.0f, 1.0f, 0.0f, -170.0f);
	Matrix world = m_Camera->GetWorldMatrix() * scale * rotate * translation;
	CBufferVS.World = Matrix::Transpose(world);
	CBufferVS.WorldInverse = Matrix::InverseTranspose(world);
	CBufferVS.WVP = Matrix::Transpose(world * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));
	D3DEngine::Instance().BindMesh(m_Lamp);
	D3DEngine::Instance().SetShaderResourceView(m_LampTexDiffuse, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetShaderResourceView(m_LampTexNormal, 1U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetShaderResourceView(m_LampTexSpecular, 2U, D3DShader::ePixelShader);
	D3DEngine::Instance().DrawIndexed(m_Lamp.IndexCount, 0U, 0, eTriangleList);

	///translation = Matrix::Translation(-39.0f, 125.0f, -150.0f);
	rotate = Matrix::RotationAxis(1.0f, 0.0f, 0.0f, 90.0f);
	world = m_Camera->GetWorldMatrix() * rotate;
	CBufferVS.World = Matrix::Transpose(world);
	CBufferVS.WorldInverse = Matrix::InverseTranspose(world);
	CBufferVS.WVP = Matrix::Transpose(world * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));
	D3DEngine::Instance().BindMesh(m_Ground);
	D3DEngine::Instance().SetShaderResourceView(m_GroundTexDiffuse, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetShaderResourceView(m_GroundTexNormal, 1U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetShaderResourceView(m_GroundTexSpecular, 2U, D3DShader::ePixelShader);
	D3DEngine::Instance().DrawIndexed(m_Ground.IndexCount, 0U, 0, eTriangleList);

	Light::DebugDisplay(pos, Light::ePoint, *m_Camera, 10.0f);

	ImGui::Checkbox("Wireframe", &m_Wireframe);
	ImGui::SliderFloat3("LightPos", (float *)&pos, -300.0f, 300.0f);
}
