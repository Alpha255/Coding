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

	Vec4 EyePos;
	Vec4 LightDir;
};

struct ConstantBufferPS
{
	Vec3 EyePos;
	float HeightScale = 0.01f;

	DirectionalLight DirLight;
	Material::RawMaterial RawMat;

	ConstantBufferPS()
	{
		DirLight.Diffuse = Vec4(1.0f, 1.0f, 1.0f, 0.0f);
		DirLight.Direction = Vec4(1.0f, 1.0f, 5.0f, 0.0f);
		DirLight.SpecularIntensity = 64.0f;
	}
};

void AppParallaxMapping::Initialize()
{
	m_Floor.CreateAsQuad(8.0f);

	m_HeightMap.Create("StoneFloor_Heightmap.dds");

	m_FloorMaterial.Set(Material::eDiffuse, "StoneFloor_Diffuse.dds");
	m_FloorMaterial.Set(Material::eSpecular, "StoneFloor_Specular.dds");
	m_FloorMaterial.Set(Material::eNormal, "StoneFloor_Normal.dds");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite);

	m_VertexShader.Create("ParallaxMapping.hlsl", "VSMain");

	m_PixelShader[eNormalMapping].Create("ParallaxMapping.hlsl", "PSMain_NormalMapping");
	m_PixelShader[eParallaxOcclusionMappingInACL].Create("ParallaxMapping.hlsl", "PSMain_ParallaxOcclusionMappingInACL");
	///m_PixelShader[eParallaxMappingWithOffsetLimit].Create("ParallaxMapping.hlsl", "PSMain_ParallaxMappingWithOffsetLimit");
	///m_PixelShader[eParallaxOcclusionMapping].Create("ParallaxMapping.hlsl", "PSMain_ParallaxOcclusionMapping");
}

void AppParallaxMapping::RenderScene()
{
	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader[m_MappingType]);

	m_Floor.Bind(&m_FloorMaterial);
	D3DEngine::Instance().SetShaderResourceView(m_HeightMap, 3U, D3DShader::ePixelShader);

	static ConstantBufferPS CBufferPS;
	Vec4 eyePos = m_Camera->GetEyePos();
	CBufferPS.EyePos = Vec3(eyePos.x, eyePos.y, eyePos.z);
	CBufferPS.RawMat = m_FloorMaterial.RawValue;
	m_CBufferPS.Update(&CBufferPS, sizeof(ConstantBufferPS));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

	ConstantBufferVS CBufferVS;
	CBufferVS.World = Matrix::Transpose(m_Camera->GetWorldMatrix());
	CBufferVS.WorldInverse = Matrix::InverseTranspose(m_Camera->GetWorldMatrix());
	CBufferVS.WVP = Matrix::Transpose(m_Camera->GetWVPMatrix());
	CBufferVS.UVTransform = Matrix::Transpose(Matrix::Scaling(1.0f));
	CBufferVS.EyePos = eyePos;
	CBufferVS.LightDir = CBufferPS.DirLight.Direction;
	m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	D3DEngine::Instance().SetRasterizerState(D3DStaticState::SolidNoneCulling);
	D3DEngine::Instance().DrawIndexed(m_Floor.IndexCount, 0U, 0, eTriangleList);

	Vec3 pos(-CBufferPS.DirLight.Direction.x, -CBufferPS.DirLight.Direction.y, -CBufferPS.DirLight.Direction.z);
	Light::DebugDisplay(pos, Light::ePoint, *m_Camera);

	if (m_MappingType > eNormalMapping)
	{
		ImGui::SliderFloat("HeightScale", &CBufferPS.HeightScale, 0.0f, 0.1f);
	}

	D3DEngine::Instance().SetVSync(m_VSync);

	ImGui::Text("%.2f FPS", m_FPS);
	ImGui::Combo("MappingType", &m_MappingType, "NormalMapping\0ParallaxOcclusionMappingInACL");
	ImGui::Checkbox("VSync", &m_VSync);
	ImGui::SliderFloat3("LightDir", (float *)&CBufferPS.DirLight.Direction, -10.0f, 10.0f);
}
