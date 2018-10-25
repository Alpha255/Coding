#include "Shadow.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "D3DTexture.h"
#include "D3DLighting.h"
#include "ImGUI.h"

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

	Matrix LightSpaceVP;

	ConstantBufferPS()
	{
		Light.Position = Vec3(-2.5f, 0.0f, -3.0f);
		Light.Range = 50.0f;
	}
};

ConstantBufferVS g_CBufferVS;
ConstantBufferPS g_CBufferPS;
Matrix g_LightSpaceVP;

void AppShadow::Initialize()
{
	m_FloorMesh.CreateAsQuad(10.0f);
	m_BoxMesh.CreateAsCube(1.0f);
	m_QuadMesh.CreateAsQuad(0.3f, 0.9f, 0.4f, 0.5f);

	m_VertexShaders[eDrawMain].Create("Shadow.hlsl", "VSMain");
	m_VertexShaders[eDrawQuad].Create("Shadow.hlsl", "VSMain_Quad");

	m_PixelShaders[eDrawMain].Create("Shadow.hlsl", "PSMain");
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

	m_ViewportsDepth = { 0.0f, 0.0f, (float)DepthTexSize, (float)DepthTexSize, 0.0f, 1.0f };

	m_Camera->SetViewRadius(15.0f);
	m_Camera->Move(0, 200);
}

void AppShadow::DrawClutter(bool bDepthOnly, const Matrix &vp)
{
	g_CBufferPS.EyePos = m_Camera->GetEyePos();
	g_CBufferPS.RawMat = m_FloorMaterial.RawValue;
	g_CBufferPS.LightSpaceVP = Matrix::Transpose(g_LightSpaceVP);
	m_CBufferPS.Update(&g_CBufferPS, sizeof(ConstantBufferPS));

	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

	Matrix worldCamera = m_Camera->GetWorldMatrix();
	g_CBufferVS.World = Matrix::Transpose(worldCamera);
	g_CBufferVS.WorldInverse = Matrix::InverseTranspose(m_Camera->GetWorldMatrix());
	g_CBufferVS.WVP = Matrix::Transpose(worldCamera * vp);
	m_CBufferVS.Update(&g_CBufferVS, sizeof(ConstantBufferVS));

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
	g_CBufferVS.World = Matrix::Transpose(world);
	g_CBufferVS.WorldInverse = Matrix::InverseTranspose(world);
	g_CBufferVS.WVP = Matrix::Transpose(world * vp);
	m_CBufferVS.Update(&g_CBufferVS, sizeof(ConstantBufferVS));

	g_CBufferPS.RawMat = m_BoxMaterial.RawValue;
	m_CBufferPS.Update(&g_CBufferPS, sizeof(ConstantBufferPS));

	m_BoxMesh.Bind(&m_BoxMaterial);
	D3DEngine::Instance().DrawIndexed(m_BoxMesh.IndexCount, 0U, 0, eTriangleList);

	world = m_Camera->GetWorldMatrix() * Matrix::Translation(1.5f, 0.0f, -1.0f);
	g_CBufferVS.World = Matrix::Transpose(world);
	g_CBufferVS.WorldInverse = Matrix::InverseTranspose(world);
	g_CBufferVS.WVP = Matrix::Transpose(world * vp);
	m_CBufferVS.Update(&g_CBufferVS, sizeof(ConstantBufferVS));
	D3DEngine::Instance().DrawIndexed(m_BoxMesh.IndexCount, 0U, 0, eTriangleList);

	world = m_Camera->GetWorldMatrix() * Matrix::RotationAxis(1.0f, 0.0f, 1.0f, 60.0f) * Matrix::Scaling(0.5f) * Matrix::Translation(0.0f, -2.0f, -0.4f);
	g_CBufferVS.World = Matrix::Transpose(world);
	g_CBufferVS.WorldInverse = Matrix::InverseTranspose(world);
	g_CBufferVS.WVP = Matrix::Transpose(world * vp);
	m_CBufferVS.Update(&g_CBufferVS, sizeof(ConstantBufferVS));
	D3DEngine::Instance().DrawIndexed(m_BoxMesh.IndexCount, 0U, 0, eTriangleList);
}

void AppShadow::DrawQuad()
{
	D3DEngine::Instance().SetVertexShader(m_VertexShaders[eDrawQuad]);
	D3DEngine::Instance().SetPixelShader(m_PixelShaders[eDrawQuad]);

	D3DEngine::Instance().SetShaderResourceView(m_DepthTexture, 0U, D3DShader::ePixelShader);

	m_QuadMesh.Draw();
}

float g_x = 0.0f;

void AppShadow::RenderScene()
{
	Matrix lightViewMatrix = Matrix::LookAtLH(g_CBufferPS.Light.Position, Vec3(g_x, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	Matrix lightPorjMatrix = Matrix::PerspectiveFovLH(Math::XM_PIDIV2, 1.0f, 1.0f, 10.0f);
	g_LightSpaceVP = lightViewMatrix * lightPorjMatrix;

	D3DRenderTargetView EmptyRTV;
	D3DPixelShader EmptyPS;
	D3DEngine::Instance().SetViewport(m_ViewportsDepth);
	D3DEngine::Instance().ClearDepthStencilView(m_DepthSurface, D3DDepthStencilView::eDepthStencil, 1.0f, 0U);
	D3DEngine::Instance().SetRenderTargetView(EmptyRTV, 0U);
	D3DEngine::Instance().SetDepthStencilView(m_DepthSurface);
	D3DEngine::Instance().SetVertexShader(m_VertexShaders[eDrawMain]);
	D3DEngine::Instance().SetPixelShader(EmptyPS);
	DrawClutter(true, g_LightSpaceVP);

	D3DEngine::Instance().ResetDefaultRenderSurfaces();
	D3DEngine::Instance().SetViewport(D3DViewport(0.0f, 0.0f, (float)m_Width, (float)m_Height));
	D3DEngine::Instance().ForceCommitState();
	D3DEngine::Instance().SetVertexShader(m_VertexShaders[eDrawMain]);
	D3DEngine::Instance().SetPixelShader(m_PixelShaders[eDrawMain]);
	DrawClutter(false, m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());

	DrawQuad();

	Light::DebugDisplay(g_CBufferPS.Light.Position, Light::ePoint, *m_Camera);

	ImGui::SliderFloat3("LightPos", (float *)&g_CBufferPS.Light.Position, -10.0f, 10.0f);
	ImGui::SliderFloat("x", &g_x, -10.0f, 10.0f);
}