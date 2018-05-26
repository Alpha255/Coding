#include "Shadow.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "D3DTexture.h"
#include "D3DLighting.h"
#include "D3DGUI_imGui.h"

struct ConstantBufferVS
{
	Matrix World;
	Matrix WorldInverse;
	Matrix WVP;
};

struct ConstantBufferPS
{
	Vec4 EyePos;
	Material::RawMaterial RawMat;
	PointLight Light;

	ConstantBufferPS()
	{
		Light.Position = Vec3(2.5f, 0.0f, -2.5f);
		Light.Range = 20.0f;
	}
};

void AppShadow::Initialize()
{
	m_FloorMesh.CreateAsQuad(10.0f);
	m_BoxMesh.CreateAsCube(1.0f);
	m_QuadMesh.CreateAsQuad(0.3f, 0.9f, 0.5f, 0.4f);

	m_VertexShaders[eDrawMain].Create("Shadow.hlsl", "VSMain");
	m_VertexShaders[eDrawDepth].Create("Shadow.hlsl", "VSMain_Depth");
	m_VertexShaders[eDrawQuad].Create("Shadow.hlsl", "VSMain_Quad");

	m_PixelShaders[eDrawMain].Create("Shadow.hlsl", "PSMain");
	m_PixelShaders[eDrawDepth].Create("Shadow.hlsl", "PSMain_Depth");
	m_PixelShaders[eDrawQuad].Create("Shadow.hlsl", "PSMain_Quad");

	m_FloorMaterial.Set(Material::eDiffuse, "wood.dds");
	m_BoxMaterial.Set(Material::eDiffuse, "WoodCrate01.dds");

	const uint32_t DepthTexSize = 1024U;
	D3DTexture2D depthTexture;
	depthTexture.Create(eR32_Typeless, DepthTexSize, DepthTexSize, D3DBuffer::eBindAsShaderResource | D3DBuffer::eBindAsDepthStencil);
	m_DepthTexture.CreateAsTexture(D3DView::eTexture2D, depthTexture, eR32_Float, 0U, 1U);
	m_DepthSurface.CreateAsTexture(D3DView::eTexture2D, depthTexture, eD32_Float, 0U, 0U);

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite, nullptr);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite, nullptr);

	m_Viewports[eDrawMain] = { 0.0f, 0.0f, (float)m_Width, (float)m_Height, 0.0f, 1.0f };
	m_Viewports[eDrawDepth] = { 0.0f, 0.0f, (float)DepthTexSize, (float)DepthTexSize, 0.0f, 1.0f };

	m_Camera->SetViewRadius(15.0f);
	m_Camera->Move(0, 200);
}

void AppShadow::DrawClutter(bool bDepthOnly)
{
	ConstantBufferVS CBufferVS;
	ConstantBufferPS CBufferPS;

	CBufferPS.EyePos = m_Camera->GetEyePos();
	CBufferPS.RawMat = m_FloorMaterial.RawValue;
	m_CBufferPS.Update(&CBufferPS, sizeof(ConstantBufferPS));

	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

	CBufferVS.World = Matrix::Transpose(m_Camera->GetWorldMatrix());
	CBufferVS.WorldInverse = Matrix::InverseTranspose(m_Camera->GetWorldMatrix());
	CBufferVS.WVP = Matrix::Transpose(m_Camera->GetWVPMatrix());
	m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));

	m_FloorMesh.Bind(&m_FloorMaterial);
	if (!bDepthOnly)
	{
		D3DEngine::Instance().SetShaderResourceView(m_DepthTexture, 1U, D3DShader::ePixelShader);
	}
	else
	{
		D3DShaderResourceView EmptySRV;
		D3DEngine::Instance().SetShaderResourceView(EmptySRV, 1U, D3DShader::ePixelShader);
	}
	D3DEngine::Instance().DrawIndexed(m_FloorMesh.IndexCount, 0U, 0, eTriangleList);

	Matrix world = m_Camera->GetWorldMatrix() * Matrix::Translation(0.0f, 0.0f, -0.5f);
	CBufferVS.World = Matrix::Transpose(world);
	CBufferVS.WorldInverse = Matrix::InverseTranspose(world);
	CBufferVS.WVP = Matrix::Transpose(world * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));

	CBufferPS.RawMat = m_BoxMaterial.RawValue;
	m_CBufferPS.Update(&CBufferPS, sizeof(ConstantBufferPS));

	m_BoxMesh.Bind(&m_BoxMaterial);
	D3DEngine::Instance().DrawIndexed(m_BoxMesh.IndexCount, 0U, 0, eTriangleList);

	world = m_Camera->GetWorldMatrix() * Matrix::Translation(1.5f, 0.0f, -1.0f);
	CBufferVS.World = Matrix::Transpose(world);
	CBufferVS.WorldInverse = Matrix::InverseTranspose(world);
	CBufferVS.WVP = Matrix::Transpose(world * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));
	D3DEngine::Instance().DrawIndexed(m_BoxMesh.IndexCount, 0U, 0, eTriangleList);

	world = m_Camera->GetWorldMatrix() * Matrix::RotationAxis(1.0f, 0.0f, 1.0f, 60.0f) * Matrix::Scaling(0.5f) * Matrix::Translation(0.0f, -2.0f, -0.4f);
	CBufferVS.World = Matrix::Transpose(world);
	CBufferVS.WorldInverse = Matrix::InverseTranspose(world);
	CBufferVS.WVP = Matrix::Transpose(world * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));
	D3DEngine::Instance().DrawIndexed(m_BoxMesh.IndexCount, 0U, 0, eTriangleList);
}

void AppShadow::DrawQuad()
{
	m_QuadMesh.Bind();

	D3DEngine::Instance().SetVertexShader(m_VertexShaders[eDrawQuad]);
	D3DEngine::Instance().SetPixelShader(m_PixelShaders[eDrawQuad]);

	D3DEngine::Instance().SetShaderResourceView(m_DepthTexture, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().DrawIndexed(m_QuadMesh.IndexCount, 0U, 0, eTriangleList);
}

void AppShadow::RenderScene()
{
	D3DRenderTargetView EmptyRTV;
	D3DEngine::Instance().SetViewport(m_Viewports[eDrawDepth]);
	D3DEngine::Instance().ClearDepthStencilView(m_DepthSurface, D3DDepthStencilView::eDepthStencil, 1.0f, 0U);
	D3DEngine::Instance().SetRenderTargetView(EmptyRTV, 0U);
	D3DEngine::Instance().SetDepthStencilView(m_DepthSurface);
	D3DEngine::Instance().SetVertexShader(m_VertexShaders[eDrawDepth]);
	D3DEngine::Instance().SetPixelShader(m_PixelShaders[eDrawDepth]);
	DrawClutter(true);

	D3DEngine::Instance().SetViewport(m_Viewports[eDrawMain]);
	D3DEngine::Instance().ResetRenderSurfaces();
	D3DEngine::Instance().ClearRenderSurfaces();
	D3DEngine::Instance().ForceCommitState();
	D3DEngine::Instance().SetVertexShader(m_VertexShaders[eDrawMain]);
	D3DEngine::Instance().SetPixelShader(m_PixelShaders[eDrawMain]);
	DrawClutter(false);

	DrawQuad();
}

void AppShadow::ResizeWindow(uint32_t width, uint32_t height)
{
	Base::ResizeWindow(width, height);

	m_Viewports[eDrawMain].Width = (float)m_Width;
	m_Viewports[eDrawMain].Height = (float)m_Height;
}