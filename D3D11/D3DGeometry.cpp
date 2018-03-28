#include "D3DGeometry.h"
#include "Camera.h"

extern D3DGraphic* g_Renderer;

NamespaceBegin(Geometry)

void Light::Init()
{
	if (m_Inited)
	{
		return;
	}

	m_Inited = true;
}

void Light::Draw()
{
	Init();
}

void Material::Init()
{
	D3D11_SAMPLER_DESC sampDesc;
	memset(&sampDesc, 0, sizeof(D3D11_SAMPLER_DESC));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0.0f;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	g_Renderer->CreateSamplerState(Sampler, &sampDesc);
}

void Material::SetTexture(eTextureType type, const Ref<ID3D11ShaderResourceView> &srcTex)
{
	assert((type < eTexTypeCount) && !Textures[type].Valid() && srcTex.Valid());

	Textures[type] = srcTex;

	Params.EnableTexture[type] = 1U;

	if (!Sampler.Valid())
	{
		Init();
	}
}

void Mesh::SetLight(const Light *pLight, bool bReset)
{
	assert(pLight->Params.Type != Light::eLightTypeCount);

	if (!m_LightEnabled || bReset)
	{
		memcpy(&m_Light, &m_Light, sizeof(Light));
		m_LightEnabled = true;
		m_CBufferPS.EnableLight = 1U;
	}
}

void Mesh::SetMaterial(const Material *pMaterial, bool bReset)
{
	if (!m_MaterialEnabled || bReset)
	{
		m_Material = *pMaterial;
		m_MaterialEnabled = true;
		m_CBufferPS.EnableMaterial = 1U;
	}
}

void Mesh::Init()
{
	if (m_Inited)
	{
		return;
	}

	assert(g_Renderer && m_Created);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	g_Renderer->CreateVertexShaderAndInputLayout(m_Resource.VertexShader, m_Resource.VertexLayout, layout,
		ARRAYSIZE(layout), "Mesh.hlsl", "VSMain");
	g_Renderer->CreatePixelShader(m_Resource.PixelShader, "Mesh.hlsl", "PSMain");

	g_Renderer->CreateConstantBuffer(m_Resource.CBufferVS, sizeof(ConstantsBufferVS), D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);
	g_Renderer->CreateConstantBuffer(m_Resource.CBufferPS, sizeof(ConstantsBufferPS), D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

	g_Renderer->CreateVertexBuffer(m_Resource.VertexBuffer, (uint32_t)(sizeof(Vertex) * m_Vertices.size()), D3D11_USAGE_IMMUTABLE, &m_Vertices[0]);
	g_Renderer->CreateIndexBuffer(m_Resource.IndexBuffer, (uint32_t)(sizeof(uint32_t) * m_Indices.size()), D3D11_USAGE_IMMUTABLE, &m_Indices[0]);

	g_Renderer->CreateRasterizerState(m_Resource.Wireframe, D3D11_FILL_WIREFRAME);

	m_Inited = true;
}

void Mesh::ApplyMaterial()
{
	if (!m_MaterialEnabled)
	{
		return;
	}

	memcpy(&m_CBufferPS.MaterialParams, &m_Material.Params, sizeof(Material::ShaderParams));

	for (uint32_t i = 0U; i < Material::eTexTypeCount; ++i)
	{
		if (1U == m_Material.Params.EnableTexture[i])
		{
			g_Renderer->SetShaderResource(m_Material.Textures[i], i, D3DGraphic::ePixelShader);
		}
	}

	g_Renderer->SetSamplerStates(m_Material.Sampler, 0U, D3DGraphic::ePixelShader);
}

void Mesh::ApplyLight()
{
	if (!m_LightEnabled)
	{
		return;
	}

	memcpy(&m_CBufferPS.LightParams, &m_Light.Params, sizeof(Light::ShaderParams));
}

void Mesh::Draw(const Camera &cam)
{
	Init();

	ApplyMaterial();

	ApplyLight();

	g_Renderer->SetInputLayout(m_Resource.VertexLayout);

	g_Renderer->SetVertexBuffer(m_Resource.VertexBuffer, sizeof(Vertex), 0U, 0U);
	g_Renderer->SetIndexBuffer(m_Resource.IndexBuffer, DXGI_FORMAT_R32_UINT, 0U);

	Matrix world = cam.GetWorldMatrix();
	world = world * m_Transform.Scale * m_Transform.Rotate * m_Transform.Translation;
	Matrix wvp = world * cam.GetViewMatrix() * cam.GetProjMatrix();

	m_CBufferVS.World = world.Transpose();
	m_CBufferVS.WorldInverse = world.InverseTranspose();
	m_CBufferVS.WorldViewProj = wvp.Transpose();
	m_CBufferVS.EyePos = cam.GetEyePos();
	g_Renderer->UpdateBuffer(m_Resource.CBufferVS, &m_CBufferVS, sizeof(ConstantsBufferVS));
	g_Renderer->SetConstantBuffer(m_Resource.CBufferVS, 0U, D3DGraphic::eVertexShader);

	g_Renderer->UpdateBuffer(m_Resource.CBufferPS, &m_CBufferPS, sizeof(ConstantsBufferPS));
	g_Renderer->SetConstantBuffer(m_Resource.CBufferPS, 0U, D3DGraphic::ePixelShader);

	g_Renderer->SetVertexShader(m_Resource.VertexShader);
	g_Renderer->SetPixelShader(m_Resource.PixelShader);

	if (m_Wireframe)
	{
		g_Renderer->SetRasterizerState(m_Resource.Wireframe);
	}

	g_Renderer->DrawIndexed((uint32_t)m_Indices.size(), 0U, 0);
}

void Mesh::DrawNormal(const Camera &/*cam*/)
{

}

void Mesh::SubDivide(Mesh& mesh)
{
	/// Save a copy of the input geometry.
	Mesh inputCopy = mesh;

	mesh.m_Vertices.resize(0);
	mesh.m_Indices.resize(0);

	///       v1
	///       *
	///      / \
	///     /   \
	///  m0*-----*m1
	///   / \   / \
	///  /   \ /   \
	/// *-----*-----*
	/// v0    m2    v2

	uint32_t numTris = (uint32_t)inputCopy.m_Indices.size() / 3;
	for (uint32_t i = 0; i < numTris; ++i)
	{
		Vertex v0 = inputCopy.m_Vertices[inputCopy.m_Indices[i * 3 + 0]];
		Vertex v1 = inputCopy.m_Vertices[inputCopy.m_Indices[i * 3 + 1]];
		Vertex v2 = inputCopy.m_Vertices[inputCopy.m_Indices[i * 3 + 2]];

		///
		/// Generate the midpoints.
		///

		Vertex m0, m1, m2;

		/// For subdivision, we just care about the position component.  We derive the other
		/// vertex components in CreateGeosphere.

		m0.Position = Vec3(0.5f * (v0.Position.x + v1.Position.x), 0.5f * (v0.Position.y + v1.Position.y), 0.5f * (v0.Position.z + v1.Position.z));
		m1.Position = Vec3(0.5f * (v1.Position.x + v2.Position.x), 0.5f * (v1.Position.y + v2.Position.y), 0.5f * (v1.Position.z + v2.Position.z));
		m2.Position = Vec3(0.5f * (v0.Position.x + v2.Position.x), 0.5f * (v0.Position.y + v2.Position.y), 0.5f * (v0.Position.z + v2.Position.z));

		///
		/// Add new geometry.
		///

		mesh.m_Vertices.push_back(v0); /// 0
		mesh.m_Vertices.push_back(v1); /// 1
		mesh.m_Vertices.push_back(v2); /// 2
		mesh.m_Vertices.push_back(m0); /// 3
		mesh.m_Vertices.push_back(m1); /// 4
		mesh.m_Vertices.push_back(m2); /// 5

		mesh.m_Indices.push_back(i * 6 + 0);
		mesh.m_Indices.push_back(i * 6 + 3);
		mesh.m_Indices.push_back(i * 6 + 5);
			 
		mesh.m_Indices.push_back(i * 6 + 3);
		mesh.m_Indices.push_back(i * 6 + 4);
		mesh.m_Indices.push_back(i * 6 + 5);
			 
		mesh.m_Indices.push_back(i * 6 + 5);
		mesh.m_Indices.push_back(i * 6 + 4);
		mesh.m_Indices.push_back(i * 6 + 2);
			 
		mesh.m_Indices.push_back(i * 6 + 3);
		mesh.m_Indices.push_back(i * 6 + 1);
		mesh.m_Indices.push_back(i * 6 + 4);
	}
}

void Mesh::MakeCylinderTopBottomCap(bool bTop, float bottomRadius, float topRadius, float height, uint32_t slice, Mesh &mesh)
{
	float y = bTop ? (0.5f * height) : (-0.5f * height);
	float r = bTop ? topRadius : bottomRadius;
	float cy = bTop ? 1.0f : -1.0f;
	float theta = DirectX::XM_2PI / slice;
	uint32_t baseIndex = (uint32_t)mesh.m_Vertices.size();

	/// Duplicate cap ring vertices because the texture coordinates and normals differ.
	for (uint32_t i = 0U; i <= slice; ++i)
	{
		float x = r * cosf(i * theta);
		float z = r * sinf(i * theta);

		/// Scale down by the height to try and make top cap texture coord area proportional to base.
		Vertex v(
			x, y, z,
			0.0f, cy, 0.0f,
			1.0f, 0.0f, 0.0f,
			x / height + 0.5f, z / height + 0.5f);
		mesh.m_Vertices.push_back(v);
	}

	/// Cap center vertex.
	Vertex center(0.0f, y, 0.0f, 0.0f, cy, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f);
	mesh.m_Vertices.push_back(center);

	/// Cache the index of center vertex.
	uint32_t centerIndex = (uint32_t)(mesh.m_Vertices.size() - 1U);

	for (uint32_t i = 0U; i < slice; ++i)
	{
		mesh.m_Indices.push_back(centerIndex);

		uint32_t i1 = bTop ? (baseIndex + i + 1) : (baseIndex + i);
		uint32_t i2 = bTop ? (baseIndex + i) : (baseIndex + i + 1);

		mesh.m_Indices.push_back(i1);
		mesh.m_Indices.push_back(i2);
	}
}

void Mesh::MakeBox(float width, float height, float depth, Mesh &mesh)
{
	Vertex vertices[24];

	float w = width  * 0.5f;
	float h = height * 0.5f;
	float d = depth  * 0.5f;

	vertices[0] = Vertex(-w, -h, -d, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[1] = Vertex(-w, h, -d, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertices[2] = Vertex(w, h, -d, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	vertices[3] = Vertex(w, -h, -d, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	vertices[4] = Vertex(-w, -h, d, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	vertices[5] = Vertex(w, -h, d, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[6] = Vertex(w, h, d, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertices[7] = Vertex(-w, h, d, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	vertices[8] = Vertex(-w, h, -d, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[9] = Vertex(-w, h, d, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertices[10] = Vertex(w, h, d, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	vertices[11] = Vertex(w, h, -d, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	vertices[12] = Vertex(-w, -h, -d, -1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	vertices[13] = Vertex(w, -h, -d, -1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[14] = Vertex(w, -h, d, -1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertices[15] = Vertex(-w, -h, d, -1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	vertices[16] = Vertex(-w, -h, d, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	vertices[17] = Vertex(-w, h, d, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	vertices[18] = Vertex(-w, h, -d, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	vertices[19] = Vertex(-w, -h, -d, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	vertices[20] = Vertex(w, -h, -d, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vertices[21] = Vertex(w, h, -d, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	vertices[22] = Vertex(w, h, d, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vertices[23] = Vertex(w, -h, d, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	mesh.m_Vertices.assign(&vertices[0], &vertices[24]);

	uint32_t indices[36] = { 0 };

	/// front
	indices[0] = 0;  indices[1] = 1;  indices[2] = 2;  indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

	/// back
	indices[6] = 4;  indices[7] = 5;  indices[8] = 6;  indices[9] = 4;  indices[10] = 6;  indices[11] = 7;

	/// top
	indices[12] = 8;  indices[13] = 9;  indices[14] = 10; indices[15] = 8;  indices[16] = 10; indices[17] = 11;

	/// bottom
	indices[18] = 12; indices[19] = 13; indices[20] = 14; indices[21] = 12; indices[22] = 14; indices[23] = 15;

	/// left
	indices[24] = 16; indices[25] = 17; indices[26] = 18; indices[27] = 16; indices[28] = 18; indices[29] = 19;

	/// right
	indices[30] = 20; indices[31] = 21; indices[32] = 22; indices[33] = 20; indices[34] = 22; indices[35] = 23;

	mesh.m_Indices.assign(&indices[0], &indices[36]);

	mesh.m_Created = true;
}

void Mesh::MakeCube(float width, Mesh &mesh)
{
	MakeBox(width, width, width, mesh);
}

void Mesh::MakeSphere(float radius, uint32_t slice, uint32_t stack, Mesh &mesh)
{
	mesh.m_Vertices.clear();
	mesh.m_Indices.clear();

	Vertex tVertex(0.0f, radius, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	Vertex bVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	float phiStep = DirectX::XM_PI / stack;
	float thetaStep = 2.0f * DirectX::XM_PI / slice;

	/// Compute vertices for each stack ring (do not count the poles as rings).
	mesh.m_Vertices.push_back(tVertex);
	for (uint32_t i = 1; i < stack; ++i)
	{
		float phi = i * phiStep;

		// Vertices of ring.
		for (uint32_t j = 0; j <= slice; ++j)
		{
			float theta = j * thetaStep;
			Vertex vec;

			vec.Position.x = radius * sinf(phi) * cosf(theta);
			vec.Position.y = radius * cosf(phi);
			vec.Position.z = radius * sinf(phi) * sinf(theta);

			vec.Tangent.x = -radius * sinf(phi) * sinf(theta);
			vec.Tangent.y = 0.0f;
			vec.Tangent.z = radius * sinf(phi) * cosf(theta);

			DirectX::XMVECTOR tangent = DirectX::XMLoadFloat3(&vec.Tangent);
			DirectX::XMStoreFloat3(&vec.Tangent, DirectX::XMVector3Normalize(tangent));

			DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&vec.Position);
			DirectX::XMStoreFloat3(&vec.Normal, DirectX::XMVector3Normalize(pos));

			vec.UV.x = theta / DirectX::XM_2PI;
			vec.UV.y = phi / DirectX::XM_PI;

			mesh.m_Vertices.push_back(vec);
		}
	}
	mesh.m_Vertices.push_back(bVertex);

	///
	/// Compute indices for top stack.  The top stack was written first to the vertex buffer
	/// and connects the top pole to the first ring.
	///
	for (uint32_t i = 1; i <= slice; ++i)
	{
		mesh.m_Indices.push_back(0);
		mesh.m_Indices.push_back(i + 1);
		mesh.m_Indices.push_back(i);
	}

	///
	/// Compute indices for inner stacks (not connected to poles).
	///
	/// Offset the indices to the index of the first vertex in the first ring.
	/// This is just skipping the top pole vertex.
	uint32_t baseIndex = 1;
	uint32_t ringVertexCount = slice + 1;
	for (uint32_t i = 0; i < stack - 2; ++i)
	{
		for (uint32_t j = 0; j < slice; ++j)
		{
			mesh.m_Indices.push_back(baseIndex + i * ringVertexCount + j);
			mesh.m_Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			mesh.m_Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
				 
			mesh.m_Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			mesh.m_Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			mesh.m_Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	///
	/// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	/// and connects the bottom pole to the bottom ring.
	///
	/// South pole vertex was added last.
	uint32_t southPoleIndex = (uint32_t)mesh.m_Vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (uint32_t i = 0; i < slice; ++i)
	{
		mesh.m_Indices.push_back(southPoleIndex);
		mesh.m_Indices.push_back(baseIndex + i);
		mesh.m_Indices.push_back(baseIndex + i + 1);
	}
}

void Mesh::MakeGeoSphere(float radius, uint32_t subDivisions, Mesh &mesh)
{
	/// Put a cap on the number of subdivisions.
	subDivisions = min(subDivisions, 5u);

	/// Approximate a sphere by tessellating an icosahedron.

	const float X = 0.525731f;
	const float Z = 0.850651f;

	Vec3 pos[12] =
	{
		Vec3(-X, 0.0f, Z),  Vec3(X, 0.0f, Z),
		Vec3(-X, 0.0f, -Z), Vec3(X, 0.0f, -Z),
		Vec3(0.0f, Z, X),   Vec3(0.0f, Z, -X),
		Vec3(0.0f, -Z, X),  Vec3(0.0f, -Z, -X),
		Vec3(Z, X, 0.0f),   Vec3(-Z, X, 0.0f),
		Vec3(Z, -X, 0.0f),  Vec3(-Z, -X, 0.0f)
	};

	uint32_t k[60] =
	{
		1, 4,  0,   4, 9, 0,  4,  5,  9,  8, 5,  4,   1, 8, 4,
		1, 10, 8,  10, 3, 8,  8,  3,  5,  3, 2,  5,   3, 7, 2,
		3, 10, 7,  10, 6, 7,  6, 11,  7,  6, 0, 11,   6, 1, 0,
		10, 1, 6,  11, 0, 9,  2, 11,  9,  5, 2,  9,  11, 2, 7
	};

	mesh.m_Vertices.resize(12);
	mesh.m_Indices.resize(60);

	for (uint32_t i = 0; i < 12; ++i)
	{
		mesh.m_Vertices[i].Position = pos[i];
	}

	for (uint32_t i = 0; i < 60; ++i)
	{
		mesh.m_Indices[i] = k[i];
	}

	for (uint32_t i = 0; i < subDivisions; ++i)
	{
		SubDivide(mesh);
	}

	/// Project vertices onto sphere and scale.
	for (uint32_t i = 0; i < mesh.m_Vertices.size(); ++i)
	{
		/// Project onto unit sphere.
		DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&mesh.m_Vertices[i].Position));

		/// Project onto sphere.
		DirectX::XMVECTOR p = DirectX::XMVectorScale(n, radius);

		DirectX::XMStoreFloat3(&mesh.m_Vertices[i].Position, p);
		DirectX::XMStoreFloat3(&mesh.m_Vertices[i].Normal, n);

		/// Derive texture coordinates from spherical coordinates.
		float theta = Math::AngleFromXY(mesh.m_Vertices[i].Position.x, mesh.m_Vertices[i].Position.z);

		float phi = acosf(mesh.m_Vertices[i].Position.y / radius);

		mesh.m_Vertices[i].UV.x = theta / DirectX::XM_2PI;
		mesh.m_Vertices[i].UV.y = phi / DirectX::XM_PI;

		/// Partial derivative of P with respect to theta
		mesh.m_Vertices[i].Tangent.x = -radius * sinf(phi) * sinf(theta);
		mesh.m_Vertices[i].Tangent.y = 0.0f;
		mesh.m_Vertices[i].Tangent.z = radius * sinf(phi) * cosf(theta);

		DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&mesh.m_Vertices[i].Tangent);
		DirectX::XMStoreFloat3(&mesh.m_Vertices[i].Tangent, DirectX::XMVector3Normalize(T));
	}
}

void Mesh::MakeFlatGeoSphere(float radius, uint32_t subDivisions, Mesh &mesh)
{
	MakeGeoSphere(radius, subDivisions, mesh);

	uint32_t numTris = (uint32_t)mesh.m_Indices.size() / 3;
	for (uint32_t i = 0; i < numTris; ++i)
	{
		Vertex &v0 = mesh.m_Vertices[mesh.m_Indices[i * 3 + 0]];
		Vertex &v1 = mesh.m_Vertices[mesh.m_Indices[i * 3 + 1]];
		Vertex &v2 = mesh.m_Vertices[mesh.m_Indices[i * 3 + 2]];

		Vec3 v01 = v1.Position - v0.Position;
		Vec3 v12 = v2.Position - v1.Position;

		DirectX::XMVECTOR dv01 = DirectX::XMLoadFloat3(&v01);
		DirectX::XMVECTOR dv12 = DirectX::XMLoadFloat3(&v12);
		DirectX::XMVECTOR normal = DirectX::XMVector3Cross(dv01, dv12);
		normal = DirectX::XMVector3Normalize(normal);

		DirectX::XMStoreFloat3(&v0.Normal, normal);
		DirectX::XMStoreFloat3(&v1.Normal, normal);
		DirectX::XMStoreFloat3(&v2.Normal, normal);
	}
}

void Mesh::MakeCylinder(float bottomRadius, float topRadius, float height, uint32_t slice, uint32_t stack, Mesh &mesh)
{
	mesh.m_Vertices.clear();
	mesh.m_Indices.clear();

	float stackHeight = height / stack;

	/// Amount to increment radius as we move up each stack level from bottom to top.
	float radiusStep = (topRadius - bottomRadius) / stack;

	uint32_t ring = stack + 1;

	/// Compute vertices for each stack ring starting at the bottom and moving up.
	for (uint32_t i = 0U; i < ring; ++i)
	{
		float y = -0.5f * height + i * stackHeight;
		float r = bottomRadius + i * radiusStep;

		/// Vertices of ring
		float theta = 2.0f * DirectX::XM_PI / slice;
		for (uint32_t j = 0U; j <= slice; ++j)
		{
			float c = cosf(j * theta);
			float s = sinf(j * theta);

			Vertex v;
			v.Position = Vec3(r * c, y, r * s);

			v.UV.x = (float)j / slice;
			v.UV.y = 1.0f - (float)i / stack;

			/// Cylinder can be parameterized as follows, where we introduce v
			/// parameter that goes in the same direction as the v tex-coord
			/// so that the bitangent goes in the same direction as the v tex-coord.
			/// Let r0 be the bottom radius and let r1 be the top radius.
			/// y(v) = h - hv for v in [0,1].
			/// r(v) = r1 + (r0-r1)v
			/// 
			/// x(t, v) = r(v)*cos(t)
			/// y(t, v) = h - hv
			/// z(t, v) = r(v)*sin(t)
			/// 
			/// dx/dt = -r(v)*sin(t)
			/// dy/dt = 0
			/// dz/dt = +r(v)*cos(t)
			/// 
			/// dx/dv = (r0-r1)*cos(t)
			/// dy/dv = -h
			/// dz/dv = (r0-r1)*sin(t)

			/// Unit length
			v.Tangent = Vec3(-s, 0.0f, c);

			float dR = bottomRadius - topRadius;
			Vec3 binNormal(dR * c, -height, dR * s);

			DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&v.Tangent);
			DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&binNormal);
			DirectX::XMVECTOR N = DirectX::XMVector3Cross(T, B);
			DirectX::XMStoreFloat3(&v.Normal, N);

			mesh.m_Vertices.push_back(v);
		}
	}

	/// Add one because we duplicate the first and last vertex per ring
	/// since the texture coordinates are different.
	uint32_t ringVertexCount = slice + 1;

	/// Compute indices for each stack.
	for (uint32_t i = 0U; i < stack; ++i)
	{
		for (uint32_t j = 0U; j < slice; ++j)
		{
			mesh.m_Indices.push_back(i * ringVertexCount + j);
			mesh.m_Indices.push_back((i + 1) * ringVertexCount + j);
			mesh.m_Indices.push_back((i + 1) * ringVertexCount + j + 1);
				 
			mesh.m_Indices.push_back(i * ringVertexCount + j);
			mesh.m_Indices.push_back((i + 1) * ringVertexCount + j + 1);
			mesh.m_Indices.push_back(i * ringVertexCount + j + 1);
		}
	}

	MakeCylinderTopBottomCap(true, bottomRadius, topRadius, height, slice, mesh);
	MakeCylinderTopBottomCap(false, bottomRadius, topRadius, height, slice, mesh);
}

void Mesh::MakeGrid(float width, float depth, uint32_t m, uint32_t n, Mesh &mesh)
{
	uint32_t vertexCount = m * n;
	uint32_t faceCount = (m - 1) * (n - 1) * 2;

	/// Create the vertices
	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	mesh.m_Vertices.resize(vertexCount);
	for (uint32_t i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;
		for (uint32_t j = 0; j < n; ++j)
		{
			float x = -halfWidth + j * dx;

			mesh.m_Vertices[i * n + j].Position = Vec3(x, 0.0f, z);
			mesh.m_Vertices[i * n + j].Normal = Vec3(0.0f, 1.0f, 0.0f);
			mesh.m_Vertices[i * n + j].Tangent = Vec3(1.0f, 0.0f, 0.0f);
				 
			mesh.m_Vertices[i * n + j].UV.x = j * du;
			mesh.m_Vertices[i * n + j].UV.y = i * dv;
		}		 
	}

	/// Create the indices
	mesh.m_Indices.resize(faceCount * 3);

	uint32_t k = 0;
	for (uint32_t i = 0; i < m - 1; ++i)
	{
		for (uint32_t j = 0; j < n - 1; ++j)
		{
			mesh.m_Indices[k] = i * n + j;
			mesh.m_Indices[k + 1] = i * n + j + 1;
			mesh.m_Indices[k + 2] = (i + 1) * n + j;
				 
			mesh.m_Indices[k + 3] = (i + 1) * n + j;
			mesh.m_Indices[k + 4] = i * n + j + 1;
			mesh.m_Indices[k + 5] = (i + 1) * n + j + 1;

			k += 6;
		}
	}
}

void Mesh::MakeQuad(float length, Mesh &mesh)
{
	mesh.m_Vertices.resize(4U);
	mesh.m_Indices.resize(6U);

	float halfLen = length / 2.0f;

	/// Position coordinates specified in NDC space.
	mesh.m_Vertices[0] = Vertex(
		-halfLen, -halfLen, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f);

	mesh.m_Vertices[1] = Vertex(
		-halfLen, +halfLen, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f);

	mesh.m_Vertices[2] = Vertex(
		+halfLen, +halfLen, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f);

	mesh.m_Vertices[3] = Vertex(
		+halfLen, -halfLen, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f);

	mesh.m_Indices[0] = 0;
	mesh.m_Indices[1] = 1;
	mesh.m_Indices[2] = 2;
		 
	mesh.m_Indices[3] = 0;
	mesh.m_Indices[4] = 2;
	mesh.m_Indices[5] = 3;
}

NamespaceEnd(Geometry)