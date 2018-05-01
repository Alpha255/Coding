#include "Shadow.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "D3DGUI_imGui.h"

void AppShadow::Initialize()
{
	m_FloorMesh.CreateAsQuad(10.0f);
	m_BoxMesh.CreateAsCube(1.0f);

	m_VertexShader.Create("Shadow.hlsl", "VSMain");
	m_PixelShader.Create("Shadow.hlsl", "PSMain");

	m_DiffuseTexFloor.Create("wood.dds");
	m_DiffuseTexBox.Create("WoodCrate01.dds");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(Matrix), D3DBuffer::eGpuReadCpuWrite, nullptr);

	m_Camera->SetViewRadius(15.0f);
	m_Camera->Move(0, 200);
}

void AppShadow::DrawClutter()
{
	Matrix wvp = Matrix::Transpose(m_Camera->GetWVPMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);

	D3DEngine::Instance().BindMesh(m_FloorMesh);
	D3DEngine::Instance().SetShaderResourceView(m_DiffuseTexFloor, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().DrawIndexed(m_FloorMesh.IndexCount, 0U, 0, eTriangleList);

	Matrix translation = Matrix::Translation(0.0f, 0.0f, -0.5f);
	wvp = Matrix::Transpose(m_Camera->GetWorldMatrix() * translation * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));
	D3DEngine::Instance().BindMesh(m_BoxMesh);
	D3DEngine::Instance().SetShaderResourceView(m_DiffuseTexBox, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().DrawIndexed(m_BoxMesh.IndexCount, 0U, 0, eTriangleList);

	translation = Matrix::Translation(1.5f, 0.0f, -1.0f);
	wvp = Matrix::Transpose(m_Camera->GetWorldMatrix() * translation * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));
	D3DEngine::Instance().DrawIndexed(m_BoxMesh.IndexCount, 0U, 0, eTriangleList);

	translation = Matrix::Translation(0.0f, -2.0f, -0.4f);
	Matrix scale = Matrix::Scaling(0.5f);
	Matrix rotate = Matrix::RotationAxis(1.0f, 0.0f, 1.0f, 60.0f);
	wvp = Matrix::Transpose(m_Camera->GetWorldMatrix() * scale * rotate * translation * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));
	D3DEngine::Instance().DrawIndexed(m_BoxMesh.IndexCount, 0U, 0, eTriangleList);
}

void AppShadow::DrawQuad()
{

}

void AppShadow::RenderScene()
{
	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);
	DrawClutter();
}