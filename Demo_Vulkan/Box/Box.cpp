#include "Box.h"
#include "Camera.h"
#include "VulkanShader.h"

void AppBox::Initialize()
{
	m_Camera->SetViewRadius(5.0f);

	VulkanVertexShader vsTest;
	vsTest.Create("VulkanBox.glsl", "main");
}

void AppBox::RenderScene()
{
}