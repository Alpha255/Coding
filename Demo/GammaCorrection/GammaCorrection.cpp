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
		PointLights[0].Specular = Vec4(0.7f, 0.7f, 0.7f, 1.0f);
		PointLights[0].Position = Vec3(-3.0f, 0.0f, -1.0f);
		PointLights[0].Range = 10.0f;
		PointLights[0].Attenuation = Vec4(0.0f, 0.1f, 0.0f, 0.0f);

		PointLights[1].Ambient = Vec4(0.1f, 0.1f, 0.1f, 1.0f);
		PointLights[1].Diffuse = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
		PointLights[1].Specular = Vec4(0.7f, 0.7f, 0.7f, 1.0f);
		PointLights[1].Position = Vec3(0.0f, 0.0f, -1.0f);
		PointLights[1].Range = 10.0f;
		PointLights[1].Attenuation = Vec4(0.0f, 0.1f, 0.0f, 0.0f);

		PointLights[2].Ambient = Vec4(0.1f, 0.1f, 0.1f, 1.0f);
		PointLights[2].Diffuse = Vec4(0.75f, 0.75f, 0.75f, 1.0f);
		PointLights[2].Specular = Vec4(0.7f, 0.7f, 0.7f, 1.0f);
		PointLights[2].Position = Vec3(3.0f, 0.0f, -1.0f);
		PointLights[2].Range = 10.0f;
		PointLights[2].Attenuation = Vec4(0.0f, 0.1f, 0.0f, 0.0f);
	}
}; 

Material g_MatFloor;
Material g_MatBox;

void AppGammaCorrection::Initialize()
{
	m_FloorMesh.CreateAsQuad(20.0f);
	m_BoxMesh.CreateAsCube(1.0f);

	m_VertexShader.Create("GammaCorrection.hlsl", "VSMain");
	m_PixelShader.Create("GammaCorrection.hlsl", "PSMain");

	m_DiffuseTexFloor.Create("wood.dds");
	m_DiffuseTexBox.Create("WoodCrate01.dds");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite, nullptr);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite, nullptr);

	g_MatFloor.VertexColor = {};
	g_MatFloor.Ambient = Vec4(0.48f, 0.77f, 0.46f, 1.0f);
	g_MatFloor.Diffuse = Vec4(0.48f, 0.77f, 0.46f, 1.0f);
	g_MatFloor.Specular = Vec4(0.2f, 0.2f, 0.2f, 16.0f);
	g_MatFloor.Reflection = {};

	g_MatBox.VertexColor = {};
	g_MatBox.Ambient = Vec4(0.137f, 0.42f, 0.556f, 1.0f);
	g_MatBox.Diffuse = Vec4(0.137f, 0.42f, 0.556f, 1.0f);
	g_MatBox.Specular = Vec4(0.8f, 0.8f, 0.8f, 96.0f);
	g_MatBox.Reflection = {};
}

void AppGammaCorrection::RenderScene()
{
	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);

	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);

	ConstantBufferVS CBufferVS;
	ConstantBufferPS CBufferPS;

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
	D3DEngine::Instance().SetInputLayout(m_FloorMesh.VertexLayout);
	D3DEngine::Instance().SetVertexBuffer(m_FloorMesh.VertexBuffer, sizeof(Geometry::Vertex), 0U, 0U);
	D3DEngine::Instance().SetIndexBuffer(m_FloorMesh.IndexBuffer, eR32_UInt, 0U);
	D3DEngine::Instance().SetShaderResourceView(m_DiffuseTexFloor, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetRasterizerState(D3DStaticState::SolidNoneCulling);
	D3DEngine::Instance().DrawIndexed(m_FloorMesh.IndexCount, 0U, 0, eTriangleList);

	/// Draw Box
	Matrix translation = Matrix::Translation(0.0f, 0.0f, -0.51f);
	Matrix world = m_Camera->GetWorldMatrix() * translation;
	CBufferVS.World = Matrix::Transpose(world);
	CBufferVS.WorldInverse = Matrix::InverseTranspose(world);
	CBufferVS.WVP = Matrix::Transpose(world * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));
	CBufferPS.Mat = g_MatBox;
	m_CBufferPS.Update(&CBufferPS, sizeof(ConstantBufferPS));
	D3DEngine::Instance().SetInputLayout(m_BoxMesh.VertexLayout);
	D3DEngine::Instance().SetVertexBuffer(m_BoxMesh.VertexBuffer, sizeof(Geometry::Vertex), 0U, 0U);
	D3DEngine::Instance().SetIndexBuffer(m_BoxMesh.IndexBuffer, eR32_UInt, 0U);
	D3DEngine::Instance().SetShaderResourceView(m_DiffuseTexBox, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetRasterizerState(D3DStaticState::Solid);
	D3DEngine::Instance().DrawIndexed(m_BoxMesh.IndexCount, 0U, 0, eTriangleList);

	ImGui::Checkbox("GammaCorrection", &m_bGammaCorrection);
}
