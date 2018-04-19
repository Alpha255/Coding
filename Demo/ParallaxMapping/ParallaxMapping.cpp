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
	DirectionalLight DirLight;
	Material Mat;

	Vec4 EyePos;

	ConstantBufferPS()
	{
		Mat.Ambient = Vec4(0.1f, 0.1f, 0.1f, 0.0f);
		Mat.Diffuse = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		Mat.Specular = Vec4(0.5f, 0.5f, 0.5f, 30.0f);
		Mat.Reflection = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

		DirLight.Ambient = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
		DirLight.Diffuse = Vec4(0.7f, 0.7f, 0.6f, 1.0f);
		DirLight.Specular = Vec4(0.8f, 0.8f, 0.7f, 1.0f);
		DirLight.Direction = Vec4(0.7f, 0.0f, -0.7f, 0.0f);
	}
};

void AppParallaxMapping::Initialize()
{
	m_CylinderMesh.CreateAsCylinder(5.0f, 5.0f, 0.05f, 30U, 5U);

	m_DiffuseMap.Create("wall_diffuse.dds");
	m_NormalMap.Create("wall_normal.dds");
	m_HeightMap.Create("wall_height.dds");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite, nullptr);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite, nullptr);

	m_VertexShader[eNormalMapping].Create("ParallaxMapping.hlsl", "VSMain");
	m_PixelShader[eNormalMapping].Create("ParallaxMapping.hlsl", "PSMain");
}

void AppParallaxMapping::RenderScene()
{
	D3DEngine::Instance().SetVertexShader(m_VertexShader[m_MappingType]);
	D3DEngine::Instance().SetPixelShader(m_PixelShader[m_MappingType]);

	D3DEngine::Instance().SetInputLayout(m_CylinderMesh.VertexLayout);
	D3DEngine::Instance().SetVertexBuffer(m_CylinderMesh.VertexBuffer, sizeof(Geometry::Vertex), 0U, 0U);
	D3DEngine::Instance().SetIndexBuffer(m_CylinderMesh.IndexBuffer, eR32_UInt, 0U);

	ConstantBufferVS CBufferVS;
	CBufferVS.World = Matrix::Transpose(m_Camera->GetWorldMatrix());
	CBufferVS.WorldInverse = Matrix::InverseTranspose(m_Camera->GetWorldMatrix());
	CBufferVS.WVP = Matrix::Transpose(m_Camera->GetWVPMatrix());
	CBufferVS.UVTransform = Matrix::Transpose(Matrix::Scaling(0.01f));
	m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	static ConstantBufferPS CBufferPS;
	CBufferPS.EyePos = m_Camera->GetEyePos();
	m_CBufferPS.Update(&CBufferPS, sizeof(ConstantBufferPS));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetShaderResourceView(m_DiffuseMap, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetShaderResourceView(m_NormalMap, 1U, D3DShader::ePixelShader);

	if (m_bWireframe)
	{
		D3DEngine::Instance().SetRasterizerState(D3DStaticState::Wireframe);
	}
	D3DEngine::Instance().DrawIndexed(m_CylinderMesh.IndexCount, 0U, 0, eTriangleList);

	ImGui::Checkbox("Wireframe", &m_bWireframe);
	ImGui::SliderFloat3("Position", (float*)&CBufferPS.DirLight.Direction, -10.0f, 10.0f);
}
