#include "DeferredShading.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "D3DGUI_imGui.h"

void AppDeferredShading::Initialize()
{
	m_Bunny.Create("bunny.sdkmesh");

	m_GBuffer.Init(m_Width, m_Height);
	m_LightController.Init();

	m_VertexShader.Create("DeferredShading\\DeferredShading.hlsl", "VSMain");
	m_PixelShader.Create("DeferredShading\\DeferredShading.hlsl", "PSMain");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite);

	m_Camera->SetViewRadius(100.0f);
	m_Camera->Move(0, -100);
}

void AppDeferredShading::RenderScene()
{
	ConstantBufferVS CBufferVS;
	ConstantBufferPS CBufferPS;

    D3DEngine::Instance().SetViewport(D3DViewport(0.0f, 0.0f, (float)m_Width, (float)m_Height));

	CBufferVS.World = Matrix::Transpose(m_Camera->GetWorldMatrix());
	CBufferVS.WVP = Matrix::Transpose(m_Camera->GetWVPMatrix());
	m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));

	CBufferPS.LightSpecular = m_LightController.GetLightSpecular((Light::eLightType)m_LightingType);
	m_CBufferPS.Update(&CBufferPS, sizeof(ConstantBufferPS));

	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

	m_GBuffer.Bind();
	m_Bunny.Draw(false);
	m_GBuffer.UnBind();

	D3DEngine::Instance().ResetDefaultRenderTargetView();

	Matrix proj = m_Camera->GetProjMatrix();
	Vec4 perspective = Vec4(1.0f / proj._11, 1.0f / proj._22, proj._43, -proj._33);

	m_LightController.TurnonTheLights((Light::eLightType)m_LightingType, m_GBuffer, perspective, m_Camera->GetViewMatrix());

	m_GBuffer.VisulizeGBuffer(m_bVisualizeGBuffer, perspective);

	ImGui::Checkbox("VisualizeGBuffer", &m_bVisualizeGBuffer);
	ImGui::Combo("LightingType", &m_LightingType, "Point\0Directional\0Spot\0Capsule");
}

void AppDeferredShading::ResizeWindow(uint32_t width, uint32_t height)
{
	Base::ResizeWindow(width, height);

	m_GBuffer.Resize(width, height);
}
