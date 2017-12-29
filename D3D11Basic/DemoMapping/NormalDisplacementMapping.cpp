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

	uint32_t CylinderIndexCount;
	uint32_t CylinderIndexOffset;
	int32_t CylinderVertexOffset;

	Matrix WorldGrid;
	Matrix WorldBox;
	Matrix WorldSphere[10];
	Matrix WorldCylinder[10];

	Lighting::Material MatGrid;
	Lighting::Material MatBox;
	Lighting::Material MatSphere;
	Lighting::Material MatCylinder;
};

struct DemoMappingResource
{
	SDKMesh Skull;
	SkyBox Sky;

	Ref<ID3D11InputLayout> Layout;

	Ref<ID3D11Buffer> GeometriesVBuf;
	Ref<ID3D11Buffer> GeometriesIBuf;

	Ref<ID3D11Buffer> ConstantsBufVS;
	Ref<ID3D11Buffer> ConstantsBufPS;

	Ref<ID3D11VertexShader> NormalMapVS;
	Ref<ID3D11PixelShader> NormalMapPS;

	Ref<ID3D11VertexShader> DisplacementMapVS;
	Ref<ID3D11PixelShader> DisplacementMapPS;
	Ref<ID3D11HullShader> DisplacementMapHS;
	Ref<ID3D11DomainShader> DisplacementMapDS;

	Ref<ID3D11SamplerState> Sampler;

	Ref<ID3D11RasterizerState> Wireframe;

	Ref<ID3D11ShaderResourceView> FloorTex;
	Ref<ID3D11ShaderResourceView> FloorNormalTex;
	Ref<ID3D11ShaderResourceView> BrickTex;
	Ref<ID3D11ShaderResourceView> BrickNormalTex;
};

struct ConstantsBufVS
{
	Matrix World;
	Matrix WorldInverse;
	Matrix VP;
	Matrix WVP;
	Matrix TexTransform;
};

struct ConstantsBufPS
{
	Lighting::DirectionalLight DirLights[3];
	Lighting::Material Mat;

	Vec3 EyePos;
	float HeightScale = 0.07f;

	float MinTessDistance = 1.0f;
	float MaxTessDistance = 25.0f;
	float MinTessFactor = 1.0f;
	float MaxTessFactor = 5.0f;
};

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
	///Math::Geometry::MakeGeoSphere(0.5f, 3U, sphere);

	Math::Geometry::Mesh cylinder;
	Math::Geometry::MakeCylinder(0.5f, 0.5f, 3.0f, 15U, 15U, cylinder);

	s_Geometries.BoxIndexCount = (uint32_t)box.Indices.size();
	s_Geometries.BoxIndexOffset = 0U;
	s_Geometries.BoxVertexOffset = 0;

	s_Geometries.GridIndexCount = (uint32_t)grid.Indices.size();
	s_Geometries.GridIndexOffset = s_Geometries.BoxIndexCount;
	s_Geometries.GridVertexOffset = (int32_t)box.Vertices.size();

	s_Geometries.SphereIndexCount = (uint32_t)sphere.Indices.size();
	s_Geometries.SphereIndexOffset = s_Geometries.GridIndexOffset + s_Geometries.GridIndexCount;
	s_Geometries.SphereVertexOffset = s_Geometries.GridVertexOffset + (int32_t)grid.Vertices.size();

	s_Geometries.CylinderIndexCount = (uint32_t)cylinder.Indices.size();
	s_Geometries.CylinderIndexOffset = s_Geometries.SphereIndexOffset + s_Geometries.SphereIndexCount;
	s_Geometries.CylinderVertexOffset = s_Geometries.SphereVertexOffset + (int32_t)sphere.Vertices.size();

	uint32_t totalVertexCount = (uint32_t)(box.Vertices.size() + grid.Vertices.size() + sphere.Vertices.size() + cylinder.Vertices.size());
	uint32_t totalIndexCount = (uint32_t)(box.Indices.size() + grid.Indices.size() + sphere.Indices.size() + cylinder.Indices.size());

	std::vector<Math::Geometry::Vertex> vertices;
	vertices.insert(vertices.end(), box.Vertices.begin(), box.Vertices.end());
	vertices.insert(vertices.end(), grid.Vertices.begin(), grid.Vertices.end());
	vertices.insert(vertices.end(), sphere.Vertices.begin(), sphere.Vertices.end());
	vertices.insert(vertices.end(), cylinder.Vertices.begin(), cylinder.Vertices.end());

	std::vector<uint32_t> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
	indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());

	g_Renderer->CreateVertexBuffer(s_Resource.GeometriesVBuf, sizeof(Math::Geometry::Vertex) * totalVertexCount,
		D3D11_USAGE_IMMUTABLE, &vertices[0]);
	g_Renderer->CreateIndexBuffer(s_Resource.GeometriesIBuf, sizeof(uint32_t) * totalIndexCount,
		D3D11_USAGE_IMMUTABLE, &indices[0]);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	char *const pShaderName = "Mapping.hlsl";

	g_Renderer->CreateVertexShaderAndInputLayout(s_Resource.NormalMapVS, 
		s_Resource.Layout, layout, ARRAYSIZE(layout), pShaderName, "VS_Main_NormalMap");
	g_Renderer->CreatePixelShader(s_Resource.NormalMapPS, pShaderName, "PS_Main_NormalMap");

	g_Renderer->CreateVertexShader(s_Resource.DisplacementMapVS, pShaderName, "VS_Main_DisplacementMap");
	g_Renderer->CreatePixelShader(s_Resource.DisplacementMapPS, pShaderName, "PS_Main_DisplacementMap");
	g_Renderer->CreateHullShader(s_Resource.DisplacementMapHS, pShaderName, "HS_Main_DisplacementMap");
	g_Renderer->CreateDomainShader(s_Resource.DisplacementMapDS, pShaderName, "DS_Main_DisplacementMap");

	g_Renderer->CreateRasterizerState(s_Resource.Wireframe, D3D11_FILL_WIREFRAME);

	g_Renderer->CreateConstantBuffer(s_Resource.ConstantsBufVS, sizeof(ConstantsBufVS), 
		D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);
	g_Renderer->CreateConstantBuffer(s_Resource.ConstantsBufPS, sizeof(ConstantsBufPS),
		D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

	s_Geometries.WorldGrid.Identity();
	s_Geometries.WorldBox = Matrix::Scaling(3.0f, 1.0f, 3.0f) * Matrix::Translation(0.0f, 0.5f, 0.0f);
	for (uint32_t i = 0U; i < 5U; ++i)
	{
		s_Geometries.WorldSphere[i * 2 + 0] = Matrix::Translation(-5.0f, 3.5f, -10.0f + i * 5.0f);
		s_Geometries.WorldSphere[i * 2 + 1] = Matrix::Translation(+5.0f, 3.5f, -10.0f + i * 5.0f);

		s_Geometries.WorldCylinder[i * 2 + 0] = Matrix::Translation(-5.0f, 1.5f, -10.0f + i * 5.0f);
		s_Geometries.WorldCylinder[i * 2 + 1] = Matrix::Translation(+5.0f, 1.5f, -10.0f + i * 5.0f);
	}

	s_Geometries.MatGrid.Ambient = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
	s_Geometries.MatGrid.Diffuse = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
	s_Geometries.MatGrid.Specular = Vec4(0.8f, 0.8f, 0.8f, 16.0f);
	s_Geometries.MatGrid.Reflect = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

	s_Geometries.MatSphere.Ambient = Vec4(0.6f, 0.8f, 1.0f, 1.0f);
	s_Geometries.MatSphere.Diffuse = Vec4(0.6f, 0.8f, 1.0f, 1.0f);
	s_Geometries.MatSphere.Specular = Vec4(0.9f, 0.9f, 0.9f, 16.0f);
	s_Geometries.MatSphere.Reflect = Vec4(0.4f, 0.4f, 0.4f, 1.0f);

	s_Geometries.MatBox.Ambient = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	s_Geometries.MatBox.Diffuse = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	s_Geometries.MatBox.Specular = Vec4(0.8f, 0.8f, 0.8f, 16.0f);
	s_Geometries.MatBox.Reflect = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

	s_Geometries.MatCylinder.Ambient = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	s_Geometries.MatCylinder.Diffuse = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	s_Geometries.MatCylinder.Specular = Vec4(1.0f, 1.0f, 1.0f, 32.0f);
	s_Geometries.MatCylinder.Reflect = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

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

	g_Renderer->CreateShaderResourceView(s_Resource.FloorTex, "floor.dds");
	g_Renderer->CreateShaderResourceView(s_Resource.FloorNormalTex, "floor_nmap.dds");
	g_Renderer->CreateShaderResourceView(s_Resource.BrickTex, "bricks.dds");
	g_Renderer->CreateShaderResourceView(s_Resource.BrickNormalTex, "bricks_nmap.dds");
}

void ApplicationMapping::SetupScene()
{
	InitGeometriesResource();

	s_Resource.Sky.Create("snowcube1024.dds", 5000.0f);

	s_Resource.Skull.CreateFromTxt("skull.txt");
	s_Resource.Skull.SetLightCount(3U);
	Matrix worldSkull = Matrix::Scaling(0.5f, 0.5f, 0.5f) * Matrix::Translation(0.0f, 1.0f, 0.0f);
	s_Resource.Skull.SetWorldMatrix(worldSkull);

	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	m_Camera->Translation(0.0f, 2.0f, -15.0f);

	D3D11_VIEWPORT viewport;
	viewport.Width = (float)m_Width;
	viewport.Height = (float)m_Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = viewport.TopLeftY = 0.0f;
	g_Renderer->SetViewports(&viewport);

	D3D11_SAMPLER_DESC sampDesc;
	memset(&sampDesc, 0, sizeof(D3D11_SAMPLER_DESC));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0.0f;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	g_Renderer->CreateSamplerState(s_Resource.Sampler, &sampDesc);

#ifdef UsingAntTweakBar
	GUIAntTweakBar::WidgeDesc enableDisplacementMap;
	enableDisplacementMap.Type = GUIAntTweakBar::eCheckBox;
	enableDisplacementMap.Title = "DisplacementMapping";
	enableDisplacementMap.GetVarFunc = GetEnableDisplacementMap;
	enableDisplacementMap.SetVarFunc = SetEnableDisplacementMap;
	m_GUI.AddWidget(enableDisplacementMap);
#endif
}

void ApplicationMapping::RenderScene()
{
	g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);

	g_Renderer->SetInputLayout(s_Resource.Layout);
	g_Renderer->SetVertexBuffer(s_Resource.GeometriesVBuf, sizeof(Math::Geometry::Vertex), 0U);
	g_Renderer->SetIndexBuffer(s_Resource.GeometriesIBuf, DXGI_FORMAT_R32_UINT);

	g_Renderer->SetConstantBuffer(s_Resource.ConstantsBufVS, 0U, D3DGraphic::eVertexShader);
	g_Renderer->SetConstantBuffer(s_Resource.ConstantsBufPS, 0U, D3DGraphic::ePixelShader);

	g_Renderer->SetSamplerStates(s_Resource.Sampler, 0U, D3DGraphic::ePixelShader);

	Matrix world;
	Matrix view = m_Camera->GetViewMatrix();
	Matrix proj = m_Camera->GetProjMatrix();
	Matrix vp = view * proj;
	Matrix wvp;

	ConstantsBufVS cbVS;
	memset(&cbVS, 0, sizeof(ConstantsBufVS));
	cbVS.VP = vp.Transpose();

	Vec4 eyePos = m_Camera->GetEyePos();
	s_CBufPS.EyePos = Vec3(eyePos.x, eyePos.y, eyePos.z);

	D3D11_PRIMITIVE_TOPOLOGY primitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	static Ref<ID3D11HullShader> s_NullHullShader;
	static Ref<ID3D11DomainShader> s_NullDomainShader;

	if (eNormalMap == m_MappingType)
	{
		primitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		g_Renderer->SetVertexShader(s_Resource.NormalMapVS);
		g_Renderer->SetHullShader(s_NullHullShader);
		g_Renderer->SetDomainShader(s_NullDomainShader);
		g_Renderer->SetPixelShader(s_Resource.NormalMapPS);
	}
	else if (eDisplacementMap == m_MappingType)
	{
		primitive = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
		g_Renderer->SetVertexShader(s_Resource.DisplacementMapVS);
		g_Renderer->SetHullShader(s_Resource.DisplacementMapHS);
		g_Renderer->SetDomainShader(s_Resource.DisplacementMapDS);
		g_Renderer->SetPixelShader(s_Resource.DisplacementMapPS);

		g_Renderer->SetConstantBuffer(s_Resource.ConstantsBufPS, 1U, D3DGraphic::eVertexShader);

		g_Renderer->SetSamplerStates(s_Resource.Sampler, 0U, D3DGraphic::eDomainShader);

		g_Renderer->SetConstantBuffer(s_Resource.ConstantsBufVS, 0U, D3DGraphic::eDomainShader);
		g_Renderer->SetConstantBuffer(s_Resource.ConstantsBufPS, 1U, D3DGraphic::eDomainShader);

		g_Renderer->SetShaderResource(s_Resource.BrickNormalTex, 0U, D3DGraphic::eDomainShader);
	}

	if (m_bWireframe)
	{
		g_Renderer->SetRasterizerState(s_Resource.Wireframe);
	}

	{
		/// Draw grid
		world = s_Geometries.WorldGrid;
		wvp = world * view * proj;
		cbVS.World = world.Transpose();
		cbVS.WorldInverse = cbVS.World.Inverse();
		cbVS.WVP = wvp.Transpose();
		cbVS.TexTransform = Matrix::Scaling(8.0f, 10.0f, 1.0f).Transpose();
		g_Renderer->UpdateBuffer(s_Resource.ConstantsBufVS, &cbVS, sizeof(ConstantsBufVS));

		memcpy(&s_CBufPS.Mat, &s_Geometries.MatGrid, sizeof(Lighting::Material));
		g_Renderer->UpdateBuffer(s_Resource.ConstantsBufPS, &s_CBufPS, sizeof(ConstantsBufPS));

		g_Renderer->SetShaderResource(s_Resource.FloorTex, 0U, D3DGraphic::ePixelShader);
		g_Renderer->SetShaderResource(s_Resource.FloorNormalTex, 1U, D3DGraphic::ePixelShader);

		g_Renderer->DrawIndexed(s_Geometries.GridIndexCount, s_Geometries.GridIndexOffset, s_Geometries.GridVertexOffset, primitive);
	}

	{
		/// Draw Box
		world = s_Geometries.WorldBox;
		wvp = world * view * proj;
		cbVS.World = world.Transpose();
		cbVS.WorldInverse = cbVS.World.Inverse();
		cbVS.WVP = wvp.Transpose();
		cbVS.TexTransform = Matrix::Scaling(2.0f, 1.0f, 1.0f).Transpose();
		g_Renderer->UpdateBuffer(s_Resource.ConstantsBufVS, &cbVS, sizeof(ConstantsBufVS));

		memcpy(&s_CBufPS.Mat, &s_Geometries.MatBox, sizeof(Lighting::Material));
		g_Renderer->UpdateBuffer(s_Resource.ConstantsBufPS, &s_CBufPS, sizeof(ConstantsBufPS));

		g_Renderer->SetShaderResource(s_Resource.BrickTex, 0U, D3DGraphic::ePixelShader);
		g_Renderer->SetShaderResource(s_Resource.BrickNormalTex, 1U, D3DGraphic::ePixelShader);

		g_Renderer->DrawIndexed(s_Geometries.BoxIndexCount, s_Geometries.BoxIndexOffset, s_Geometries.BoxVertexOffset, primitive);
	}

	{
		/// Draw Cylinders
		for (uint32_t i = 0U; i < 10U; ++i)
		{
			world = s_Geometries.WorldCylinder[i];
			wvp = world * view * proj;
			cbVS.World = world.Transpose();
			cbVS.WorldInverse = cbVS.World.Inverse();
			cbVS.WVP = wvp.Transpose();
			cbVS.TexTransform = Matrix::Scaling(1.0f, 2.0f, 1.0f).Transpose();
			g_Renderer->UpdateBuffer(s_Resource.ConstantsBufVS, &cbVS, sizeof(ConstantsBufVS));

			memcpy(&s_CBufPS.Mat, &s_Geometries.MatCylinder, sizeof(Lighting::Material));
			g_Renderer->UpdateBuffer(s_Resource.ConstantsBufPS, &s_CBufPS, sizeof(ConstantsBufPS));

			g_Renderer->SetShaderResource(s_Resource.BrickTex, 0U, D3DGraphic::ePixelShader);
			g_Renderer->SetShaderResource(s_Resource.BrickNormalTex, 1U, D3DGraphic::ePixelShader);

			g_Renderer->DrawIndexed(s_Geometries.CylinderIndexCount, s_Geometries.CylinderIndexOffset, s_Geometries.CylinderVertexOffset, primitive);
		}
	}

	if (eDisplacementMap == m_MappingType)
	{
		g_Renderer->SetVertexShader(s_Resource.NormalMapVS);
		g_Renderer->SetHullShader(s_NullHullShader);
		g_Renderer->SetDomainShader(s_NullDomainShader);
		g_Renderer->SetPixelShader(s_Resource.NormalMapPS);
	}

	{
		/// Draw Spheres
		for (uint32_t i = 0U; i < 10U; ++i)
		{
			world = s_Geometries.WorldSphere[i];
			wvp = world * view * proj;
			cbVS.World = world.Transpose();
			cbVS.WorldInverse = cbVS.World.Inverse();
			cbVS.WVP = wvp.Transpose();
			cbVS.TexTransform.Identity();
			g_Renderer->UpdateBuffer(s_Resource.ConstantsBufVS, &cbVS, sizeof(ConstantsBufVS));

			memcpy(&s_CBufPS.Mat, &s_Geometries.MatSphere, sizeof(Lighting::Material));
			g_Renderer->UpdateBuffer(s_Resource.ConstantsBufPS, &s_CBufPS, sizeof(ConstantsBufPS));

			g_Renderer->SetShaderResource(s_Resource.Sky.GetCubemap(), 2U, D3DGraphic::ePixelShader);

			g_Renderer->DrawIndexed(s_Geometries.SphereIndexCount, s_Geometries.SphereIndexOffset, s_Geometries.SphereVertexOffset);
		}
	}

	s_Resource.Skull.Draw(*m_Camera, m_bWireframe);

	s_Resource.Sky.Draw(*m_Camera);

	ImGui::Combo("MappingType", &m_MappingType, "NormalMapping\0DisplacementMapping");
	ImGui::Checkbox("Wireframe", &m_bWireframe);

	if (eDisplacementMap == m_MappingType)
	{
		ImGui::SliderFloat("HeightScale", &s_CBufPS.HeightScale, 0.01f, 0.2f);
		ImGui::SliderFloat("MinTessDistance", &s_CBufPS.MinTessDistance, 1.0f, 25.0f);
		ImGui::SliderFloat("MaxTessDistance", &s_CBufPS.MaxTessDistance, 1.0f, 25.0f);
		ImGui::SliderFloat("MinTessFactor", &s_CBufPS.MinTessFactor, 1.0f, 5.0f);
		ImGui::SliderFloat("MaxTessFactor", &s_CBufPS.MaxTessFactor, 1.0f, 5.0f);
	}
}