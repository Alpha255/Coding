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
	PointLight PLights[3];
	SpotLight SLights[3];
	CapsuleLight CLights[3];

	ConstantBufferPS()
	{
		DirLight.Direction = Vec4(1.0, -1.0f, 1.0f, 0.0f);
		DirLight.Diffuse = Vec4(0.85f, 0.8f, 0.5f, 1.0f);
		DirLight.Diffuse = DirLight.Diffuse * DirLight.Diffuse;

		PLights[0].Position = Vec3(25.0f, 13.0f, 14.4f);
		PLights[1].Position = Vec3(-25.0f, 13.0f, 14.4f);
		PLights[2].Position = Vec3(0.0f, 13.0f, -28.9f);
			  
		PLights[0].Diffuse = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
		PLights[1].Diffuse = Vec4(0.0f, 1.0f, 0.0f, 1.0f);
		PLights[2].Diffuse = Vec4(0.0f, 0.0f, 1.0f, 1.0f);
			  
		PLights[0].Range = 50.0f;
		PLights[1].Range = 50.0f;
		PLights[2].Range = 50.0f;

		SLights[0].Position = Vec3(25.0f, 13.0f, 14.4f);
		SLights[1].Position = Vec3(-25.0f, 13.0f, 14.4f);
		SLights[2].Position = Vec3(0.0f, 13.0f, -28.9f);

		SLights[0].Diffuse = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
		SLights[1].Diffuse = Vec4(0.0f, 1.0f, 0.0f, 1.0f);
		SLights[2].Diffuse = Vec4(0.0f, 0.0f, 1.0f, 1.0f);

		SLights[0].LookAt = Vec3(0.0f, 10.0f, 0.0f);
		SLights[1].LookAt = Vec3(0.0f, 10.0f, 0.0f);
		SLights[2].LookAt = Vec3(0.0f, 10.0f, 0.0f);

		CLights[0].Position = Vec3(25.0f, 13.0f, 14.4f);
		CLights[1].Position = Vec3(-25.0f, 13.0f, 14.4f);
		CLights[2].Position = Vec3(0.0f, 13.0f, -28.9f);
		
		CLights[0].Diffuse = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
		CLights[1].Diffuse = Vec4(0.0f, 1.0f, 0.0f, 1.0f);
		CLights[2].Diffuse = Vec4(0.0f, 0.0f, 1.0f, 1.0f);

		CLights[0].Direction = CLights[2].Position - CLights[1].Position;
		CLights[1].Direction = CLights[0].Position - CLights[2].Position;
		CLights[2].Direction = CLights[1].Position - CLights[0].Position;
	}
};

Vec4 g_AmbientLowerClr = Vec4(0.1f, 0.5f, 0.1f, 1.0f);
Vec4 g_AmbientUpperClr = Vec4(0.1f, 0.2f, 0.5f, 1.0f);
ConstantBufferPS g_CBufferPS;
ConstantBufferVS g_CBufferVS;

void AppForwardLighting::Initialize()
{
	m_StanfordBunnyMesh.Create("bunny.sdkmesh");

	m_VertexShader.Create("ForwardLighting.hlsl", "VSMain");
	m_PixelShader[eHemisphericAmbient].Create("ForwardLighting.hlsl", "PSMain_HemisphericAmbient");
	m_PixelShader[eDirectional].Create("ForwardLighting.hlsl", "PSMain_Directional");
	m_PixelShader[ePoint].Create("ForwardLighting.hlsl", "PSMain_Point");
	m_PixelShader[eSpot].Create("ForwardLighting.hlsl", "PSMain_Spot");
	m_PixelShader[eCapsule].Create("ForwardLighting.hlsl", "PSMain_Capsule");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite);

	m_BunnyMaterial.Set(Material::eDiffuse, Vec4(0.85f, 0.8f, 0.5f, 1.0f));
	m_BunnyMaterial.Set(Material::eSpecular, Vec4(0.25f, 0.25f, 0.25f, 1.0f));

	m_Camera->SetViewRadius(100.0f);
	m_Camera->Move(0, -100);

	const D3D11_INPUT_ELEMENT_DESC VertexLayout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,      0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	m_Layout.Create(m_VertexShader.GetBlob(), VertexLayout, _countof(VertexLayout));
	m_StanfordBunnyMesh.SetInputLayout(m_Layout);
}

void AppForwardLighting::RenderScene()
{
	D3DEngine::Instance().ResetDefaultRenderSurfaces();
	D3DEngine::Instance().SetViewport(D3DViewport(0.0f, 0.0f, (float)m_Width, (float)m_Height));

	if (m_Wireframe)
	{
		D3DEngine::Instance().SetRasterizerState(D3DStaticState::Wireframe);
	}

	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader[m_LightingType]);

	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	Matrix world = m_Camera->GetWorldMatrix();
	g_CBufferVS.WVP = Matrix::Transpose(world * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	g_CBufferVS.World = Matrix::Transpose(world);
	g_CBufferVS.WorldInverse = Matrix::InverseTranspose(world);
	m_CBufferVS.Update(&g_CBufferVS, sizeof(ConstantBufferVS));

	g_CBufferPS.AmbientLowerClr = GammaToLinear(g_AmbientLowerClr);
	g_CBufferPS.AmbientRange = GammaToLinear(g_AmbientUpperClr) - GammaToLinear(g_AmbientLowerClr);
	g_CBufferPS.EyePos = m_Camera->GetEyePos();
	g_CBufferPS.RawMat = m_BunnyMaterial.RawValue;
	m_CBufferPS.Update(&g_CBufferPS, sizeof(ConstantBufferPS));

	m_StanfordBunnyMesh.Draw(false, true);

	ImGui::Checkbox("Wireframe", &m_Wireframe);
	if (eHemisphericAmbient == m_LightingType)
	{
		g_AmbientLowerClr = Vec4(0.1f, 0.5f, 0.1f, 1.0f);
		g_AmbientUpperClr = Vec4(0.1f, 0.2f, 0.5f, 1.0f);
	}
	else
	{
		g_AmbientLowerClr = Vec4(0.1f, 0.2f, 0.1f, 1.0f);
		g_AmbientUpperClr = Vec4(0.1f, 0.2f, 0.2f, 1.0f);
	}
	if (eDirectional == m_LightingType)
	{
		ImGui::SliderFloat3("DirtionalClr", (float *)&g_CBufferPS.DirLight.Diffuse, 0.0f, 1.0f);
	}
	if (eCapsule == m_LightingType)
	{
		static float s_CLightLength = 85.0f;
		static float s_CLightRange = 20.0f;
		ImGui::SliderFloat("CapsuleLightRange", &s_CLightRange, 10.0f, 50.0f);
		ImGui::SliderFloat("CapsuleLightLength", &s_CLightLength, 10.0f, 100.0f);
		for (uint32_t i = 0U; i < 3U; ++i)
		{
			g_CBufferPS.CLights[i].Length = s_CLightLength;
			g_CBufferPS.CLights[i].Range = s_CLightRange;
		}
	}
	ImGui::Combo("LightingType", &m_LightingType, "HemisphericAmbient\0DirectionalLight\0PointLight\0Spot\0Capsule");
}
