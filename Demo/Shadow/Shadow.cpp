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
	Material Mat;
	PointLight Light;

	ConstantBufferPS()
	{
		Mat.Ambient = Vec4(0.1f, 0.1f, 0.1f, 1.0f);
		Mat.Diffuse = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
		Mat.Specular = Vec4(0.5f, 0.5f, 0.5f, 32.0f);
		Mat.Reflection = Vec4(0.0f, 0.0f, 0.0f, 0.0f);

		Light.Ambient = Vec4(0.1f, 0.1f, 0.1f, 1.0f);
		Light.Diffuse = Vec4(0.3f, 0.3f, 0.3f, 1.0f);
		Light.Specular = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
		Light.Position = Vec3(2.5f, 0.0f, 1.0f);
		Light.Range = 10.0f;
		Light.Attenuation = Vec4(0.1f, 0.1f, 0.1f, 1.0f);
	}
};

ConstantBufferVS CBufferVS;
ConstantBufferPS CBufferPS;

void AppShadow::Initialize()
{
	m_FloorMesh.CreateAsQuad(10.0f);
	m_BoxMesh.CreateAsCube(1.0f);
	m_QuadMesh.CreateAsQuad(0.3f, 0.9f, 0.5f, 0.4f);
	m_BulbMesh.Create("Lightbulb.obj");

	m_VertexShader.Create("Shadow.hlsl", "VSMain");
	m_PixelShader.Create("Shadow.hlsl", "PSMain");
	m_VertexShaderDepth.Create("Shadow.hlsl", "VSMain_Depth");
	m_PixelShaderDepth.Create("Shadow.hlsl", "PSMain_Depth");
	m_VertexShaderQuad.Create("Shadow.hlsl", "VSMain_Quad");
	m_PixelShaderQuad.Create("Shadow.hlsl", "PSMain_Quad");
	m_VertexShaderBulb.Create("Mesh.hlsl", "VSMain");
	m_PixelShaderBulb.Create("Mesh.hlsl", "PSMain");

	m_DiffuseTexFloor.Create("wood.dds");
	m_DiffuseTexBox.Create("WoodCrate01.dds");

	const uint32_t DepthTexSize = 1024U;
	D3DTexture2D depthTexture;
	depthTexture.Create(eR32_Typeless, DepthTexSize, DepthTexSize, D3DBuffer::eBindAsShaderResource | D3DBuffer::eBindAsDepthStencil);
	m_DepthTexture.CreateAsTexture(D3DView::eTexture2D, depthTexture, eR32_Float, 0U, 1U);
	m_DepthSurface.CreateAsTexture(D3DView::eTexture2D, depthTexture, eD32_Float, 0U, 0U);

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite, nullptr);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite, nullptr);
	m_CBufferVSBulb.CreateAsConstantBuffer(sizeof(Matrix), D3DBuffer::eGpuReadCpuWrite, nullptr);

	m_Viewport = { 0.0f, 0.0f, (float)m_Width, (float)m_Height, 0.0f, 1.0f };
	m_ViewportDepth = { 0.0f, 0.0f, (float)DepthTexSize, (float)DepthTexSize, 0.0f, 1.0f };

	m_Camera->SetViewRadius(15.0f);
	m_Camera->Move(0, 200);
}

void AppShadow::DrawClutter()
{
	CBufferPS.EyePos = m_Camera->GetEyePos();
	m_CBufferPS.Update(&CBufferPS, sizeof(ConstantBufferPS));

	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);

	CBufferVS.World = Matrix::Transpose(m_Camera->GetWorldMatrix());
	CBufferVS.WorldInverse = Matrix::InverseTranspose(m_Camera->GetWorldMatrix());
	CBufferVS.WVP = Matrix::Transpose(m_Camera->GetWVPMatrix());
	m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));
	D3DEngine::Instance().BindMesh(m_FloorMesh);
	D3DEngine::Instance().SetShaderResourceView(m_DiffuseTexFloor, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().DrawIndexed(m_FloorMesh.IndexCount, 0U, 0, eTriangleList);

	Matrix world = m_Camera->GetWorldMatrix() * Matrix::Translation(0.0f, 0.0f, -0.5f);
	CBufferVS.World = Matrix::Transpose(world);
	CBufferVS.WorldInverse = Matrix::InverseTranspose(world);
	CBufferVS.WVP = Matrix::Transpose(world * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));
	D3DEngine::Instance().BindMesh(m_BoxMesh);
	D3DEngine::Instance().SetShaderResourceView(m_DiffuseTexBox, 0U, D3DShader::ePixelShader);
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
	D3DEngine::Instance().SetVertexShader(m_VertexShaderQuad);
	D3DEngine::Instance().SetPixelShader(m_PixelShaderQuad);

	D3DEngine::Instance().BindMesh(m_QuadMesh);

	D3DEngine::Instance().SetShaderResourceView(m_DepthTexture, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().DrawIndexed(m_QuadMesh.IndexCount, 0U, 0, eTriangleList);
}

void AppShadow::DrawBulb(const Vec3 &pos)
{
	D3DEngine::Instance().SetVertexShader(m_VertexShaderBulb);
	D3DEngine::Instance().SetPixelShader(m_PixelShaderBulb);

	D3DEngine::Instance().BindMesh(m_BulbMesh);

	Matrix translation = Matrix::Translation(pos.x, pos.y, pos.z);
	Matrix scale = Matrix::Scaling(0.1f);
	Matrix rotate = Matrix::RotationAxis(1.0f, 0.0f, 0.0f, -90.0f);
	Matrix wvp = Matrix::Transpose(m_Camera->GetWorldMatrix() * rotate * scale * translation * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	m_CBufferVSBulb.Update(&wvp, sizeof(Matrix));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVSBulb, 0U, D3DShader::eVertexShader);

	D3DEngine::Instance().DrawIndexed(m_BulbMesh.IndexCount, 0U, 0, eTriangleList);
}

void AppShadow::RenderScene()
{
	D3DRenderTargetView EmptyRTV;
	D3DEngine::Instance().SetViewport(m_ViewportDepth);
	D3DEngine::Instance().ClearDepthStencilView(m_DepthSurface, D3DDepthStencilView::eDepthStencil, 1.0f, 0U);
	D3DEngine::Instance().SetRenderTargetView(EmptyRTV, 0U);
	D3DEngine::Instance().SetDepthStencilView(m_DepthSurface);
	D3DEngine::Instance().SetVertexShader(m_VertexShaderDepth);
	D3DEngine::Instance().SetPixelShader(m_PixelShaderDepth);
	DrawClutter();

	D3DEngine::Instance().SetViewport(m_Viewport);
	D3DEngine::Instance().ResetRenderSurfaces();
	D3DEngine::Instance().ClearRenderSurfaces();
	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);
	DrawClutter();

	DrawBulb(CBufferPS.Light.Position);

	DrawQuad();

	ImGui::SliderFloat3("LightPos", (float *)&CBufferPS.Light.Position, -10.0f, 10.0f);
}

void AppShadow::ResizeWindow(uint32_t width, uint32_t height)
{
	Base::ResizeWindow(width, height);

	m_Viewport.Width = (float)m_Width;
	m_Viewport.Height = (float)m_Height;
}