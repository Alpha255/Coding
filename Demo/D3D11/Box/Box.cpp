#include "Box.h"
#include "Util/ImGUI.h"

void AppBox::InitScene()
{
	m_BoxMesh.CreateAsCube(1.0f);
	m_DiffuseTex.Create("WoodCrate01.dds");

	m_VertexShader.Create("Box.hlsl", "VSMain");
	m_PixelShader[eNone].Create("Box.hlsl", "PSMain");
	m_PixelShader[eInversion].Create("Box.hlsl", "PSMain_Inversion");
	m_PixelShader[eGrayscale].Create("Box.hlsl", "PSMain_Grayscale");
	m_PixelShader[eSharpen].Create("Box.hlsl", "PSMain_Sharpen");
	m_PixelShader[eBlur].Create("Box.hlsl", "PSMain_Blur");
	m_PixelShader[eEdgeDetection].Create("Box.hlsl", "PSMain_EdgeDetection");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(Matrix), eGpuReadCpuWrite, nullptr);

	m_Camera.SetViewRadius(5.0f);
}

void AppBox::RenderScene()
{
	REngine::Instance().ResetDefaultRenderSurfaces();
	REngine::Instance().SetViewport(RViewport(0.0f, 0.0f, (float)m_Width, (float)m_Height));

	REngine::Instance().SetInputLayout(m_BoxMesh.InputLayout);
	REngine::Instance().SetVertexShader(m_VertexShader);
	REngine::Instance().SetPixelShader(m_PixelShader[m_Effect]);

	REngine::Instance().SetVertexBuffer(m_BoxMesh.VertexBuffer, sizeof(Geometry::Vertex), 0U);
	REngine::Instance().SetIndexBuffer(m_BoxMesh.IndexBuffer, eR32_UInt, 0U);

	Matrix wvp = Matrix::Transpose(m_Camera.GetWVPMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));
	REngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, eVertexShader);

	REngine::Instance().SetShaderResourceView(m_DiffuseTex, 0U, ePixelShader);
	REngine::Instance().SetSamplerState(RStaticState::LinearSampler, 0U, ePixelShader);

	REngine::Instance().DrawIndexed(m_BoxMesh.IndexCount, 0U, 0, eTriangleList);

	ImGui::Combo("SpecialEffect", &m_Effect, "None\0Inversion\0Grayscale\0Sharpen\0Blur\0EdgeDetection");
	ImGui::Checkbox("VSync", &m_bVSync);
	ImGui::Text("\n%.2f FPS", m_FPS);
}