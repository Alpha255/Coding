#include "Lighting.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "D3DLighting.h"
#include "D3DGUI_imGui.h"

struct ConstantBufferVS
{
	Matrix World;
	Matrix WorldInverseTranspose;
	Matrix WVP;

	Vec4 EyePos;

	DirectionalLight Light;

	Material Mat;

	ConstantBufferVS()
	{
		World.Identity();
		WorldInverseTranspose.Identity();
		WVP.Identity();

		EyePos = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

		Light.Ambient = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
		Light.Diffuse = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		Light.Specular = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
		Light.Direction = Vec4(0.5f, -0.5f, 0.5f, 0.0f);

		Mat.Ambient = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
		Mat.Diffuse = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
		Mat.Specular = Vec4(0.8f, 0.8f, 0.8f, 9.0f);
		Mat.Reflection = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}
};

struct ConstantBufferPS
{
	Vec4 EyePos;

	DirectionalLight Light;

	Material Mat;

	ConstantBufferPS()
	{
		EyePos = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

		Light.Ambient = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
		Light.Diffuse = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		Light.Specular = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
		Light.Direction = Vec4(0.5f, -0.5f, 0.5f, 0.0f);

		Mat.Ambient = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
		Mat.Diffuse = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
		Mat.Specular = Vec4(0.8f, 0.8f, 0.8f, 9.0f);
		Mat.Reflection = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
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

	m_Camera->SetViewRadius(2.5f);
}

void AppLighting::RenderScene()
{
	D3DEngine::Instance().SetInputLayout(m_Sphere.VertexLayout);

	if (eFlat == m_ShadingMode)
	{
		D3DEngine::Instance().SetVertexBuffer(m_FlatSphere.VertexBuffer, sizeof(Geometry::Vertex), 0U);
		D3DEngine::Instance().SetIndexBuffer(m_FlatSphere.IndexBuffer, eR32_UInt, 0U);
	}
	else
	{
		D3DEngine::Instance().SetVertexBuffer(m_Sphere.VertexBuffer, sizeof(Geometry::Vertex), 0U);
		D3DEngine::Instance().SetIndexBuffer(m_Sphere.IndexBuffer, eR32_UInt, 0U);
	}

	D3DEngine::Instance().SetVertexShader(m_VertexShader[m_ShadingMode]);
	D3DEngine::Instance().SetPixelShader(m_PixelShader[m_ShadingMode]);

	static ConstantBufferVS s_CBufferVS;
	Matrix world = m_Camera->GetWorldMatrix();
	s_CBufferVS.World = Matrix::Transpose(world);
	s_CBufferVS.WorldInverseTranspose = Matrix::InverseTranspose(world);
	s_CBufferVS.WVP = Matrix::Transpose(m_Camera->GetWVPMatrix());
	s_CBufferVS.EyePos = m_Camera->GetEyePos();
	m_CBufferVS.Update(&s_CBufferVS, sizeof(ConstantBufferVS));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	static ConstantBufferPS s_CBufferPS;
	if (m_ShadingMode > eGouraud)
	{
		s_CBufferPS.EyePos = m_Camera->GetEyePos();
		m_CBufferPS.Update(&s_CBufferPS, sizeof(ConstantBufferPS));
		D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);
	}

	if (m_bWireframe)
	{
		D3DEngine::Instance().SetRasterizerState(D3DStaticState::Wireframe);
	}

	uint32_t indexCount = (eFlat == m_ShadingMode) ? m_FlatSphere.IndexCount : m_Sphere.IndexCount;
	D3DEngine::Instance().DrawIndexed(indexCount, 0U, 0, eTriangleList);

	Vec3 lightPos(-s_CBufferVS.Light.Direction.x, -s_CBufferVS.Light.Direction.y, -s_CBufferVS.Light.Direction.z);
	Light::DrawLight(lightPos, Light::eDirectional, *m_Camera);

	if (m_bDrawNormal)
	{
		if (eFlat == m_ShadingMode)
		{
			m_FlatSphere.DrawNormal(*m_Camera);
		}
		else
		{
			m_Sphere.DrawNormal(*m_Camera);
		}
	}

	ImGui::Combo("ShadingType", (int32_t*)&m_ShadingMode, "Flat\0Gouraud\0Phong\0BlinnPhong");
	ImGui::Checkbox("Wireframe", &m_bWireframe);
	ImGui::Checkbox("DrawNormal", &m_bDrawNormal);
	ImGui::SliderFloat("SpecularFactor", &s_CBufferPS.Mat.Specular.w, 1.0f, 32.0f);
	ImGui::ColorEdit4("LightColorVS", (float*)&s_CBufferVS.Light.Diffuse);
	ImGui::ColorEdit4("LightColorPS", (float*)&s_CBufferPS.Light.Diffuse);
}