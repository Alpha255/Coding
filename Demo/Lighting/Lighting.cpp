#include "Lighting.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "D3DLighting.h"
#include "D3DGUI_imGui.h"

struct ConstantBufferVS
{
	Matrix World;
	Matrix WorldInverse;
	Matrix WVP;

	Vec4 EyePos;

	DirectionalLight Light;

	Material::RawMaterial Mat;

	ConstantBufferVS()
	{
		World.Identity();
		WorldInverse.Identity();
		WVP.Identity();

		Light.Direction = Vec4(0.5f, -0.5f, 0.5f, 0.0f);
	}
};

struct ConstantBufferPS
{
	Vec4 EyePos;

	DirectionalLight Light;

	Material::RawMaterial Mat;

	ConstantBufferPS()
	{
		Light.Direction = Vec4(1.0f, -1.0f, 1.0f, 0.0f);
		Light.SpecularIntensity = 16.0f;
	}
};

void AppLighting::Initialize()
{
	m_FlatSphere.CreateAsFlatGeoSphere(0.5f, 2U);
	m_Sphere.CreateAsGeoSphere(0.5f, 2U);

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite, nullptr);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite, nullptr);

	const char *pEntry[eShadingModeCount] = 
	{
		"Flat",
		"Gouraud",
		"Phong",
		"BlinnPhong"
	};

	for (uint32_t i = 0U; i < eShadingModeCount; ++i)
	{
		std::string vsEntry("VSMain");
		std::string psEntry("PSMain");
		vsEntry += pEntry[i];
		psEntry += pEntry[i];

		m_VertexShader[i].Create("Lighting.hlsl", vsEntry.c_str());
		m_PixelShader[i].Create("Lighting.hlsl", psEntry.c_str());
	}

	m_SphereMaterial.Set(Material::eDiffuse, Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	m_SphereMaterial.Set(Material::eSpecular, Vec4(1.0f, 1.0f, 1.0f, 1.0f));

	m_Camera->SetViewRadius(2.5f);
}

void AppLighting::RenderScene()
{
	D3DEngine::Instance().SetInputLayout(m_Sphere.VertexLayout);

	if (eFlat == m_ShadingMode)
	{
		m_FlatSphere.Bind(m_SphereMaterial);
	}
	else
	{
		m_Sphere.Bind(m_SphereMaterial);
	}

	D3DEngine::Instance().SetVertexShader(m_VertexShader[m_ShadingMode]);
	D3DEngine::Instance().SetPixelShader(m_PixelShader[m_ShadingMode]);

	ConstantBufferVS CBufferVS;
	Matrix world = m_Camera->GetWorldMatrix();
	CBufferVS.World = Matrix::Transpose(world);
	CBufferVS.WorldInverse = Matrix::InverseTranspose(world);
	CBufferVS.WVP = Matrix::Transpose(m_Camera->GetWVPMatrix());
	CBufferVS.EyePos = m_Camera->GetEyePos();
	CBufferVS.Mat = m_SphereMaterial.RawValue;
	m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	ConstantBufferPS CBufferPS;
	if (m_ShadingMode > eGouraud)
	{
		CBufferPS.EyePos = m_Camera->GetEyePos();
		CBufferPS.Mat = m_SphereMaterial.RawValue;
		m_CBufferPS.Update(&CBufferPS, sizeof(ConstantBufferPS));
		D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);
	}

	uint32_t indexCount = (eFlat == m_ShadingMode) ? m_FlatSphere.IndexCount : m_Sphere.IndexCount;
	D3DEngine::Instance().DrawIndexed(indexCount, 0U, 0, eTriangleList);

	ImGui::Combo("ShadingType", (int32_t*)&m_ShadingMode, "Flat\0Gouraud\0Phong\0BlinnPhong");
}