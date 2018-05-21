#include "GammaCorrection.h"
#include "Camera.h"
#include "D3DEngine.h"
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
	PointLight PointLights[3] = {};

	Material Mat = {};

	Vec3 EyePos = {};
	uint32_t EnableGammaCorrection = 0U;

	ConstantBufferPS()
	{
		PointLights[0].Ambient = Vec4(0.1f, 0.1f, 0.1f, 1.0f);
		PointLights[0].Diffuse = Vec4(0.25f, 0.25f, 0.25f, 1.0f);
		PointLights[0].Specular = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
		PointLights[0].Position = Vec3(-3.0f, 0.0f, -0.5f);
		PointLights[0].Range = 1.5f;
		PointLights[0].Attenuation = Vec4(0.0f, 0.0f, 0.0f, 0.0f);

		PointLights[1].Ambient = Vec4(0.1f, 0.1f, 0.1f, 1.0f);
		PointLights[1].Diffuse = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
		PointLights[1].Specular = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
		PointLights[1].Position = Vec3(0.0f, 0.0f, -0.5f);
		PointLights[1].Range = 1.5f;
		PointLights[1].Attenuation = Vec4(0.0f, 0.0f, 0.0f, 0.0f);

		PointLights[2].Ambient = Vec4(0.1f, 0.1f, 0.1f, 1.0f);
		PointLights[2].Diffuse = Vec4(0.75f, 0.75f, 0.75f, 1.0f);
		PointLights[2].Specular = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
		PointLights[2].Position = Vec3(3.0f, 0.0f, -0.5f);
		PointLights[2].Range = 1.5f;
		PointLights[2].Attenuation = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}
}; 

Material g_MatFloor;

void AppGammaCorrection::Initialize()
{
	m_FloorMesh.CreateAsQuad(20.0f);

	m_VertexShader.Create("GammaCorrection.hlsl", "VSMain");
	m_PixelShader.Create("GammaCorrection.hlsl", "PSMain");

	m_DiffuseTexFloor.Create("wood.dds");
	m_DiffuseTexFloorGamma.Create("woodGamma.dds");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite, nullptr);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite, nullptr);

	g_MatFloor.Ambient = Vec4(0.1f, 0.1f, 0.1f, 1.0f);
	g_MatFloor.Diffuse = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	g_MatFloor.Specular = Vec4(0.0f, 0.0f, 0.0f, 64.0f);
	g_MatFloor.Reflection = {};
}

void AppGammaCorrection::RenderScene()
{
	ConstantBufferVS CBufferVS;
	ConstantBufferPS CBufferPS;

	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);

	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);

	/// Draw Floor
	CBufferVS.World = Matrix::Transpose(m_Camera->GetWorldMatrix());
	CBufferVS.WorldInverse = Matrix::InverseTranspose(m_Camera->GetWorldMatrix());
	CBufferVS.WVP = Matrix::Transpose(m_Camera->GetWVPMatrix());
	m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));

	CBufferPS.EnableGammaCorrection = m_bGammaCorrection ? 1U : 0U;
	Vec4 eyePos = m_Camera->GetEyePos();
	CBufferPS.EyePos = { eyePos.x, eyePos.y, eyePos.z };
	CBufferPS.Mat = g_MatFloor;
	m_CBufferPS.Update(&CBufferPS, sizeof(ConstantBufferPS));

	D3DEngine::Instance().BindMesh(m_FloorMesh);

	if (m_bGammaCorrection)
	{
		D3DEngine::Instance().SetShaderResourceView(m_DiffuseTexFloorGamma, 0U, D3DShader::ePixelShader);
	}
	else
	{
		D3DEngine::Instance().SetShaderResourceView(m_DiffuseTexFloor, 0U, D3DShader::ePixelShader);
	}
	D3DEngine::Instance().SetRasterizerState(D3DStaticState::SolidNoneCulling);
	D3DEngine::Instance().DrawIndexed(m_FloorMesh.IndexCount, 0U, 0, eTriangleList);

	for (uint32_t i = 0U; i < 3U; ++i)
	{
		Light::DebugDisplay(CBufferPS.PointLights[i].Position, Light::ePoint, *m_Camera);
	}

	ImGui::Checkbox("GammaCorrection", &m_bGammaCorrection);
}
