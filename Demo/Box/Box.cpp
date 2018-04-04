#include "Box.h"
#include "Camera.h"
#include "D3DGeometry.h"

static Geometry::Mesh s_Box;

void AppBox::PreInit()
{
	s_Box.CreateAsCube(1.0f);

	D3DShaderResourceView tex;
	tex.Create("WoodCrate01.dds");
	Material mat;
	mat.SetTexture(Material::eDiffuseMap, tex);
	s_Box.SetMaterial(&mat);

	m_Camera->SetViewRadius(5.0f);
}

void AppBox::RenderScene()
{
	s_Box.Draw(*m_Camera);
}