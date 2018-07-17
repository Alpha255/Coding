
#include "DeferredShading.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "D3DGUI_imGui.h"

void AppDeferredShading::Initialize()
{
	m_GBuffer.Init(m_Width, m_Height);
	m_LightController.Init();
}

void AppDeferredShading::RenderScene()
{
    D3DEngine::Instance().ResetDefaultRenderSurfaces();
    D3DEngine::Instance().SetViewport(D3DViewport(0.0f, 0.0f, (float)m_Width, (float)m_Height));
}
