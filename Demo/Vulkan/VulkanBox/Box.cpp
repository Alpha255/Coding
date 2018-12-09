#include "Box.h"
#include "Camera.h"
#include "VulkanShader.h"
#include "VulkanEngine.h"

void AppBox::Initialize()
{
	m_Camera->SetViewRadius(5.0f);

	m_Box.CreateAsCube(1.0f);

	VulkanVertexShader vsTest;
	vsTest.Create("VulkanBox.glsl");
}

void AppBox::RenderScene()
{
	VulkanEngine::Instance().ClearRenderSurface();
}