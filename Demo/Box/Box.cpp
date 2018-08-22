#include "Box.h"
#include "Camera.h"

struct ConstantBufferPS
{
	Vec4 ColorMaskWhite = {};
	Vec4 ColorMaskBlack = {};
	Vec4 ColorMaskGrey80 = {};
	Vec4 ColorMaskGrey150 = {};
};

void AppBox::Initialize()
{
#if !defined (ColorMask)
	m_BoxMesh.CreateAsCube(1.0f);
	m_DiffuseTex.Create("WoodCrate01.dds");

	m_VertexShader.Create("Box.hlsl", "VSMain");
	m_PixelShader[eNone].Create("Box.hlsl", "PSMain");
	m_PixelShader[eInversion].Create("Box.hlsl", "PSMain_Inversion");
	m_PixelShader[eGrayscale].Create("Box.hlsl", "PSMain_Grayscale");
	m_PixelShader[eSharpen].Create("Box.hlsl", "PSMain_Sharpen");
	m_PixelShader[eBlur].Create("Box.hlsl", "PSMain_Blur");
	m_PixelShader[eEdgeDetection].Create("Box.hlsl", "PSMain_EdgeDetection");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(Matrix), D3DBuffer::eGpuReadCpuWrite, nullptr);
#else
	m_Mesh.CreateAsQuad(3.0f);
	m_DiffuseTexMiddle.Create("CR_C_F_SlaveLabourer_Bottom.dds");
	m_DiffuseTexPoor.Create("CR_C_M_Poor_Top.dds");

	m_VertexShader.Create("Character.hlsl", "VSMain");
	m_PixelShader.Create("Character.hlsl", "PSMain");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(Matrix), D3DBuffer::eGpuReadCpuWrite, nullptr);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite, nullptr);
#endif

	m_Camera->SetViewRadius(5.0f);
}

void AppBox::RenderScene()
{
	D3DEngine::Instance().ResetDefaultRenderSurfaces();
	D3DEngine::Instance().SetViewport(D3DViewport(0.0f, 0.0f, (float)m_Width, (float)m_Height));

#if !defined (ColorMask)
	D3DEngine::Instance().SetInputLayout(m_BoxMesh.VertexLayout);
	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader[m_Effect]);

	D3DEngine::Instance().SetVertexBuffer(m_BoxMesh.VertexBuffer, sizeof(Geometry::Vertex), 0U);
	D3DEngine::Instance().SetIndexBuffer(m_BoxMesh.IndexBuffer, eR32_UInt, 0U);

	Matrix wvp = Matrix::Transpose(m_Camera->GetWVPMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	D3DEngine::Instance().SetShaderResourceView(m_DiffuseTex, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);

	D3DEngine::Instance().DrawIndexed(m_BoxMesh.IndexCount, 0U, 0, eTriangleList);

	ImGui::Combo("SpecialEffect", &m_Effect, "None\0Inversion\0Grayscale\0Sharpen\0Blur\0EdgeDetection");
#else
	D3DEngine::Instance().SetInputLayout(m_Mesh.VertexLayout);
	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);

	D3DEngine::Instance().SetVertexBuffer(m_Mesh.VertexBuffer, sizeof(Geometry::Vertex), 0U);
	D3DEngine::Instance().SetIndexBuffer(m_Mesh.IndexBuffer, eR32_UInt, 0U);

	Matrix wvp = Matrix::Transpose(m_Camera->GetWVPMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	static ConstantBufferPS CBufferPS;
	m_CBufferPS.Update(&CBufferPS, sizeof(ConstantBufferPS));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

	if (0 == m_TexIndex)
	{
		D3DEngine::Instance().SetShaderResourceView(m_DiffuseTexMiddle, 0U, D3DShader::ePixelShader);
	}
	else
	{
		D3DEngine::Instance().SetShaderResourceView(m_DiffuseTexPoor, 0U, D3DShader::ePixelShader);
	}

	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);

	D3DEngine::Instance().DrawIndexed(m_Mesh.IndexCount, 0U, 0, eTriangleList);

	ImGui::SliderFloat3("WhiteMask", (float *)&CBufferPS.ColorMaskWhite, 0.0f, 1.0f);
	ImGui::SliderFloat3("BlackMask", (float *)&CBufferPS.ColorMaskBlack, 0.0f, 1.0f);
	ImGui::SliderFloat3("GreyMask80", (float *)&CBufferPS.ColorMaskGrey80, 0.0f, 1.0f);
	ImGui::SliderFloat3("GreyMask150", (float *)&CBufferPS.ColorMaskGrey150, 0.0f, 1.0f);
	ImGui::Combo("TextureType", &m_TexIndex, "Middle\0Poor");
#endif
}