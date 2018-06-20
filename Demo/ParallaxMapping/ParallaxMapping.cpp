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

ConstantBufferVS g_CBufferVS;
ConstantBufferPS g_CBufferPS;

void AppParallaxMapping::Initialize()
{
	m_Floor.CreateAsQuad(8.0f);

	///m_HeightMap.Create("StoneFloor_Heightmap.dds");
	m_HeightMap.Create("bricks2_disp.dds");

	m_FloorMaterial.Set(Material::eDiffuse, "bricks2.dds");
	m_FloorMaterial.Set(Material::eSpecular, Vec4(1.0f, 1.0f, 1.0f, 0.0f));
	m_FloorMaterial.Set(Material::eNormal, "bricks2_normal.dds");
	///m_FloorMaterial.Set(Material::eDiffuse, "StoneFloor_Diffuse.dds");
	///m_FloorMaterial.Set(Material::eSpecular, "StoneFloor_Specular.dds");
	///m_FloorMaterial.Set(Material::eNormal, "StoneFloor_Normal.dds");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite);

	m_VertexShader.Create("ParallaxMapping.hlsl", "VSMain");

	m_PixelShader[eNormalMapping].Create("ParallaxMapping.hlsl", "PSMain_NormalMapping");
	m_PixelShader[ePOMLearningOpenGL].Create("ParallaxMapping.hlsl", "PSMain_POMLearningOpenGL");
	m_PixelShader[eParallaxOcclusionMappingInACL].Create("ParallaxMapping.hlsl", "PSMain_ParallaxOcclusionMappingInACL");
	m_PixelShader[eParallaxMappingWithOffsetLimit].Create("ParallaxMapping.hlsl", "PSMain_ParallaxMappingWithOffsetLimit");
	m_PixelShader[eParallaxOcclusionMapping].Create("ParallaxMapping.hlsl", "PSMain_ParallaxOcclusionMapping");
}

void AppParallaxMapping::RenderScene()
{
	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader[m_MappingType]);

	m_Floor.Bind(&m_FloorMaterial);
	D3DEngine::Instance().SetShaderResourceView(m_HeightMap, 3U, D3DShader::ePixelShader);

	Vec4 eyePos = m_Camera->GetEyePos();
	g_CBufferPS.EyePos = Vec3(eyePos.x, eyePos.y, eyePos.z);
	g_CBufferPS.RawMat = m_FloorMaterial.RawValue;
	m_CBufferPS.Update(&g_CBufferPS, sizeof(ConstantBufferPS));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

	g_CBufferVS.EyePos = eyePos;
	g_CBufferVS.World = Matrix::Transpose(m_Camera->GetWorldMatrix());
	g_CBufferVS.WorldInverse = Matrix::InverseTranspose(m_Camera->GetWorldMatrix());
	g_CBufferVS.WVP = Matrix::Transpose(m_Camera->GetWVPMatrix());
	g_CBufferVS.UVTransform = Matrix::Transpose(Matrix::Scaling(1.0f));
	g_CBufferVS.LightDir = g_CBufferPS.DirLight.Direction;
	m_CBufferVS.Update(&g_CBufferVS, sizeof(ConstantBufferVS));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	D3DEngine::Instance().SetRasterizerState(D3DStaticState::SolidNoneCulling);
	D3DEngine::Instance().DrawIndexed(m_Floor.IndexCount, 0U, 0, eTriangleList);

	///Vec3 pos(-g_CBufferPS.DirLight.Direction.x, -g_CBufferPS.DirLight.Direction.y, -g_CBufferPS.DirLight.Direction.z);
	///Light::DebugDisplay(pos, Light::ePoint, *m_Camera);

	if (m_MappingType > eNormalMapping)
	{
		ImGui::SliderFloat("HeightScale", &g_CBufferPS.HeightScale, 0.0f, 0.2f);
	}

	D3DEngine::Instance().SetVSync(m_VSync);

	///D3DEngine::Instance().DrawTextInPos("RenderTextTest", 0U, 0U, 1U);

	ImGui::Text("%.2f FPS", m_FPS);
	ImGui::Combo("MappingType", &m_MappingType, "NormalMapping\0POMLearningOpenGL\0ParallaxOcclusionMappingInACL\0ParallaxMappingWithOffsetLimit\0ParallaxOcclusionMapping");
	ImGui::Checkbox("VSync", &m_VSync);
	///ImGui::SliderFloat3("LightDir", (float *)&g_CBufferPS.DirLight.Direction, -10.0f, 10.0f);
}
