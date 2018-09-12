#include "GammaCorrection.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "D3DGUI_imGui.h"

struct ConstantBufferVS
{
	Matrix World;
	Matrix WorldInverse;
	Matrix WVP;
};

struct ConstantBufferPS
{
	Vec4 EyePos;

	PointLight Lights[3];

	Material::RawMaterial Mat;

	ConstantBufferPS()
	{
		Lights[0].Diffuse = Vec4(0.25f, 0.25f, 0.25f, 1.0f);
		Lights[0].Position = Vec3(-3.0f, 0.0f, -0.5f);

		Lights[1].Diffuse = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
		Lights[1].Position = Vec3(0.0f, 0.0f, -0.5f);

		Lights[2].Diffuse = Vec4(0.75f, 0.75f, 0.75f, 1.0f);
		Lights[2].Position = Vec3(3.0f, 0.0f, -0.5f);
	}
}; 

void AppGammaCorrection::Initialize()
{
	m_FloorMesh.CreateAsQuad(10.0f);

	m_VertexShader.Create("GammaCorrection.hlsl", "VSMain");
	m_PixelShader.Create("GammaCorrection.hlsl", "PSMain");
	
	///m_DiffuseTexFloorGamma.Create("woodGamma.dds");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite, nullptr);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite, nullptr);

	m_FloorMaterial.Set(Material::eDiffuse, "wood.dds");
}

void AppGammaCorrection::RenderScene()
{
	ConstantBufferVS CBufferVS;
	ConstantBufferPS CBufferPS;

	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);

	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

	D3DEngine::Instance().SetRasterizerState(D3DStaticState::SolidNoneCulling);

	/// Draw Floor
	CBufferVS.World = Matrix::Transpose(m_Camera->GetWorldMatrix());
	CBufferVS.WorldInverse = Matrix::InverseTranspose(m_Camera->GetWorldMatrix());
	CBufferVS.WVP = Matrix::Transpose(m_Camera->GetWVPMatrix());
	m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));

	CBufferPS.EyePos = m_Camera->GetEyePos();
	CBufferPS.Mat = m_FloorMaterial.RawValue;
	m_CBufferPS.Update(&CBufferPS, sizeof(ConstantBufferPS));

	m_FloorMesh.Draw(&m_FloorMaterial);

	///if (m_bGammaCorrection)
	///{
	///	D3DEngine::Instance().SetShaderResourceView(m_DiffuseTexFloorGamma, 0U, D3DShader::ePixelShader);
	///}
	///else
	///{
	///	D3DEngine::Instance().SetShaderResourceView(m_DiffuseTexFloor, 0U, D3DShader::ePixelShader);
	///}

	for (uint32_t i = 0U; i < 3U; ++i)
	{
		Light::DebugDisplay(CBufferPS.Lights[i].Position, Light::ePoint, *m_Camera);
	}

	ImGui::Checkbox("GammaCorrection", &m_bGammaCorrection);
}
