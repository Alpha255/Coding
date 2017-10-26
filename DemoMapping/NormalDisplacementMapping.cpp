#include "NormalDisplacementMapping.h"

#include "D3DGraphic.h"
#include "D3DModel.h"
#include "Camera.h"
#include "SkyBox.h"

/// https://cgcookie.com/2017/06/15/normal-vs-displacement-mapping-why-games-use-normals/
/// https://msdn.microsoft.com/en-us/library/windows/desktop/ff476340(v=vs.85).aspx

extern D3DGraphic* g_Renderer;

struct GeometriesInfo
{
	uint32_t BoxIndexCount;
	uint32_t BoxIndexOffset;
	int32_t BoxVertexOffset;

	uint32_t GridIndexCount;
	uint32_t GridIndexOffset;
	int32_t GridVertexOffset;

	uint32_t SphereIndexCount;
	uint32_t SphereIndexOffset;
	int32_t SphereVertexOffset;

	Matrix WorldGrid;
	Matrix WorldBox;
	Matrix WorldSphere[10];

	Lighting::Material MatGrid;
	Lighting::Material MatBox;
	Lighting::Material MatSphere;
};

struct DemoMappingResource
{
	SimpleMesh Skull;
	SkyBox Sky;

	Ref<ID3D11InputLayout> Layout;

	Ref<ID3D11Buffer> GeometriesVBuf;
	Ref<ID3D11Buffer> GeometriesIBuf;

	Ref<ID3D11Buffer> ConstantsBufVS;
	Ref<ID3D11Buffer> ConstantsBufPS;

	Ref<ID3D11VertexShader> VertexShader;
	Ref<ID3D11PixelShader> PixelShader;

	Ref<ID3D11HullShader> HullShader;
	Ref<ID3D11DomainShader> DomainShader;

	Ref<ID3D11SamplerState> Sampler;

	Ref<ID3D11ShaderResourceView> FloorTex;
	Ref<ID3D11ShaderResourceView> FloorNormalTex;
	Ref<ID3D11ShaderResourceView> BrickTex;
	Ref<ID3D11ShaderResourceView> BrickNormalTex;
};

struct ConstantsBufVS
{
	Matrix World;
	Matrix WorldInverse;
	Matrix WVP;
	Matrix TexTransform;
};

struct ConstantsBufPS
{
	Vec4 EyePos;

	Lighting::DirectionalLight DirLights[3];
	Lighting::Material Mat;
};

static float s_Radius = 12.0f;
static float s_Phi = DirectX::XM_PI * 0.42f;
static float s_Theta = DirectX::XM_PI * 1.24f;
static Camera s_Camera;
static GeometriesInfo s_Geometries = { 0 };
static DemoMappingResource s_Resource;
static ConstantsBufPS s_CBufPS;

void ApplicationMapping::InitGeometriesResource()
{
	Math::Geometry::Mesh box;
	Math::Geometry::MakeBox(1.0f, 1.0f, 1.0f, box);

	Math::Geometry::Mesh grid;
	Math::Geometry::MakeGrid(20.0f, 30.0f, 50U, 40U, grid);

	Math::Geometry::Mesh sphere;
	Math::Geometry::MakeSphere(0.5f, 20U, 20U, sphere);

	s_Geometries.BoxIndexCount = (uint32_t)box.Indices.size();
	s_Geometries.BoxIndexOffset = 0U;
	s_Geometries.BoxVertexOffset = 0;

	s_Geometries.GridIndexCount = (uint32_t)grid.Indices.size();
	s_Geometries.GridIndexOffset = s_Geometries.BoxIndexCount;
	s_Geometries.GridVertexOffset = (int32_t)box.Vertices.size();

	s_Geometries.SphereIndexCount = (uint32_t)sphere.Indices.size();
	s_Geometries.SphereIndexOffset = s_Geometries.GridIndexOffset + s_Geometries.GridIndexCount;
	s_Geometries.SphereVertexOffset = s_Geometries.GridVertexOffset + (int32_t)grid.Vertices.size();

	uint32_t totalVertexCount = (uint32_t)(box.Vertices.size() + grid.Vertices.size() + sphere.Vertices.size());
	uint32_t totalIndexCount = (uint32_t)(box.Indices.size() + grid.Indices.size() + sphere.Indices.size());

	std::vector<Math::Geometry::Vertex> vertices;
	vertices.insert(vertices.end(), box.Vertices.begin(), box.Vertices.end());
	vertices.insert(vertices.end(), grid.Vertices.begin(), grid.Vertices.end());
	vertices.insert(vertices.end(), sphere.Vertices.begin(), sphere.Vertices.end());

	std::vector<uint32_t> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());

	g_Renderer->CreateVertexBuffer(s_Resource.GeometriesVBuf.Reference(), sizeof(Math::Geometry::Vertex) * totalVertexCount,
		D3D11_USAGE_IMMUTABLE, &vertices[0]);
	g_Renderer->CreateIndexBuffer(s_Resource.GeometriesIBuf.Reference(), sizeof(uint32_t) * totalIndexCount,
		D3D11_USAGE_IMMUTABLE, &indices[0]);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	char *const pShaderName = "Mapping.hlsl";

	g_Renderer->CreateVertexShaderAndInputLayout(s_Resource.VertexShader.Reference(), 
		s_Resource.Layout.Reference(), layout, ARRAYSIZE(layout), pShaderName, "VS_Main");

	g_Renderer->CreatePixelShader(s_Resource.PixelShader.Reference(), pShaderName, "PS_Main");

	g_Renderer->CreateHullShader(s_Resource.HullShader.Reference(), pShaderName, "HS_Main");

	g_Renderer->CreateDomainShader(s_Resource.DomainShader.Reference(), pShaderName, "DS_Main");

	s_Geometries.WorldGrid.Identity();
	s_Geometries.WorldBox = Matrix::Scaling(3.0f, 1.0f, 3.0f) * Matrix::Translation(0.0f, 0.5f, 0.0f);
	for (uint32_t i = 0U; i < 5U; ++i)
	{
		s_Geometries.WorldSphere[i * 2 + 0] = Matrix::Translation(-5.0f, 3.5f, -10.0f + i * 5.0f);
		s_Geometries.WorldSphere[i * 2 + 1] = Matrix::Translation(+5.0f, 3.5f, -10.0f + i * 5.0f);
	}

	s_Geometries.MatGrid.Ambient = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
	s_Geometries.MatGrid.Diffuse = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
	s_Geometries.MatGrid.Specular = Vec4(0.8f, 0.8f, 0.8f, 16.0f);
	s_Geometries.MatGrid.Reflect = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

	s_Geometries.MatSphere.Ambient = Vec4(0.6f, 0.8f, 1.0f, 1.0f);
	s_Geometries.MatSphere.Diffuse = Vec4(0.6f, 0.8f, 1.0f, 1.0f);
	s_Geometries.MatSphere.Specular = Vec4(0.9f, 0.9f, 0.9f, 16.0f);
	s_Geometries.MatSphere.Reflect = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

	s_Geometries.MatBox.Ambient = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	s_Geometries.MatBox.Diffuse = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	s_Geometries.MatBox.Specular = Vec4(0.8f, 0.8f, 0.8f, 16.0f);
	s_Geometries.MatBox.Reflect = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

	memset(&s_CBufPS, 0, sizeof(ConstantsBufPS));
	s_CBufPS.DirLights[0].Ambient = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
	s_CBufPS.DirLights[0].Diffuse = Vec4(0.7f, 0.7f, 0.6f, 1.0f);
	s_CBufPS.DirLights[0].Specular = Vec4(0.8f, 0.8f, 0.7f, 1.0f);
	s_CBufPS.DirLights[0].Direction = Vec4(0.707f, 0.0f, 0.707f, 0.0f);

	s_CBufPS.DirLights[1].Ambient = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	s_CBufPS.DirLights[1].Diffuse = Vec4(0.40f, 0.40f, 0.40f, 1.0f);
	s_CBufPS.DirLights[1].Specular = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
	s_CBufPS.DirLights[1].Direction = Vec4(0.0f, -0.707f, 0.707f, 0.0f);

	s_CBufPS.DirLights[2].Ambient = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	s_CBufPS.DirLights[2].Diffuse = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
	s_CBufPS.DirLights[2].Specular = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
	s_CBufPS.DirLights[2].Direction = Vec4(-0.57735f, -0.57735f, -0.57735f, 0.0f);

	g_Renderer->CreateShaderResourceView(s_Resource.FloorTex.Reference(), "floor.dds");
	g_Renderer->CreateShaderResourceView(s_Resource.FloorNormalTex.Reference(), "floor_nmap.dds");
	g_Renderer->CreateShaderResourceView(s_Resource.BrickTex.Reference(), "bricks.dds");
	g_Renderer->CreateShaderResourceView(s_Resource.BrickNormalTex.Reference(), "bricks_nmap.dds");
}

void ApplicationMapping::SetupScene()
{
	InitGeometriesResource();

	s_Resource.Sky.Create("snowcube1024.dds", 5000.0f);

	s_Resource.Skull.CreateFromTxt("skull.txt");
	s_Resource.Skull.SetLightCount(3U);

	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	s_Camera.Translation(0.0f, 2.0f, -15.0f);

	D3D11_VIEWPORT viewport;
	viewport.Width = (float)m_Width;
	viewport.Height = (float)m_Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = viewport.TopLeftY = 0.0f;
	g_Renderer->SetViewports(&viewport);

	D3D11_SAMPLER_DESC sampDesc;
	memset(&sampDesc, 0, sizeof(D3D11_SAMPLER_DESC));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0.0f;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	g_Renderer->CreateSamplerState(s_Resource.Sampler.Reference(), &sampDesc);

	m_bInited = true;
}

void ApplicationMapping::RenderScene()
{
	g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget());
}

void ApplicationMapping::UpdateScene(float /*elapsedTime*/, float /*totalTime*/)
{
	float x = s_Radius * sinf(s_Phi) * cosf(s_Theta);
	float z = s_Radius * sinf(s_Phi) * sinf(s_Theta);
	float y = s_Radius * cosf(s_Phi);

	Vec3 eyePos(x, y, z);
	Vec3 lookAt(0.0f, 0.0f, 0.0f);
	s_Camera.SetViewParams(eyePos, lookAt);
}

void ApplicationMapping::ResizeWindow(uint32_t width, uint32_t height)
{
	s_Camera.SetProjParams(DirectX::XM_PIDIV4, (float)width / height, 1.0f, 1000.0f);

	Base::ResizeWindow(width, height);
}

void ApplicationMapping::MouseMove(WPARAM wParam, int x, int y)
{
	if ((wParam & MK_LBUTTON) != 0)
	{
		float dx = DirectX::XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos[0]));
		float dy = DirectX::XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos[1]));

		s_Theta += dx;
		s_Phi += dy;

		s_Phi = Math::Clamp(s_Phi, 0.1f, DirectX::XM_PI - 0.1f);
	}
	else if ((wParam & MK_RBUTTON) != 0)
	{
		float dx = 0.01f * static_cast<float>(x - m_LastMousePos[0]);
		float dy = 0.01f * static_cast<float>(y - m_LastMousePos[1]);

		s_Radius += dx - dy;

		s_Radius = Math::Clamp(s_Radius, 3.0f, 50.0f);
	}

	m_LastMousePos[0] = x;
	m_LastMousePos[1] = y;
}