#include "Cubemap.h"

#include "D3DGraphic.h"
#include "Camera.h"
#include "SkyBox.h"
#include "D3DModel.h"

#include <vector>

extern D3DGraphic* g_Renderer;

struct Vertex
{
	Vec3 Pos;
	Vec3 Normal;
	Vec2 UV;
};

struct GeometriesInfo
{
	uint32_t BoxIndexCount;
	uint32_t BoxIndexOffset;
	uint32_t BoxVertexOffset;

	uint32_t GridIndexCount;
	uint32_t GridIndexOffset;
	uint32_t GridVertexOffset;

	uint32_t SphereIndexCount;
	uint32_t SphereIndexOffset;
	uint32_t SphereVertexOffset;

	uint32_t CylinderIndexCount;
	uint32_t CylinderIndexOffset;
	uint32_t CylinderVertexOffset;
};

struct DemoCubeResource
{
	SkyBox Sky;
	SimpleMesh Skull;

	Ref<ID3D11Buffer> GeometriesVBuf;
	Ref<ID3D11Buffer> GeometriesIBuf;

	Ref<ID3D11ShaderResourceView> FloorTex;
	Ref<ID3D11ShaderResourceView> StoneTex;
	Ref<ID3D11ShaderResourceView> BrickTex;

	Ref<ID3D11ShaderResourceView> DynamicCubeMap;
};

static DemoCubeResource s_Resource;
static GeometriesInfo s_Geometries = { 0 };
static Camera s_CubemapCamera;
static float s_Radius = 12.0f;
static float s_Phi = DirectX::XM_PI * 0.42f;
static float s_Theta = DirectX::XM_PI * 1.24f;

void ApplicationCubemap::MakeGeometriesResource()
{
	Math::Geometry::Mesh box;
	Math::Geometry::MakeBox(1.0f, 1.0f, 1.0f, box);

	Math::Geometry::Mesh grid;
	Math::Geometry::MakeGrid(20.0f, 30.0f, 60U, 40U, grid);

	Math::Geometry::Mesh sphere;
	Math::Geometry::MakeSphere(0.5f, 20U, 20U, sphere);

	//Math::Geometry::Mesh cylinder;
	//Math::Geometry::MakeCylinder(0.5f, 0.3f, 3.0f, 20U, 20U, cylinder);

	uint32_t totalVertexCount = (uint32_t)(box.Vertices.size() + grid.Vertices.size() + sphere.Vertices.size());
	uint32_t totalIndexCount = (uint32_t)(box.Indices.size() + grid.Indices.size() + sphere.Indices.size());
	std::vector<Vertex> vertices(totalVertexCount);
	uint32_t k = 0U;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].UV = box.Vertices[i].UV;
	}
	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
		vertices[k].UV = grid.Vertices[i].UV;
	}
	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Normal = sphere.Vertices[i].Normal;
		vertices[k].UV = sphere.Vertices[i].UV;
	}
	//for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	//{
	//	vertices[k].Pos = cylinder.Vertices[i].Position;
	//	vertices[k].Normal = cylinder.Vertices[i].Normal;
	//	vertices[k].UV = cylinder.Vertices[i].UV;
	//}

	std::vector<uint32_t> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
	///indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());

	s_Geometries.BoxIndexCount = (uint32_t)box.Indices.size();
	s_Geometries.BoxIndexOffset = 0U;
	s_Geometries.BoxVertexOffset = 0U;

	s_Geometries.GridIndexCount = (uint32_t)grid.Indices.size();
	s_Geometries.GridIndexOffset = s_Geometries.BoxIndexCount;
	s_Geometries.GridVertexOffset = s_Geometries.BoxVertexOffset;

	s_Geometries.SphereIndexCount = (uint32_t)sphere.Indices.size();
	s_Geometries.SphereIndexOffset = s_Geometries.GridIndexOffset + s_Geometries.GridIndexCount;
	s_Geometries.SphereVertexOffset = s_Geometries.GridVertexOffset + (uint32_t)sphere.Vertices.size();

	g_Renderer->CreateVertexBuffer(s_Resource.GeometriesVBuf.Reference(), sizeof(Vertex) * totalVertexCount,
		D3D11_USAGE_IMMUTABLE, &vertices[0]);
	g_Renderer->CreateIndexBuffer(s_Resource.GeometriesIBuf.Reference(), sizeof(uint32_t) * totalIndexCount,
		D3D11_USAGE_IMMUTABLE, &indices[0]);
}

void ApplicationCubemap::SetupScene()
{
	MakeGeometriesResource();

	g_Renderer->CreateShaderResourceView(s_Resource.BrickTex.Reference(), "bricks.dds");
	g_Renderer->CreateShaderResourceView(s_Resource.FloorTex.Reference(), "floor.dds");
	g_Renderer->CreateShaderResourceView(s_Resource.StoneTex.Reference(), "stone.dds");

	s_Resource.Sky.Create("sunsetcube1024.dds", 5000.0f);

	s_Resource.Skull.CreateFromTxt("skull.txt");
	s_Resource.Skull.SetLightCount(3U);

	s_CubemapCamera.Translation(0.0f, 2.0f, -15.0f);

	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	D3D11_VIEWPORT viewport;
	viewport.Width = (float)m_Width;
	viewport.Height = (float)m_Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = viewport.TopLeftY = 0.0f;
	g_Renderer->SetViewports(&viewport);

	m_bInited = true;
}

void ApplicationCubemap::DrawGeometries()
{

}

void ApplicationCubemap::RenderScene()
{
	g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);

	s_Resource.Sky.Draw(s_CubemapCamera);

	s_Resource.Skull.Draw(s_CubemapCamera);
}

void ApplicationCubemap::UpdateScene(float /*elapsedTime*/, float totalTime)
{
	float x = s_Radius * sinf(s_Phi) * cosf(s_Theta);
	float z = s_Radius * sinf(s_Phi) * sinf(s_Theta);
	float y = s_Radius * cosf(s_Phi);

	Vec3 eyePos(x, y, z);
	Vec3 lookAt(0.0f, 0.0f, 0.0f);
	s_CubemapCamera.SetViewParams(eyePos, lookAt);

	Matrix skullScale = Matrix::Scaling(0.3f, 0.3f, 0.3f);
	Matrix skullOffset = Matrix::Translation(3.0f, 2.0f, 0.0f);
	Matrix skullRotateLocal = Matrix::RotationAxis(0.0f, 1.0f, 0.0f, 2.0f * totalTime);
	Matrix skullRotateGlobal = Matrix::RotationAxis(0.0f, 1.0f, 0.0f, 0.5f * totalTime);
	Matrix skullWorld = skullScale * skullRotateLocal * skullOffset * skullRotateGlobal;  /// ??? 
	s_Resource.Skull.SetWorldMatrix(skullWorld);
}

void ApplicationCubemap::ResizeWindow(uint32_t width, uint32_t height)
{
	s_CubemapCamera.SetProjParams(DirectX::XM_PIDIV4, (float)width / height, 1.0f, 1000.0f);

	Base::ResizeWindow(width, height);
}

void ApplicationCubemap::MouseMove(WPARAM wParam, int x, int y)
{
	if ((wParam & MK_LBUTTON) != 0)
	{
		float dx = DirectX::XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos[0]));
		float dy = DirectX::XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos[1]));

		s_Theta += dx;
		s_Phi += dy;

		s_Phi = Math::Clamp(s_Phi, 0.1f, DirectX::XM_PI - 0.1f);
	}

	m_LastMousePos[0] = x;
	m_LastMousePos[1] = y;
}