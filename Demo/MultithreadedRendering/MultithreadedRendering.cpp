
#include "MultithreadedRendering.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "D3DGUI_imGui.h"

struct ConstantBufferVS
{
	Matrix WVP;
	Matrix World;
	Matrix WorldInverse;
};

ConstantBufferVS g_CBufferVS;

void AppMultithreadedRendering::Initialize()
{
	m_SquidRoom.Create("crypt.sdkmesh");

	m_VertexShader.Create("MultithreadedRendering.hlsl", "VSMain");
	m_PixelShader.Create("MultithreadedRendering.hlsl", "PSMain");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite);
}

void AppMultithreadedRendering::RenderScene()
{
	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);

	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	Matrix world = m_Camera->GetWorldMatrix();
	g_CBufferVS.WVP = Matrix::Transpose(world * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	g_CBufferVS.World = Matrix::Transpose(world);
	g_CBufferVS.WorldInverse = Matrix::InverseTranspose(world);
	m_CBufferVS.Update(&g_CBufferVS, sizeof(ConstantBufferVS));

	m_SquidRoom.Draw(false);

	ImGui::Text("%.2f FPS", m_FPS);
}
