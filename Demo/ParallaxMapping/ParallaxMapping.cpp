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
	Material::RawMaterial RawMat;

	ConstantBufferPS()
	{
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

	//m_FloorMaterial.Set(Material::eDiffuse, "bricks2.dds");
	//m_FloorMaterial.Set(Material::eSpecular, "bricks2_disp.dds");
	//m_FloorMaterial.Set(Material::eNormal, "bricks2_normal.dds");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite);

	m_VertexShader.Create("ParallaxMapping.hlsl", "VSMain");

	m_PixelShader[eNormalMapping].Create("ParallaxMapping.hlsl", "PSMain_NormalMapping");
}

void AppParallaxMapping::RenderScene()
{
	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader[m_MappingType]);

	m_Floor.Bind(&m_FloorMaterial);

	ConstantBufferVS CBufferVS;
	CBufferVS.World = Matrix::Transpose(m_Camera->GetWorldMatrix());
	CBufferVS.WorldInverse = Matrix::InverseTranspose(m_Camera->GetWorldMatrix());
	CBufferVS.WVP = Matrix::Transpose(m_Camera->GetWVPMatrix());
	CBufferVS.UVTransform = Matrix::Transpose(Matrix::Scaling(1.0f));
	m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	ConstantBufferPS CBufferPS;
	CBufferPS.EyePos = m_Camera->GetEyePos();
	CBufferPS.RawMat = m_FloorMaterial.RawValue;
	m_CBufferPS.Update(&CBufferPS, sizeof(ConstantBufferPS));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

	///D3DEngine::Instance().SetRasterizerState(D3DStaticState::SolidNoneCulling);
	D3DEngine::Instance().DrawIndexed(m_Floor.IndexCount, 0U, 0, eTriangleList);

	Vec3 pos(-CBufferPS.DirLight.Direction.x, -CBufferPS.DirLight.Direction.y, -CBufferPS.DirLight.Direction.z);
	///Light::DebugDisplay(pos, Light::ePoint, *m_Camera);

	ImGui::Combo("MappingType", &m_MappingType, "NormalMapping");
}
