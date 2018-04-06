#include "Box.h"
#include "Camera.h"

void AppBox::Initialize()
{
	m_Box.CreateAsCube(1.0f);

	D3DShaderResourceView tex;
	tex.Create("WoodCrate01.dds");

	Material mat;
	mat.SetTexture(Material::eDiffuseMap, tex);
	
	m_Box.SetMaterial(mat);

	m_Camera->SetViewRadius(5.0f);
}

void AppBox::RenderScene()
{
	m_Box.Draw(*m_Camera);
}