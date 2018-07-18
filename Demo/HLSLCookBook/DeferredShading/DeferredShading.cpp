#include "DeferredShading.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "D3DGUI_imGui.h"

struct ConstantBufferVS
{
	Matrix World;
	Matrix WVP;
};

struct ConstantBufferPS
{
	DirectionalLight DirLight;
};

void AppDeferredShading::Initialize()
{
	m_Bunny.Create("bunny.sdkmesh");

	m_GBuffer.Init(m_Width, m_Height);
	m_LightController.Init();

	m_VertexShader.Create("DeferredShading\\DeferredShading.hlsl", "VSMain");
	m_PixelShader.Create("DeferredShading\\DeferredShading.hlsl", "PSMain");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite);
}

void AppDeferredShading::RenderScene()
{
    D3DEngine::Instance().ResetDefaultRenderSurfaces();
    D3DEngine::Instance().SetViewport(D3DViewport(0.0f, 0.0f, (float)m_Width, (float)m_Height));
}

void AppDeferredShading::ResizeWindow(uint32_t width, uint32_t height)
{
	Base::ResizeWindow(width, height);

	m_GBuffer.Resize(width, height);
}
