#include "Box.h"
#include "D3DGraphic.h"
#include "Camera.h"
#include "D3DGeometry.h"

extern D3DGraphic* g_Renderer;

static Geometry::Mesh s_Box;
static Ref<ID3D11ShaderResourceView> tex;

void ApplicationBox::SetupScene()
{
	Geometry::Mesh::MakeCube(1.0f, s_Box);

	g_Renderer->CreateShaderResourceView(tex, "WoodCrate01.dds");
	Geometry::Material mat;
	mat.SetTexture(Geometry::Material::eDiffuseMap, tex);
	s_Box.SetMaterial(&mat);

	m_Camera->SetViewRadius(5.0f);
}

void ApplicationBox::RenderScene()
{
	s_Box.Draw(*m_Camera);
}