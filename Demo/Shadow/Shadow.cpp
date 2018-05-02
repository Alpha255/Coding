#include "Shadow.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "D3DTexture.h"
#include "D3DGUI_imGui.h"

void AppShadow::Initialize()
{
	m_FloorMesh.CreateAsQuad(10.0f);
	m_BoxMesh.CreateAsCube(1.0f);
	m_QuadMesh.CreateAsQuad(0.3f, 0.9f, 0.5f, 0.4f);

	m_VertexShaderDepth.Create("Shadow.hlsl", "VSMain_Depth");
	m_PixelShaderDepth.Create("Shadow.hlsl", "PSMain_Depth");
	m_VertexShader.Create("Shadow.hlsl", "VSMain");
	m_PixelShader.Create("Shadow.hlsl", "PSMain");
	m_VertexShaderQuad.Create("Shadow.hlsl", "VSMain_Quad");
	m_PixelShaderQuad.Create("Shadow.hlsl", "PSMain_Quad");

	m_DiffuseTexFloor.Create("wood.dds");
	m_DiffuseTexBox.Create("WoodCrate01.dds");

	D3DTexture2D depthTexture;
	depthTexture.Create(eRGBA8_UNorm, 1024U, 1024U, D3DBuffer::eBindAsRenderTarget | D3DBuffer::eBindAsShaderResource);
	m_DepthTexture.CreateAsTexture(D3DView::eTexture2D, depthTexture, eRGBA8_UNorm, 0U, 1U);
	m_DepthSurface.CreateAsTexture(D3DView::eTexture2D, depthTexture, eRGBA8_UNorm, 0U);
	m_DepthDSV.Create(eD32_Float, 1024U, 1024U);

	m_CBufferVS.CreateAsConstantBuffer(sizeof(Matrix), D3DBuffer::eGpuReadCpuWrite, nullptr);

	m_Viewport = { 0.0f, 0.0f, (float)m_Width, (float)m_Height, 0.0f, 1.0f };
	m_ViewportDepth = { 0.0f, 0.0f, (float)1024U, (float)1024U, 0.0f, 1.0f };

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
	D3DEngine::Instance().SetVertexShader(m_VertexShaderQuad);
	D3DEngine::Instance().SetPixelShader(m_PixelShaderQuad);

	D3DEngine::Instance().BindMesh(m_QuadMesh);

	D3DEngine::Instance().SetShaderResourceView(m_DepthTexture, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().DrawIndexed(m_QuadMesh.IndexCount, 0U, 0, eTriangleList);
}

void AppShadow::RenderScene()
{
	D3DEngine::Instance().SetViewport(m_ViewportDepth);
	D3DEngine::Instance().ClearDepthStencilView(m_DepthDSV, D3DDepthStencilView::eDepthStencil, 1.0f, 0U);
	D3DEngine::Instance().ClearRenderTargetView(m_DepthSurface, reinterpret_cast<const float*>(&Color::Silver));
	D3DEngine::Instance().SetRenderTargetView(m_DepthSurface, 0U);
	D3DEngine::Instance().SetDepthStencilView(m_DepthDSV);
	D3DEngine::Instance().SetVertexShader(m_VertexShaderDepth);
	D3DEngine::Instance().SetPixelShader(m_PixelShaderDepth);
	DrawClutter();

	D3DEngine::Instance().SetViewport(m_Viewport);
	D3DEngine::Instance().ResetRenderSurfaces();
	D3DEngine::Instance().ClearRenderSurfaces();
	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);
	DrawClutter();

	DrawQuad();
}

void AppShadow::ResizeWindow(uint32_t width, uint32_t height)
{
	Base::ResizeWindow(width, height);

	m_Viewport.Width = (float)m_Width;
	m_Viewport.Height = (float)m_Height;
}