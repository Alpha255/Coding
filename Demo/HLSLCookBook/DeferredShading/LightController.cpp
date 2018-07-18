#include "LightController.h"

void LightController::Init()
{
	m_DirLight.Diffuse = Vec4(0.85f, 0.8f, 0.5f, 0.0f);
	m_DirLight.Direction = Vec4(1.0f, -1.0f, 1.0f, 0.0f);
	m_DirLight.Specular = Vec3(0.25f, 0.25f, 0.25f);
	m_DirLight.SpecularIntensity = 250.0f;

	m_Inited = true;
}

void LightController::PointLighting()
{
}

void LightController::DirectionalLighting()
{
}

void LightController::SpotLighting()
{
}

void LightController::CapsuleLighting()
{
}
