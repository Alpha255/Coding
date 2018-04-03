#include "Box.h"
#include "Camera.h"
#include "D3DGeometry.h"

static Geometry::Mesh s_Box;

void AppBox::PreInit()
{
	Geometry::Mesh::MakeCube(1.0f, s_Box);

	D3DShaderResourceView tex;
	tex.Create("WoodCrate01.dds");
	Geometry::Material mat;
	mat.SetTexture(Geometry::Material::eDiffuseMap, tex);
	s_Box.SetMaterial(&mat);

	m_Camera->SetViewRadius(5.0f);
}

void AppBox::RenderScene()
{
	s_Box.Draw(*m_Camera);
}