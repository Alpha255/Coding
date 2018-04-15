#include "ParallaxMapping.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "D3DGUI_imGui.h"

/// https://cgcookie.com/2017/06/15/normal-vs-displacement-mapping-why-games-use-normals/
/// https://msdn.microsoft.com/en-us/library/windows/desktop/ff476340(v=vs.85).aspx

void AppParallaxMapping::Initialize()
{
	m_CylinderMesh.CreateAsCylinder(5.0f, 5.0f, 0.05f, 30U, 3U);

	m_DiffuseMap.Create("wall_diffuse.dds");
	m_NormalMap.Create("wall_normal.dds");
	m_HeightMap.Create("wall_height.dds");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(Matrix), D3DBuffer::eGpuReadCpuWrite, nullptr);

	m_VertexShader[eNormalMapping].Create("Mesh.hlsl", "VSMain");
	m_PixelShader[eNormalMapping].Create("Mesh.hlsl", "PSMain");
}

void AppParallaxMapping::RenderScene()
{
	D3DEngine::Instance().SetVertexShader(m_VertexShader[m_MappingType]);
	D3DEngine::Instance().SetPixelShader(m_PixelShader[m_MappingType]);

	D3DEngine::Instance().SetInputLayout(m_CylinderMesh.VertexLayout);
	D3DEngine::Instance().SetVertexBuffer(m_CylinderMesh.VertexBuffer, sizeof(Geometry::Vertex), 0U, 0U);
	D3DEngine::Instance().SetIndexBuffer(m_CylinderMesh.IndexBuffer, eR32_UInt, 0U);

	Matrix wvp = Matrix::Transpose(m_Camera->GetWVPMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetShaderResourceView(m_DiffuseMap, 0U, D3DShader::ePixelShader);

	if (m_bWireframe)
	{
		D3DEngine::Instance().SetRasterizerState(D3DStaticState::Wireframe);
	}
	D3DEngine::Instance().DrawIndexed(m_CylinderMesh.IndexCount, 0U, 0, eTriangleList);

	ImGui::Checkbox("Wireframe", &m_bWireframe);
}
