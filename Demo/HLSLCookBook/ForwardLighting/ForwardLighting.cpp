#include "ForwardLighting.h"
#include "D3DEngine.h"
#include "D3DGUI_imGui.h"
#include "D3DLighting.h"
#include "Camera.h"

struct ConstantBufferVS
{
	Matrix WVP;
	Matrix World;
	Matrix WorldInverse;
};

struct ConstantBufferPS
{
	Vec4 AmbientLowerClr;
	Vec4 AmbientRange;

	Vec4 EyePos;

	Material::RawMaterial RawMat;
	DirectionalLight DirLight;
	PointLight PLight[3];

	ConstantBufferPS()
	{
		DirLight.Direction = Vec4(1.0, -1.0f, 1.0f, 0.0f);
		DirLight.Diffuse = Vec4(0.85f, 0.8f, 0.5f, 1.0f);
		DirLight.Diffuse = DirLight.Diffuse * DirLight.Diffuse;

		PLight[0].Position = Vec3(25.0f, 13.0f, 14.4f);
		PLight[1].Position = Vec3(-25.0f, 13.0f, 14.4f);
		PLight[2].Position = Vec3(0.0f, 13.0f, -28.9f);

		PLight[0].Diffuse = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
		PLight[1].Diffuse = Vec4(0.0f, 1.0f, 0.0f, 1.0f);
		PLight[2].Diffuse = Vec4(0.0f, 0.0f, 1.0f, 1.0f);

		PLight[0].Range = 50.0f;
		PLight[1].Range = 50.0f;
		PLight[2].Range = 50.0f;
	}
};

Vec4 g_AmbientLowerClr = Vec4(0.1f, 0.5f, 0.1f, 1.0f);
Vec4 g_AmbientUpperClr = Vec4(0.1f, 0.2f, 0.5f, 1.0f);
ConstantBufferPS g_CBufferPS;
ConstantBufferVS g_CBufferVS;

void AppForwardLighting::Initialize()
{
	m_StanfordBunny.Create("bun_zipper_res2.obj");
	m_Floor.CreateAsQuad(10.0f);
	m_StanfordBunnyMesh.Create("bunny.sdkmesh");

	m_VertexShader.Create("ForwardLighting.hlsl", "VSMain");
	m_PixelShader[eHemisphericAmbient].Create("ForwardLighting.hlsl", "PSMain_HemisphericAmbient");
	m_PixelShader[eDirectional].Create("ForwardLighting.hlsl", "PSMain_Directional");
	m_PixelShader[ePoint].Create("ForwardLighting.hlsl", "PSMain_Point");
	m_PixelShader[eSpot].Create("ForwardLighting.hlsl", "PSMain_Spot");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite);

	m_BunnyMaterial.Set(Material::eDiffuse, Vec4(0.85f, 0.8f, 0.5f, 1.0f));
	m_BunnyMaterial.Set(Material::eSpecular, Vec4(0.25f, 0.25f, 0.25f, 1.0f));

	m_Camera->SetViewRadius(10.0f);
	m_Camera->Move(0, -100);
}

void AppForwardLighting::RenderScene()
{
	if (m_Wireframe)
	{
		D3DEngine::Instance().SetRasterizerState(D3DStaticState::Wireframe);
	}

	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader[m_LightingType]);

	/// The higher the material's specular exponent is, the smaller the light spread is
	/// DownColor * (1-a) + UpColor * a = DownColor + a * (UpColor - DownColor)
	g_CBufferPS.AmbientLowerClr = GammaToLinear(g_AmbientLowerClr);
	g_CBufferPS.AmbientRange = GammaToLinear(g_AmbientUpperClr) - GammaToLinear(g_AmbientLowerClr);
	g_CBufferPS.EyePos = m_Camera->GetEyePos();
	g_CBufferPS.RawMat = m_BunnyMaterial.RawValue;
	m_CBufferPS.Update(&g_CBufferPS, sizeof(ConstantBufferPS));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	Matrix world = m_Camera->GetWorldMatrix() * Matrix::Scaling(15.0f) * Matrix::Translation(0.0f, -1.0f, 0.0f);
	g_CBufferVS.WVP = Matrix::Transpose(world * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	g_CBufferVS.World = Matrix::Transpose(world);
	g_CBufferVS.WorldInverse = Matrix::InverseTranspose(world);
	m_CBufferVS.Update(&g_CBufferVS, sizeof(ConstantBufferVS));
	m_StanfordBunny.Bind(&m_BunnyMaterial);
	D3DEngine::Instance().DrawIndexed(m_StanfordBunny.IndexCount, 0U, 0, eTriangleList);

	world = m_Camera->GetWorldMatrix() * Matrix::RotationAxis(1.0f, 0.0f, 0.0f, 90.0f) * Matrix::Translation(0.0f, -0.5f, 0.0f);
	g_CBufferVS.WVP = Matrix::Transpose(world * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	g_CBufferVS.World = Matrix::Transpose(world);
	g_CBufferVS.WorldInverse = Matrix::InverseTranspose(world);
	m_CBufferVS.Update(&g_CBufferVS, sizeof(ConstantBufferVS));
	m_Floor.Bind();
	D3DEngine::Instance().DrawIndexed(m_Floor.IndexCount, 0U, 0, eTriangleList);

	///Transform trans;
	///m_StanfordBunnyMesh.Draw(*m_Camera, trans, true);

	ImGui::Checkbox("Wireframe", &m_Wireframe);
	ImGui::SliderFloat3("AmbientLowerClr", (float *)&g_AmbientLowerClr, 0.0f, 1.0f);
	ImGui::SliderFloat3("AmbientUpperClr", (float *)&g_AmbientUpperClr, 0.0f, 1.0f);
	ImGui::SliderFloat3("DirtionalClr", (float *)&g_CBufferPS.DirLight.Diffuse, 0.0f, 1.0f);
	ImGui::Combo("LightingType", &m_LightingType, "HemisphericAmbient\0DirectionalLight\0PointLight");
}
