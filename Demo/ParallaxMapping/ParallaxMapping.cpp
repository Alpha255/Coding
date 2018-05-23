#include "ParallaxMapping.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "D3DGUI_imGui.h"
#include "D3DLighting.h"

/// https://cgcookie.com/2017/06/15/normal-vs-displacement-mapping-why-games-use-normals/
/// https://msdn.microsoft.com/en-us/library/windows/desktop/ff476340(v=vs.85).aspx

struct ConstantBufferVS
{
	Matrix World;
	Matrix WorldInverse;
	Matrix WVP;
	Matrix UVTransform;
};

struct ConstantBufferPS
{
	Vec4 EyePos;

	DirectionalLight DirLight;
	Material Mat;

	ConstantBufferPS()
	{
		DirLight.Direction = Vec4(0.7f, 0.0f, -3.0f, 0.0f);
	}
};

void AppParallaxMapping::Initialize()
{
	m_QuadMesh.CreateAsQuad(8.0f);

	m_DiffuseMap.Create("StoneFloor_Diffuse.dds");
	m_NormalMap.Create("StoneFloor_Normal.dds");
	m_HeightMap.Create("StoneFloor_Heightmap.dds");
	m_SpecularMap.Create("StoneFloor_Specular.dds");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite, nullptr);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite, nullptr);

	for (uint32_t i = eNone; i <= eParallaxOcclusionMapping; ++i)
	{
		m_VertexShader[i].Create("ParallaxMapping.hlsl", "VSMain");
	}

	m_PixelShader[eNone].Create("ParallaxMapping.hlsl", "PSMain");
	m_PixelShader[eNormalMapping].Create("ParallaxMapping.hlsl", "PSMain_NormalMapping");
}

void AppParallaxMapping::RenderScene()
{
	D3DEngine::Instance().SetVertexShader(m_VertexShader[m_MappingType]);
	D3DEngine::Instance().SetPixelShader(m_PixelShader[m_MappingType]);

	D3DEngine::Instance().SetInputLayout(m_QuadMesh.VertexLayout);
	D3DEngine::Instance().SetVertexBuffer(m_QuadMesh.VertexBuffer, sizeof(Geometry::Vertex), 0U, 0U);
	D3DEngine::Instance().SetIndexBuffer(m_QuadMesh.IndexBuffer, eR32_UInt, 0U);

	ConstantBufferVS CBufferVS;
	CBufferVS.World = Matrix::Transpose(m_Camera->GetWorldMatrix());
	CBufferVS.WorldInverse = Matrix::InverseTranspose(m_Camera->GetWorldMatrix());
	CBufferVS.WVP = Matrix::Transpose(m_Camera->GetWVPMatrix());
	CBufferVS.UVTransform = Matrix::Transpose(Matrix::Scaling(1.0f));
	m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	static ConstantBufferPS CBufferPS;
	CBufferPS.EyePos = m_Camera->GetEyePos();
	m_CBufferPS.Update(&CBufferPS, sizeof(ConstantBufferPS));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetShaderResourceView(m_DiffuseMap, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetShaderResourceView(m_NormalMap, 1U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetShaderResourceView(m_SpecularMap, 2U, D3DShader::ePixelShader);

	D3DEngine::Instance().SetRasterizerState(D3DStaticState::SolidNoneCulling);
	D3DEngine::Instance().DrawIndexed(m_QuadMesh.IndexCount, 0U, 0, eTriangleList);

	ImGui::SliderFloat3("LightDirection", (float*)&CBufferPS.DirLight.Direction, -10.0f, 10.0f);
	ImGui::Combo("MappingType", &m_MappingType, "None\0NormalMapping");
}
