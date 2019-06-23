#include "Model.h"
#include "Base/AssetFile.h"
#include "Tool/AssetTool/AssetTool.h"

NamespaceBegin(Geometry)

void Model::AppendSubModel(const SubModel &subModel, bool bUseDefaultLayout)
{
	if (bUseDefaultLayout && !m_InputLayouts[0].IsValid())
	{
		std::vector<VertexLayout> vertexLayout =
		{
			{ "POSITION", sizeof(Vertex::Position), offsetof(Vertex, Position), eRGB32_Float  },
			{ "NORMAL",   sizeof(Vertex::Normal),   offsetof(Vertex, Normal),   eRGB32_Float  },
			{ "TANGENT",  sizeof(Vertex::Tangent),  offsetof(Vertex, Tangent),  eRGB32_Float  },
			{ "TEXCOORD", sizeof(Vertex::UV),       offsetof(Vertex, UV),       eRG32_Float   },
		};

		RVertexShader vertexShader;
		vertexShader.Create("Mesh.shader", "VSMain");
		m_InputLayouts[0].Create(vertexShader.GetBlob(), vertexLayout);
	}

	m_SubModels.emplace_back(subModel);
}

void Model::CreateAsCube(float width, float height, float depth)
{
	assert(!m_Valid);

	std::vector<Vertex> vertices(24);

	float w = width * 0.5f;
	float h = height * 0.5f;
	float d = depth * 0.5f;

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

	std::vector<uint32_t> indices =
	{
		/// front
		0, 1, 2, 0, 2, 3,
		/// back
		4, 5, 6, 4, 6, 7,
		/// top
		8, 9, 10, 8, 10, 11,
		/// bottom
		12, 13, 14, 12, 14, 15,
		/// left
		16, 17, 18, 16, 18, 19,
		/// right
		20, 21, 22, 20, 22, 23
	};

	m_BoundingBox = Box(vertices[0].Position, vertices[22].Position);
	m_HasBoundingBox = true;

	AddBuffer(vertices);
	AddBuffer(indices);
	AppendSubModel(SubModel{ 0U, 0U, 0U, (uint32_t)indices.size(), 0U, 0 });

	m_Valid = true;
}

void Model::CreateAsSphere(float radius, uint32_t slice, uint32_t stack)
{
	assert(!m_Valid);

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	Vertex tVertex(0.0f, radius, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	Vertex bVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	float phiStep = DirectX::XM_PI / stack;
	float thetaStep = 2.0f * DirectX::XM_PI / slice;

	/// Compute vertices for each stack ring (do not count the poles as rings).
	vertices.emplace_back(tVertex);
	for (uint32_t i = 1; i < stack; ++i)
	{
		float phi = i * phiStep;

		/// Vertices of ring.
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

			vertices.emplace_back(vec);
		}
	}
	vertices.emplace_back(bVertex);

	///
	/// Compute indices for top stack.  The top stack was written first to the vertex buffer
	/// and connects the top pole to the first ring.
	///
	for (uint32_t i = 1; i <= slice; ++i)
	{
		indices.emplace_back(0);
		indices.emplace_back(i + 1);
		indices.emplace_back(i);
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
			indices.emplace_back(baseIndex + i * ringVertexCount + j);
			indices.emplace_back(baseIndex + i * ringVertexCount + j + 1);
			indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j);

			indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j);
			indices.emplace_back(baseIndex + i * ringVertexCount + j + 1);
			indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	///
	/// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	/// and connects the bottom pole to the bottom ring.
	///
	/// South pole vertex was added last.
	uint32_t southPoleIndex = (uint32_t)vertices.size() - 1;

	/// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (uint32_t i = 0; i < slice; ++i)
	{
		indices.emplace_back(southPoleIndex);
		indices.emplace_back(baseIndex + i);
		indices.emplace_back(baseIndex + i + 1);
	}

	AddBuffer(vertices);
	AddBuffer(indices);
	AppendSubModel(SubModel{ 0U, 0U, 0U, (uint32_t)indices.size(), 0U, 0 });

	m_Valid = true;
}

void Model::CreateAsGrid(float width, float depth, uint32_t m, uint32_t n)
{
	assert(!m_Valid);

	uint32_t vertexCount = m * n;
	uint32_t faceCount = (m - 1) * (n - 1) * 2;

	/// Create the vertices
	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	vertices.resize(vertexCount);
	for (uint32_t i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;
		for (uint32_t j = 0; j < n; ++j)
		{
			float x = -halfWidth + j * dx;

			vertices[i * n + j].Position = Vec3(x, 0.0f, z);
			vertices[i * n + j].Normal = Vec3(0.0f, 1.0f, 0.0f);
			vertices[i * n + j].Tangent = Vec3(1.0f, 0.0f, 0.0f);

			vertices[i * n + j].UV.x = j * du;
			vertices[i * n + j].UV.y = i * dv;
		}
	}

	/// Create the indices
	indices.resize(faceCount * 3);

	uint32_t k = 0;
	for (uint32_t i = 0; i < m - 1; ++i)
	{
		for (uint32_t j = 0; j < n - 1; ++j)
		{
			indices[k] = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1) * n + j;

			indices[k + 3] = (i + 1) * n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1) * n + j + 1;

			k += 6;
		}
	}

	AddBuffer(vertices);
	AddBuffer(indices);
	AppendSubModel(SubModel{ 0U, 0U, 0U, (uint32_t)indices.size(), 0U, 0 });

	m_Valid = true;
}

void Model::CreateAsQuad(float left, float top, float width, float height)
{
	assert(!m_Valid);

	std::vector<Vertex> vertices(4U);
	std::vector<uint32_t> indices(6U);

	/// Position coordinates specified in NDC space.
	vertices[0] = Vertex(
		left, top - height, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f);

	vertices[1] = Vertex(
		left, top, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f);

	vertices[2] = Vertex(
		left + width, top, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f);

	vertices[3] = Vertex(
		left + width, top - height, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	AddBuffer(vertices);
	AddBuffer(indices);
	AppendSubModel(SubModel{ 0U, 0U, (uint32_t)indices.size(), 0U, 0 });

	m_Valid = true;
}

void Model::CreateFromFile(const std::string &fileName)
{
	assert(!m_Valid);

	AssetFile modelFile(fileName);

	switch (modelFile.GetType())
	{
	case AssetFile::eTxtMesh:
		assert(0);
		break;
	case AssetFile::eObjMesh:
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		Verify(AssetTool::LoadOBJ(modelFile, vertices, indices, m_BoundingBox));

		AddBuffer(vertices);
		AddBuffer(indices);
		AppendSubModel(SubModel{ 0U, 0U, (uint32_t)indices.size(), 0U, 0 });

		m_HasBoundingBox = true;
	}
	break;
	case AssetFile::eSDKMesh:
		Verify(AssetTool::LoadSDKMesh(modelFile, *this));
		break;
	default:
		assert(0);
		break;
	}

	m_Valid = true;
}

Box::Box(const Vec3 &min, const Vec3 &max)
{
	m_Center = (min + max) * 0.5f;
	m_Size = max - min;

	m_Vertices.resize(8U);
	m_Indices.resize(36U);

	m_Vertices[0].Position = min;
	m_Vertices[1].Position = Vec3(min.x, min.y, max.z);
	m_Vertices[2].Position = Vec3(max.x, min.y, max.z);
	m_Vertices[3].Position = Vec3(max.x, min.y, min.z);

	m_Vertices[4].Position = Vec3(min.x, max.y, min.z);
	m_Vertices[5].Position = Vec3(min.x, max.y, max.z);
	m_Vertices[6].Position = max;
	m_Vertices[7].Position = Vec3(max.x, max.y, min.z);

	m_Indices =
	{
		0, 4, 7, 0, 7, 3, 	/// front
		1, 5, 6, 1, 6, 2,  	/// back
		4, 5, 6, 4, 6, 7,	/// top
		0, 1, 2, 0, 2, 3,	/// bottom
		0, 4, 5, 0, 5, 1,	/// left
		3, 7, 6, 3, 6, 2	/// right
	};
}

Sphere::Sphere(const Vec3 &center, float radius)
	: m_Center(center)
	, m_Radius(radius)
{

}

Box ComputeBoundingBox(const std::vector<Vertex> &vertices)
{
	DirectX::XMVECTOR vecMin = {}, vecMax = {};

	for (auto it = vertices.cbegin(); it != vertices.cend(); ++it)
	{
		auto vec = DirectX::XMLoadFloat3(&it->Position);
		vecMin = DirectX::XMVectorMin(vecMin, vec);
		vecMax = DirectX::XMVectorMax(vecMax, vec);
	}

	DirectX::XMFLOAT3 min = {};
	DirectX::XMStoreFloat3(&min, vecMin);

	DirectX::XMFLOAT3 max = {};
	DirectX::XMStoreFloat3(&min, vecMax);

	return Box(Vec3(min.x, min.y, min.z), Vec3(max.x, max.y, max.z));
}

Sphere ComputeBoundingBoundingSphere(const std::vector<Vertex> &vertices)
{
	return Sphere();
}

void Model::Draw(const DXUTCamera &camera, bool bDrawBoundingBox)
{
	for (auto it = m_SubModels.cbegin(); it != m_SubModels.cend(); ++it)
	{
		const SubModel &subModel = *it;
		REngine::Instance().SetInputLayout(m_InputLayouts[subModel.InputLayout]);
		REngine::Instance().SetVertexBuffer(m_VertexBuffers[subModel.VertexBuffer], m_InputLayouts[subModel.InputLayout].GetVertexStride(), 0U, 0U);
		REngine::Instance().SetIndexBuffer(m_IndexBuffers[subModel.IndexBuffer], subModel.IndexFormat, 0U);

		if (subModel.MaterialIndex != UINT32_MAX)
		{
			REngine::Instance().SetSamplerState(RStaticState::LinearSampler, 0U, ePixelShader);

			const Material &mat = m_Materials[subModel.MaterialIndex];
			if (mat.Textures[Material::eDiffuse].IsValid())
			{
				REngine::Instance().SetShaderResourceView(mat.Textures[Material::eDiffuse], 0U, ePixelShader);
			}
		}

		REngine::Instance().DrawIndexed(subModel.IndexCount, subModel.StartIndex, subModel.VertexOffset, subModel.PrimitiveType);
	}

	if (bDrawBoundingBox && m_HasBoundingBox)
	{
		DrawBoundingBox(camera);
	}
}

void Model::DrawBoundingBox(const DXUTCamera &camera)
{
	static RBuffer s_VB;
	static RBuffer s_IB;
	static RBuffer s_CB;
	static RVertexShader s_VS;
	static RPixelShader s_PS;
	static uint32_t s_IndexCount = 0U;

	if (!s_VB.IsValid())
	{
		const auto &vertices = m_BoundingBox.GetVertices();
		const auto &indices = m_BoundingBox.GetIndices();
		s_IndexCount = indices.size();
		s_VB.CreateAsVertexBuffer(sizeof(Vertex) * vertices.size(), eGpuReadOnly, vertices.data());
		s_IB.CreateAsIndexBuffer(sizeof(uint32_t) * indices.size(), eGpuReadOnly, indices.data());
		s_CB.CreateAsUniformBuffer(sizeof(Matrix), eGpuReadCpuWrite, nullptr);

		s_VS.Create("Mesh.shader", "VSMain");
		s_PS.Create("Mesh.shader", "PSMain");
	}

	Matrix wvp = Matrix::Transpose(camera.GetWVPMatrix());
	s_CB.Update(&wvp, sizeof(Matrix));

	REngine::Instance().SetVertexShader(s_VS);
	REngine::Instance().SetPixelShader(s_PS);

	REngine::Instance().SetVertexBuffer(s_VB, sizeof(Vertex), 0U, 0U);
	REngine::Instance().SetIndexBuffer(s_IB, eR32_UInt, 0U);

	REngine::Instance().SetUniformBuffer(s_CB, 0U, eVertexShader);

	REngine::Instance().SetRasterizerState(RStaticState::WireframeNoneCulling);

	REngine::Instance().DrawIndexed(s_IndexCount, 0U, 0, eTriangleList);
}

void Model::AddBuffer(const std::vector<Vertex> &vertices)
{
	RBuffer buffer;
	buffer.CreateAsVertexBuffer(sizeof(Vertex) * vertices.size(), eGpuReadOnly, vertices.data());
	m_VertexBuffers.emplace_back(buffer);
}

void Model::AddBuffer(const std::vector<uint32_t> &indices)
{
	RBuffer buffer;
	buffer.CreateAsIndexBuffer(sizeof(uint32_t) * indices.size(), eGpuReadOnly, indices.data());
	m_IndexBuffers.emplace_back(buffer);
}

NamespaceEnd(Geometry)