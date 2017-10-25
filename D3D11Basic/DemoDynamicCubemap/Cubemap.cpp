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
	int32_t BoxVertexOffset;

	uint32_t GridIndexCount;
	uint32_t GridIndexOffset;
	int32_t GridVertexOffset;

	uint32_t SphereIndexCount;
	uint32_t SphereIndexOffset;
	int32_t SphereVertexOffset;

	uint32_t CylinderIndexCount;
	uint32_t CylinderIndexOffset;
	int32_t CylinderVertexOffset;

	Matrix WorldGrid;
	Matrix WorldBox;
	Matrix WorldCenterSphere;
	Matrix WorldSphere[10];

	Lighting::Material MatGrid;
	Lighting::Material MatBox;
	Lighting::Material MatSphere;
	Lighting::Material MatCenterSphere;
};

struct ConstantsBufVS
{
	Matrix World;
	Matrix WorldInverse;
	Matrix WVP;
};

struct ConstantsBufPS
{
	Vec3 EyePos;
	uint32_t EnableReflection;

	Lighting::DirectionalLight DirLights[3];
	Lighting::Material Mat;
};

struct DemoCubeResource
{
	SkyBox Sky;
	SimpleMesh Skull;

	Ref<ID3D11Buffer> ConstantsBufVS;
	Ref<ID3D11Buffer> ConstantsBufPS;

	Ref<ID3D11VertexShader> VertexShader;
	Ref<ID3D11PixelShader> PixelShader;

	Ref<ID3D11InputLayout> Layout;

	Ref<ID3D11Buffer> GeometriesVBuf;
	Ref<ID3D11Buffer> GeometriesIBuf;

	Ref<ID3D11ShaderResourceView> FloorTex;
	Ref<ID3D11ShaderResourceView> StoneTex;
	Ref<ID3D11ShaderResourceView> BrickTex;

	Ref<ID3D11ShaderResourceView> DynamicCubeMap;
	Ref<ID3D11RenderTargetView> DynamicCubemapRTVs[6];
	Ref<ID3D11DepthStencilView> DynamicCubemapDSV;

	Ref<ID3D11SamplerState> Sampler;

	D3D11_VIEWPORT DynamicCubemapVP;

	D3D11_VIEWPORT VP;
};

static DemoCubeResource s_Resource;
static GeometriesInfo s_Geometries = { 0 };
static Camera s_CubemapCamera[6];
static Camera s_Camera;
static ConstantsBufPS s_CBPS;
static float s_Radius = 12.0f;
static float s_Phi = DirectX::XM_PI * 0.42f;
static float s_Theta = DirectX::XM_PI * 1.24f;

void ApplicationCubemap::InitGeometriesResource()
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
	for (size_t i = 0U; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].UV = box.Vertices[i].UV;
	}
	for (size_t i = 0U; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
		vertices[k].UV = grid.Vertices[i].UV;
	}
	for (size_t i = 0U; i < sphere.Vertices.size(); ++i, ++k)
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
	s_Geometries.BoxVertexOffset = 0;

	s_Geometries.GridIndexCount = (uint32_t)grid.Indices.size();
	s_Geometries.GridIndexOffset = s_Geometries.BoxIndexCount;
	s_Geometries.GridVertexOffset = (int32_t)box.Vertices.size();

	s_Geometries.SphereIndexCount = (uint32_t)sphere.Indices.size();
	s_Geometries.SphereIndexOffset = s_Geometries.GridIndexOffset + s_Geometries.GridIndexCount;
	s_Geometries.SphereVertexOffset = s_Geometries.GridVertexOffset + (int32_t)grid.Vertices.size();

	g_Renderer->CreateVertexBuffer(s_Resource.GeometriesVBuf.Reference(), sizeof(Vertex) * totalVertexCount,
		D3D11_USAGE_IMMUTABLE, &vertices[0]);
	g_Renderer->CreateIndexBuffer(s_Resource.GeometriesIBuf.Reference(), sizeof(uint32_t) * totalIndexCount,
		D3D11_USAGE_IMMUTABLE, &indices[0]);

	g_Renderer->CreateShaderResourceView(s_Resource.BrickTex.Reference(), "bricks.dds");
	g_Renderer->CreateShaderResourceView(s_Resource.FloorTex.Reference(), "floor.dds");
	g_Renderer->CreateShaderResourceView(s_Resource.StoneTex.Reference(), "stone.dds");

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	char *const pShaderName = "Cubemap.hlsl";

	g_Renderer->CreateVertexShaderAndInputLayout(s_Resource.VertexShader.Reference(), s_Resource.Layout.Reference(), 
		layout, ARRAYSIZE(layout), pShaderName, "VS_Main");
	g_Renderer->CreatePixelShader(s_Resource.PixelShader.Reference(), pShaderName, "PS_Main");

	g_Renderer->CreateConstantBuffer(s_Resource.ConstantsBufVS.Reference(), sizeof(ConstantsBufVS), 
		D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);
	g_Renderer->CreateConstantBuffer(s_Resource.ConstantsBufPS.Reference(), sizeof(ConstantsBufPS),
		D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

	s_Resource.VP.Width = (float)m_Width;
	s_Resource.VP.Height = (float)m_Height;
	s_Resource.VP.MinDepth = 0.0f;
	s_Resource.VP.MaxDepth = 1.0f;
	s_Resource.VP.TopLeftX = s_Resource.VP.TopLeftY = 0.0f;
	g_Renderer->SetViewports(&s_Resource.VP);

	s_Geometries.WorldGrid.Identity();
	s_Geometries.WorldBox = Matrix::Scaling(3.0f, 1.0f, 3.0f) * Matrix::Translation(0.0f, 0.5f, 0.0f);
	s_Geometries.WorldCenterSphere = Matrix::Scaling(2.0f, 2.0f, 2.0f) * Matrix::Translation(0.0f, 2.0f, 0.0f);
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

	s_Geometries.MatCenterSphere.Ambient = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
	s_Geometries.MatCenterSphere.Diffuse = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
	s_Geometries.MatCenterSphere.Specular = Vec4(0.8f, 0.8f, 0.8f, 16.0f);
	s_Geometries.MatCenterSphere.Reflect = Vec4(0.8f, 0.8f, 0.8f, 1.0f);

	memset(&s_CBPS, 0, sizeof(ConstantsBufPS));
	s_CBPS.DirLights[0].Ambient = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
	s_CBPS.DirLights[0].Diffuse = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
	s_CBPS.DirLights[0].Specular = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
	s_CBPS.DirLights[0].Direction = Vec4(0.57735f, -0.57735f, 0.57735f, 0.0f);

	s_CBPS.DirLights[1].Ambient = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	s_CBPS.DirLights[1].Diffuse = Vec4(0.20f, 0.20f, 0.20f, 1.0f);
	s_CBPS.DirLights[1].Specular = Vec4(0.25f, 0.25f, 0.25f, 1.0f);
	s_CBPS.DirLights[1].Direction = Vec4(-0.57735f, -0.57735f, 0.57735f, 0.0f);

	s_CBPS.DirLights[2].Ambient = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	s_CBPS.DirLights[2].Diffuse = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
	s_CBPS.DirLights[2].Specular = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	s_CBPS.DirLights[2].Direction = Vec4(0.0f, -0.707f, -0.707f, 0.0f);

	s_CBPS.EnableReflection = 1;

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
}

void ApplicationCubemap::InitCubemapCameras(float cx, float cy, float cz)
{
	Vec3 worldCenter = Vec3(cx, cy, cz);
	Vec3 upDir = Vec3(0.0f, 1.0f, 0.0f);

	Vec3 lookAts[6] =
	{
		Vec3(cx + 1.0f, cy, cz), /// +X
		Vec3(cx - 1.0f, cy, cz), /// -X
		Vec3(cx, cy + 1.0f, cz), /// +Y
		Vec3(cx, cy - 1.0f, cz), /// -Y
		Vec3(cx, cy, cz + 1.0f), /// +Z
		Vec3(cx, cy, cz - 1.0f), /// -Z
	};

	Vec3 upDirs[6] = 
	{
		Vec3(0.0f, 1.0f,  0.0f), /// +X
		Vec3(0.0f, 1.0f,  0.0f), /// -X
		Vec3(0.0f, 0.0f, -1.0f), /// +Y
		Vec3(0.0f, 0.0f,  1.0f), /// -Y
		Vec3(0.0f, 1.0f,  0.0f), /// +Z
		Vec3(0.0f, 1.0f,  0.0f), /// -Z
	};

	for (uint32_t i = 0U; i < 6U; ++i)
	{
		s_CubemapCamera[i].SetViewParams(worldCenter, lookAts[i], upDirs[i]);
		s_CubemapCamera[i].SetProjParams(DirectX::XM_PIDIV2, 1.0f, 0.1f, 1000.0f);
	}
}

void ApplicationCubemap::InitCubemapResource()
{
	static uint32_t s_CubemapSize = 1024U;
	
	Ref<ID3D11Texture2D> cubemapTex;
	g_Renderer->CreateTexture2D(cubemapTex.Reference(), DXGI_FORMAT_R8G8B8A8_UNORM, s_CubemapSize, s_CubemapSize,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0U, 6U, 0U, 1U, 0U,
		D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE);

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	memset(&rtvDesc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.ArraySize = 1U;
	rtvDesc.Texture2DArray.MipSlice = 0U;

	for (uint32_t i = 0U; i < 6U; ++i)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		g_Renderer->CreateRenderTargetView(s_Resource.DynamicCubemapRTVs[i].Reference(), cubemapTex.Ptr(), &rtvDesc);
	}

	g_Renderer->CreateShaderResourceView(s_Resource.DynamicCubeMap.Reference(), cubemapTex.Ptr(), 
		DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_SRV_DIMENSION_TEXTURECUBE);

	Ref<ID3D11Texture2D> depthTex;
	g_Renderer->CreateTexture2D(depthTex.Reference(), DXGI_FORMAT_D32_FLOAT, s_CubemapSize, s_CubemapSize, D3D11_BIND_DEPTH_STENCIL);
	g_Renderer->CreateDepthStencilView(s_Resource.DynamicCubemapDSV.Reference(), depthTex.Ptr(), DXGI_FORMAT_D32_FLOAT);

	s_Resource.DynamicCubemapVP.TopLeftX = 0.0f;
	s_Resource.DynamicCubemapVP.TopLeftY = 0.0f;
	s_Resource.DynamicCubemapVP.Width = (float)s_CubemapSize;
	s_Resource.DynamicCubemapVP.Height = (float)s_CubemapSize;
	s_Resource.DynamicCubemapVP.MinDepth = 0.0f;
	s_Resource.DynamicCubemapVP.MaxDepth = 1.0f;
}

void ApplicationCubemap::SetupScene()
{
	InitGeometriesResource();

	InitCubemapCameras(0.0f, 2.0f, 0.0f);
	InitCubemapResource();

	s_Resource.Sky.Create("sunsetcube1024.dds", 5000.0f);

	s_Resource.Skull.CreateFromTxt("skull.txt");
	s_Resource.Skull.SetLightCount(3U);

	s_Camera.Translation(0.0f, 2.0f, -15.0f);

	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	m_bInited = true;
}

void ApplicationCubemap::DrawScene(const Camera &cam, bool bDrawCenterSphere)
{
	s_Resource.Skull.Draw(cam);

	ConstantsBufVS cbVS;
	memset(&cbVS, 0, sizeof(ConstantsBufVS));

	Vec4 eyePos = cam.GetEyePos();
	s_CBPS.EyePos = Vec3(eyePos.x, eyePos.y, eyePos.z);

	Matrix world, view, proj, wvp;
	view = cam.GetViewMatrix();
	proj = cam.GetProjMatrix();

	g_Renderer->SetInputLayout(s_Resource.Layout.Ptr());
	
	g_Renderer->SetVertexShader(s_Resource.VertexShader.Ptr());
	g_Renderer->SetPixelShader(s_Resource.PixelShader.Ptr());

	g_Renderer->SetConstantBuffer(s_Resource.ConstantsBufVS.Ptr(), 0U, D3DGraphic::eVertexShader);
	g_Renderer->SetConstantBuffer(s_Resource.ConstantsBufPS.Ptr(), 0U, D3DGraphic::ePixelShader);

	g_Renderer->SetVertexBuffer(s_Resource.GeometriesVBuf.Ptr(), sizeof(Vertex), 0U);
	g_Renderer->SetIndexBuffer(s_Resource.GeometriesIBuf.Ptr(), DXGI_FORMAT_R32_UINT, sizeof(uint32_t));

	g_Renderer->SetSamplerStates(s_Resource.Sampler.Reference());
	
	{
		/// Draw Grid
		world = s_Geometries.WorldGrid;
		wvp = world * view * proj;
		cbVS.World = world.Transpose();
		cbVS.WorldInverse = cbVS.World.Inverse();
		cbVS.WVP = wvp.Transpose();
		g_Renderer->UpdateConstantBuffer(s_Resource.ConstantsBufVS.Ptr(), &cbVS, sizeof(ConstantsBufVS));

		memcpy(&s_CBPS.Mat, &s_Geometries.MatGrid, sizeof(Lighting::Material));
		g_Renderer->UpdateConstantBuffer(s_Resource.ConstantsBufPS.Ptr(), &s_CBPS, sizeof(ConstantsBufPS));

		g_Renderer->SetShaderResource(s_Resource.FloorTex.Reference());

		g_Renderer->DrawIndexed(s_Geometries.GridIndexCount, s_Geometries.GridIndexOffset, s_Geometries.GridVertexOffset);
	}

	{
		/// Draw Box
		//world = s_Geometries.WorldBox;
		//wvp = world * view * proj;
		//cbVS.World = world.Transpose();
		//cbVS.WorldInverse = cbVS.World.Inverse();
		//cbVS.WVP = wvp.Transpose();
		//g_Renderer->UpdateConstantBuffer(s_Resource.ConstantsBufVS.Ptr(), &cbVS, sizeof(ConstantsBufVS));

		//memcpy(&s_CBPS.Mat, &s_Geometries.MatBox, sizeof(Lighting::Material));
		//g_Renderer->UpdateConstantBuffer(s_Resource.ConstantsBufPS.Ptr(), &s_CBPS, sizeof(ConstantsBufPS));

		//g_Renderer->SetShaderResource(s_Resource.StoneTex.Reference());

		//g_Renderer->DrawIndexed(s_Geometries.BoxIndexCount, s_Geometries.BoxIndexOffset, s_Geometries.BoxVertexOffset);
	}

	{
		/// Draw Spheres
		//for (uint32_t i = 0U; i < 10U; ++i)
		//{
		//	world = s_Geometries.WorldSphere[i];
		//	wvp = world * view * proj;
		//	cbVS.World = world.Transpose();
		//	cbVS.WorldInverse = cbVS.World.Inverse();
		//	cbVS.WVP = wvp.Transpose();
		//	g_Renderer->UpdateConstantBuffer(s_Resource.ConstantsBufVS.Ptr(), &cbVS, sizeof(ConstantsBufVS));

		//	memcpy(&s_CBPS.Mat, &s_Geometries.MatSphere, sizeof(Lighting::Material));
		//	g_Renderer->UpdateConstantBuffer(s_Resource.ConstantsBufPS.Ptr(), &s_CBPS, sizeof(ConstantsBufPS));

		//	g_Renderer->SetShaderResource(s_Resource.StoneTex.Reference());

		//	g_Renderer->DrawIndexed(s_Geometries.SphereIndexCount, s_Geometries.SphereIndexOffset, s_Geometries.SphereVertexOffset);
		//}
	}

	if (bDrawCenterSphere)
	{
		//world = s_Geometries.WorldCenterSphere;
		//wvp = world * view * proj;
		//cbVS.World = world.Transpose();
		//cbVS.WorldInverse = cbVS.World.Inverse();
		//cbVS.WVP = wvp.Transpose();
		//g_Renderer->UpdateConstantBuffer(s_Resource.ConstantsBufVS.Ptr(), &cbVS, sizeof(ConstantsBufVS));

		//memcpy(&s_CBPS.Mat, &s_Geometries.MatCenterSphere, sizeof(Lighting::Material));
		//g_Renderer->UpdateConstantBuffer(s_Resource.ConstantsBufPS.Ptr(), &s_CBPS, sizeof(ConstantsBufPS));

		//g_Renderer->SetShaderResource(s_Resource.StoneTex.Reference());

		//g_Renderer->DrawIndexed(s_Geometries.SphereIndexCount, s_Geometries.SphereIndexOffset, s_Geometries.SphereVertexOffset);
	}

	s_Resource.Sky.Draw(cam);
}

void ApplicationCubemap::DrawDynamicCubemap()
{
	g_Renderer->SetViewports(&s_Resource.DynamicCubemapVP);

	for (uint32_t i = 0U; i < 6U; ++i)
	{
		g_Renderer->ClearRenderTarget(s_Resource.DynamicCubemapRTVs[i].Ptr(), reinterpret_cast<const float*>(&Color::Silver));
		g_Renderer->ClearDepthStencil(s_Resource.DynamicCubemapDSV.Ptr(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);

		g_Renderer->SetRenderTarget(s_Resource.DynamicCubemapRTVs[i].Ptr());
		g_Renderer->SetDepthStencil(s_Resource.DynamicCubemapDSV.Ptr());

		DrawScene(s_CubemapCamera[i]);
	}
}

void ApplicationCubemap::RenderScene()
{
	///DrawDynamicCubemap();

	g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);

	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	g_Renderer->SetViewports(&s_Resource.VP);

	DrawScene(s_Camera, true);
}

void ApplicationCubemap::UpdateScene(float /*elapsedTime*/, float totalTime)
{
	float x = s_Radius * sinf(s_Phi) * cosf(s_Theta);
	float z = s_Radius * sinf(s_Phi) * sinf(s_Theta);
	float y = s_Radius * cosf(s_Phi);

	Vec3 eyePos(x, y, z);
	Vec3 lookAt(0.0f, 0.0f, 0.0f);
	s_Camera.SetViewParams(eyePos, lookAt);

	Matrix skullScale = Matrix::Scaling(0.3f, 0.3f, 0.3f);
	Matrix skullOffset = Matrix::Translation(3.0f, 2.0f, 0.0f);
	Matrix skullRotateLocal = Matrix::RotationAxis(0.0f, 1.0f, 0.0f, 2.0f * totalTime);
	Matrix skullRotateGlobal = Matrix::RotationAxis(0.0f, 1.0f, 0.0f, 0.5f * totalTime);
	Matrix skullWorld = skullScale * skullRotateLocal * skullOffset * skullRotateGlobal;  /// ??? 
	s_Resource.Skull.SetWorldMatrix(skullWorld);
}

void ApplicationCubemap::ResizeWindow(uint32_t width, uint32_t height)
{
	s_Camera.SetProjParams(DirectX::XM_PIDIV4, (float)width / height, 1.0f, 1000.0f);

	s_Resource.VP.Width = (float)width;
	s_Resource.VP.Height = (float)height;

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