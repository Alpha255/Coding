#include "D3DMath.h"

NamespaceBegin(Math)
NamespaceBegin(Geometry)

void Waves::Create(uint32_t row, uint32_t col, float spatialStep, float timeStep, float speed, float damping)
{
	m_Row = row;
	m_Column = col;

	m_VertexNum = row * col;
	m_TriangleNum = (row - 1) * (col - 1) * 2;

	m_TimeStep = timeStep;
	m_SpatialStep = spatialStep;

	float d = damping * timeStep + 2.0f;
	float e = (speed * speed) * (timeStep * timeStep) / (spatialStep * spatialStep);
	m_K1 = (damping * timeStep - 2.0f) / d;
	m_K2 = (4.0f - 8.0f * e) / d;
	m_K3 = (2.0f * e) / d;

	SafeDeleteArray(m_pCurSolution);
	SafeDeleteArray(m_pPrevSolution);
	SafeDeleteArray(m_pNormals);

	m_pCurSolution = new Vec3[row * col];
	m_pPrevSolution = new Vec3[row * col];
	m_pNormals = new Vec3[row * col];

	// Generate grid vertices in system memory.
	float halfWidth = (col - 1) * spatialStep * 0.5f;
	float halfDepth = (row - 1) * spatialStep * 0.5f;
	for (uint32_t i = 0; i < row; ++i)
	{
		float z = halfDepth - i * spatialStep;
		for (uint32_t j = 0; j < col; ++j)
		{
			float x = -halfWidth + j * spatialStep;

			m_pPrevSolution[i * col + j] = Vec3(x, 0.0f, z);
			m_pCurSolution[i * col + j] = Vec3(x, 0.0f, z);
			m_pNormals[i * col + j] = Vec3(0.0f, 1.0f, 0.0f);
		}
	}
}

void Waves::Update(float fElapsed)
{
	static float time = 0.0f;

	/// Accumulate time.
	time += fElapsed;

	/// Only update the simulation at the specified time step.
	if (time >= m_TimeStep)
	{
		/// Only update interior points; we use zero boundary conditions.
		for (uint32_t i = 1; i < m_Row - 1; ++i)
		{
			for (uint32_t j = 1; j < m_Column - 1; ++j)
			{
				/// After this update we will be discarding the old previous
				/// buffer, so overwrite that buffer with the new update.
				/// Note how we can do this inplace (read/write to same element) 
				/// because we won't need prev_ij again and the assignment happens last.

				/// Note j indexes x and i indexes z: h(x_j, z_i, t_k)
				/// Moreover, our +z axis goes "down"; this is just to 
				/// keep consistent with our row indices going down.
				m_pPrevSolution[i * m_Column + j].y =
					m_K1 * m_pPrevSolution[i * m_Column + j].y +
					m_K2 * m_pCurSolution[i * m_Column + j].y +
					m_K3 * (m_pCurSolution[(i + 1) * m_Column + j].y +
						m_pCurSolution[(i - 1) * m_Column + j].y +
						m_pCurSolution[i * m_Column + j + 1].y +
						m_pCurSolution[i * m_Column + j - 1].y);
			}
		}

		/// We just overwrote the previous buffer with the new data, so
		/// this data needs to become the current solution and the old
		/// current solution becomes the new previous solution.
		std::swap(m_pPrevSolution, m_pCurSolution);

		time = 0.0f; /// reset time

		for (uint32_t i = 1; i < m_Row - 1; ++i)
		{
			for (uint32_t j = 1; j < m_Column - 1; ++j)
			{
				/// float l = m_vpCurSolution[i * m_nCountColumn + j - 1].y;  for Tangent
				float r = m_pCurSolution[i * m_Column + j + 1].y;
				float t = m_pCurSolution[(i - 1) * m_Column + j].y;
				float b = m_pCurSolution[(i + 1) * m_Column + j].y;

				m_pNormals[i * m_Column + j].x = 1 - r;
				m_pNormals[i * m_Column + j].y = 2.0f * m_SpatialStep;
				m_pNormals[i * m_Column + j].z = b - t;

				DirectX::XMVECTOR n = DirectX::XMVector3Normalize(XMLoadFloat3(&m_pNormals[i * m_Column + j]));
				DirectX::XMStoreFloat3(&m_pNormals[i * m_Column + j], n);
			}
		}
	}
}

void Waves::Disturb(uint32_t i, uint32_t j, float magnitude)
{
	/// Don't disturb boundaries.
	assert(i > 1 && i < m_Row - 2);
	assert(j > 1 && j < m_Column - 2);

	float halfMag = 0.5f * magnitude;

	/// Disturb the ijth vertex height and its neighbors.
	m_pCurSolution[i * m_Column + j].y += magnitude;
	m_pCurSolution[i * m_Column + j + 1].y += halfMag;
	m_pCurSolution[i * m_Column + j - 1].y += halfMag;
	m_pCurSolution[(i + 1) * m_Column + j].y += halfMag;
	m_pCurSolution[(i - 1) * m_Column + j].y += halfMag;
}

void MakeBox(float width, float height, float depth, Mesh& meshData)
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

	meshData.Vertices.assign(&vertices[0], &vertices[24]);

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

	meshData.Indices.assign(&indices[0], &indices[36]);
}

void MakeCube(float width, Mesh& mesh)
{
	MakeBox(width, width, width, mesh);
}

void MakeSphere(float radius, uint32_t slice, uint32_t stack, Mesh& mesh)
{
	/// Bugs....
	/// https://www.web-tinker.com/article/20167.html
	mesh.Vertices.clear();
	mesh.Indices.clear();

	Vertex tVertex(0.0f, radius, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	Vertex bVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	float phiStep = DirectX::XM_PI / stack;
	float thetaStep = 2.0f * DirectX::XM_PI / slice;

	/// Compute vertices for each stack ring (do not count the poles as rings).
	mesh.Vertices.push_back(tVertex);
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

			mesh.Vertices.push_back(vec);
		}
	}
	mesh.Vertices.push_back(bVertex);

	///
	/// Compute indices for top stack.  The top stack was written first to the vertex buffer
	/// and connects the top pole to the first ring.
	///
	for (uint32_t i = 1; i <= slice; ++i)
	{
		mesh.Indices.push_back(0);
		mesh.Indices.push_back(i + 1);
		mesh.Indices.push_back(i);
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
			mesh.Indices.push_back(baseIndex + i * ringVertexCount + j);
			mesh.Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			mesh.Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			mesh.Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			mesh.Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			mesh.Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	///
	/// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	/// and connects the bottom pole to the bottom ring.
	///
	/// South pole vertex was added last.
	uint32_t southPoleIndex = (uint32_t)mesh.Vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (uint32_t i = 0; i < slice; ++i)
	{
		mesh.Indices.push_back(southPoleIndex);
		mesh.Indices.push_back(baseIndex + i);
		mesh.Indices.push_back(baseIndex + i + 1);
	}
}

void MakeGeoSphere(float radius, uint32_t subDivisions, Mesh& mesh)
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

	mesh.Vertices.resize(12);
	mesh.Indices.resize(60);

	for (uint32_t i = 0; i < 12; ++i)
	{
		mesh.Vertices[i].Position = pos[i];
	}

	for (uint32_t i = 0; i < 60; ++i)
	{
		mesh.Indices[i] = k[i];
	}
		
	for (uint32_t i = 0; i < subDivisions; ++i)
	{
		SubDivide(mesh);
	}

	/// Project vertices onto sphere and scale.
	for (uint32_t i = 0; i < mesh.Vertices.size(); ++i)
	{
		/// Project onto unit sphere.
		DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&mesh.Vertices[i].Position));

		/// Project onto sphere.
		DirectX::XMVECTOR p = DirectX::XMVectorScale(n, radius);  

		DirectX::XMStoreFloat3(&mesh.Vertices[i].Position, p);
		DirectX::XMStoreFloat3(&mesh.Vertices[i].Normal, n);

		/// Derive texture coordinates from spherical coordinates.
		float theta = AngleFromXY(mesh.Vertices[i].Position.x, mesh.Vertices[i].Position.z);

		float phi = acosf(mesh.Vertices[i].Position.y / radius);

		mesh.Vertices[i].UV.x = theta / DirectX::XM_2PI;
		mesh.Vertices[i].UV.y = phi / DirectX::XM_PI;

		/// Partial derivative of P with respect to theta
		mesh.Vertices[i].Tangent.x = -radius * sinf(phi) * sinf(theta);
		mesh.Vertices[i].Tangent.y = 0.0f;
		mesh.Vertices[i].Tangent.z = radius * sinf(phi) * cosf(theta);

		DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&mesh.Vertices[i].Tangent);
		DirectX::XMStoreFloat3(&mesh.Vertices[i].Tangent, DirectX::XMVector3Normalize(T));
	}
}

void MakeCylinder(float /*bottomRadius*/, float /*topRadius*/, float /*height*/, uint32_t /*slice*/, uint32_t /*stack*/, Mesh& /*mesh*/)
{
	assert(!"Unsupport yet!!!");
}

void MakeGrid(float width, float depth, uint32_t m, uint32_t n, Mesh& mesh)
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

	mesh.Vertices.resize(vertexCount);
	for (uint32_t i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;
		for (uint32_t j = 0; j < n; ++j)
		{
			float x = -halfWidth + j * dx;

			mesh.Vertices[i * n + j].Position = Vec3(x, 0.0f, z);
			mesh.Vertices[i * n + j].Normal = Vec3(0.0f, 1.0f, 0.0f);
			mesh.Vertices[i * n + j].Tangent = Vec3(1.0f, 0.0f, 0.0f);

			mesh.Vertices[i * n + j].UV.x = j * du;
			mesh.Vertices[i * n + j].UV.y = i * dv;
		}
	}

	/// Create the indices
	mesh.Indices.resize(faceCount * 3);

	uint32_t k = 0;
	for (uint32_t i = 0; i < m - 1; ++i)
	{
		for (uint32_t j = 0; j < n - 1; ++j)
		{
			mesh.Indices[k] = i * n + j;
			mesh.Indices[k + 1] = i * n + j + 1;
			mesh.Indices[k + 2] = (i + 1) * n + j;

			mesh.Indices[k + 3] = (i + 1) * n + j;
			mesh.Indices[k + 4] = i * n + j + 1;
			mesh.Indices[k + 5] = (i + 1) * n + j + 1;

			k += 6;
		}
	}
}

void MakeQuad(Mesh& /*mesh*/)
{
	assert(!"Unsupport yet!!!");
}

void SubDivide(Mesh& mesh)
{
	/// Save a copy of the input geometry.
	Mesh inputCopy = mesh;

	mesh.Vertices.resize(0);
	mesh.Indices.resize(0);

	///       v1
	///       *
	///      / \
	///     /   \
	///  m0*-----*m1
    ///   / \   / \
	///  /   \ /   \
	/// *-----*-----*
    /// v0    m2     v2

	uint32_t numTris = (uint32_t)inputCopy.Indices.size() / 3;
	for (uint32_t i = 0; i < numTris; ++i)
	{
		Vertex v0 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 0]];
		Vertex v1 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 1]];
		Vertex v2 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 2]];

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

		mesh.Vertices.push_back(v0); /// 0
		mesh.Vertices.push_back(v1); /// 1
		mesh.Vertices.push_back(v2); /// 2
		mesh.Vertices.push_back(m0); /// 3
		mesh.Vertices.push_back(m1); /// 4
		mesh.Vertices.push_back(m2); /// 5

		mesh.Indices.push_back(i * 6 + 0);
		mesh.Indices.push_back(i * 6 + 3);
		mesh.Indices.push_back(i * 6 + 5);

		mesh.Indices.push_back(i * 6 + 3);
		mesh.Indices.push_back(i * 6 + 4);
		mesh.Indices.push_back(i * 6 + 5);

		mesh.Indices.push_back(i * 6 + 5);
		mesh.Indices.push_back(i * 6 + 4);
		mesh.Indices.push_back(i * 6 + 2);

		mesh.Indices.push_back(i * 6 + 3);
		mesh.Indices.push_back(i * 6 + 1);
		mesh.Indices.push_back(i * 6 + 4);
	}
}

NamespaceEnd(Geometry)
NamespaceEnd(Math)