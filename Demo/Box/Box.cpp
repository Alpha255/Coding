#include "Box.h"
#include "Camera.h"
#include "D3DEngine.h"

void AppBox::Initialize()
{
	m_BoxMesh.CreateAsCube(1.0f);
	m_DiffuseTex.Create("WoodCrate01.dds");

	m_VertexShader.Create("Box.hlsl", "VSMain");
	m_PixelShader.Create("Box.hlsl", "PSMain");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(Matrix), D3DBuffer::eGpuReadCpuWrite, nullptr);

	m_Camera->SetViewRadius(5.0f);
}

void AppBox::RenderScene()
{
	D3DEngine::Instance().SetInputLayout(m_BoxMesh.VertexLayout);
	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);

	D3DEngine::Instance().SetVertexBuffer(m_BoxMesh.VertexBuffer, sizeof(Geometry::Vertex), 0U);
	D3DEngine::Instance().SetIndexBuffer(m_BoxMesh.IndexBuffer, eR32_UInt, 0U);

	Matrix wvp = Matrix::Transpose(m_Camera->GetWVPMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	D3DEngine::Instance().SetShaderResourceView(m_DiffuseTex, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);

	D3DEngine::Instance().DrawIndexed(m_BoxMesh.IndexCount, 0U, 0, eTriangleList);
}