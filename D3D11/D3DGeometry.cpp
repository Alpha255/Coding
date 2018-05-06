#include "D3DGeometry.h"
#include "D3DEngine.h"
#include "Camera.h"
#include "System.h"

#include <fstream>
#include <sstream>

///#include <vcglib/apps/sample/trimesh_QT_shared/mesh.h>
///#include <vcglib/wrap/io_trimesh/import_obj.h>

NamespaceBegin(Geometry)

void Mesh::CreateRenderResource()
{
	VertexCount = (uint32_t)m_Vertices.size();
	IndexCount = (uint32_t)m_Indices.size();

	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3DVertexShader vertexShader;
	vertexShader.Create("Mesh.hlsl", "VSMain");
	VertexLayout.Create(vertexShader.GetBlob(), layout, 4U);

	VertexBuffer.CreateAsVertexBuffer(sizeof(Vertex) * m_Vertices.size(), D3DBuffer::eGpuReadOnly, m_Vertices.data());
	IndexBuffer.CreateAsIndexBuffer(sizeof(uint32_t) * m_Indices.size(), D3DBuffer::eGpuReadOnly, m_Indices.data());

	m_Created = true;
}

void Mesh::DrawNormal(const Camera &/*cam*/)
{

}

void Mesh::SubDivide()
{
	/// Save a copy of the input geometry.
	Mesh inputCopy = *this;

	m_Vertices.resize(0);
	m_Indices.resize(0);

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

		m_Vertices.push_back(v0); /// 0
		m_Vertices.push_back(v1); /// 1
		m_Vertices.push_back(v2); /// 2
		m_Vertices.push_back(m0); /// 3
		m_Vertices.push_back(m1); /// 4
		m_Vertices.push_back(m2); /// 5

		m_Indices.push_back(i * 6 + 0);
		m_Indices.push_back(i * 6 + 3);
		m_Indices.push_back(i * 6 + 5);
		
		m_Indices.push_back(i * 6 + 3);
		m_Indices.push_back(i * 6 + 4);
		m_Indices.push_back(i * 6 + 5);
		
		m_Indices.push_back(i * 6 + 5);
		m_Indices.push_back(i * 6 + 4);
		m_Indices.push_back(i * 6 + 2);
		
		m_Indices.push_back(i * 6 + 3);
		m_Indices.push_back(i * 6 + 1);
		m_Indices.push_back(i * 6 + 4);
	}
}

void Mesh::MakeCylinderTopBottomCap(bool bTop, float bottomRadius, float topRadius, float height, uint32_t slice)
{
	float y = bTop ? (0.5f * height) : (-0.5f * height);
	float r = bTop ? topRadius : bottomRadius;
	float cy = bTop ? 1.0f : -1.0f;
	float theta = DirectX::XM_2PI / slice;
	uint32_t baseIndex = (uint32_t)m_Vertices.size();

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
		m_Vertices.push_back(v);
	}

	/// Cap center vertex.
	Vertex center(0.0f, y, 0.0f, 0.0f, cy, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f);
	m_Vertices.push_back(center);

	/// Cache the index of center vertex.
	uint32_t centerIndex = (uint32_t)(m_Vertices.size() - 1U);

	for (uint32_t i = 0U; i < slice; ++i)
	{
		m_Indices.push_back(centerIndex);

		uint32_t i1 = bTop ? (baseIndex + i + 1) : (baseIndex + i);
		uint32_t i2 = bTop ? (baseIndex + i) : (baseIndex + i + 1);

		m_Indices.push_back(i1);
		m_Indices.push_back(i2);
	}
}

void Mesh::CreateAsBox(float width, float height, float depth)
{
	assert(!m_Created);

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

	m_Vertices.assign(&vertices[0], &vertices[24]);

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

	m_Indices.assign(&indices[0], &indices[36]);

	CreateRenderResource();
}

void Mesh::CreateAsCube(float width)
{
	CreateAsBox(width, width, width);
}

void Mesh::CreateAsSphere(float radius, uint32_t slice, uint32_t stack)
{
	assert(!m_Created);

	m_Vertices.clear();
	m_Indices.clear();

	Vertex tVertex(0.0f, radius, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	Vertex bVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	float phiStep = DirectX::XM_PI / stack;
	float thetaStep = 2.0f * DirectX::XM_PI / slice;

	/// Compute vertices for each stack ring (do not count the poles as rings).
	m_Vertices.push_back(tVertex);
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

			m_Vertices.push_back(vec);
		}
	}
	m_Vertices.push_back(bVertex);

	///
	/// Compute indices for top stack.  The top stack was written first to the vertex buffer
	/// and connects the top pole to the first ring.
	///
	for (uint32_t i = 1; i <= slice; ++i)
	{
		m_Indices.push_back(0);
		m_Indices.push_back(i + 1);
		m_Indices.push_back(i);
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
			m_Indices.push_back(baseIndex + i * ringVertexCount + j);
			m_Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			m_Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			
			m_Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			m_Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			m_Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	///
	/// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	/// and connects the bottom pole to the bottom ring.
	///
	/// South pole vertex was added last.
	uint32_t southPoleIndex = (uint32_t)m_Vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (uint32_t i = 0; i < slice; ++i)
	{
		m_Indices.push_back(southPoleIndex);
		m_Indices.push_back(baseIndex + i);
		m_Indices.push_back(baseIndex + i + 1);
	}

	CreateRenderResource();
}

void Mesh::CreateAsGeoSphere(float radius, uint32_t subDivisions)
{
	assert(!m_Created);

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

	m_Vertices.resize(12);
	m_Indices.resize(60);

	for (uint32_t i = 0; i < 12; ++i)
	{
		m_Vertices[i].Position = pos[i];
	}

	for (uint32_t i = 0; i < 60; ++i)
	{
		m_Indices[i] = k[i];
	}

	for (uint32_t i = 0; i < subDivisions; ++i)
	{
		SubDivide();
	}

	/// Project vertices onto sphere and scale.
	for (uint32_t i = 0; i < m_Vertices.size(); ++i)
	{
		/// Project onto unit sphere.
		DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&m_Vertices[i].Position));

		/// Project onto sphere.
		DirectX::XMVECTOR p = DirectX::XMVectorScale(n, radius);

		DirectX::XMStoreFloat3(&m_Vertices[i].Position, p);
		DirectX::XMStoreFloat3(&m_Vertices[i].Normal, n);

		/// Derive texture coordinates from spherical coordinates.
		float theta = Math::AngleFromXY(m_Vertices[i].Position.x, m_Vertices[i].Position.z);

		float phi = acosf(m_Vertices[i].Position.y / radius);

		m_Vertices[i].UV.x = theta / DirectX::XM_2PI;
		m_Vertices[i].UV.y = phi / DirectX::XM_PI;

		/// Partial derivative of P with respect to theta
		m_Vertices[i].Tangent.x = -radius * sinf(phi) * sinf(theta);
		m_Vertices[i].Tangent.y = 0.0f;
		m_Vertices[i].Tangent.z = radius * sinf(phi) * cosf(theta);

		DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&m_Vertices[i].Tangent);
		DirectX::XMStoreFloat3(&m_Vertices[i].Tangent, DirectX::XMVector3Normalize(T));
	}

	CreateRenderResource();
}

void Mesh::CreateAsFlatGeoSphere(float radius, uint32_t subDivisions)
{
	CreateAsGeoSphere(radius, subDivisions);

	uint32_t numTris = (uint32_t)m_Indices.size() / 3;
	for (uint32_t i = 0; i < numTris; ++i)
	{
		Vertex &v0 = m_Vertices[m_Indices[i * 3 + 0]];
		Vertex &v1 = m_Vertices[m_Indices[i * 3 + 1]];
		Vertex &v2 = m_Vertices[m_Indices[i * 3 + 2]];

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

void Mesh::CreateAsCylinder(float bottomRadius, float topRadius, float height, uint32_t slice, uint32_t stack)
{
	assert(!m_Created);

	m_Vertices.clear();
	m_Indices.clear();

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

			m_Vertices.push_back(v);
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
			m_Indices.push_back(i * ringVertexCount + j);
			m_Indices.push_back((i + 1) * ringVertexCount + j);
			m_Indices.push_back((i + 1) * ringVertexCount + j + 1);
			
			m_Indices.push_back(i * ringVertexCount + j);
			m_Indices.push_back((i + 1) * ringVertexCount + j + 1);
			m_Indices.push_back(i * ringVertexCount + j + 1);
		}
	}

	MakeCylinderTopBottomCap(true, bottomRadius, topRadius, height, slice);
	MakeCylinderTopBottomCap(false, bottomRadius, topRadius, height, slice);

	CreateRenderResource();
}

void Mesh::CreateAsGrid(float width, float depth, uint32_t m, uint32_t n)
{
	assert(!m_Created);

	uint32_t vertexCount = m * n;
	uint32_t faceCount = (m - 1) * (n - 1) * 2;

	/// Create the vertices
	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	m_Vertices.resize(vertexCount);
	for (uint32_t i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;
		for (uint32_t j = 0; j < n; ++j)
		{
			float x = -halfWidth + j * dx;

			m_Vertices[i * n + j].Position = Vec3(x, 0.0f, z);
			m_Vertices[i * n + j].Normal = Vec3(0.0f, 1.0f, 0.0f);
			m_Vertices[i * n + j].Tangent = Vec3(1.0f, 0.0f, 0.0f);
			
			m_Vertices[i * n + j].UV.x = j * du;
			m_Vertices[i * n + j].UV.y = i * dv;
		}		 
	}

	/// Create the indices
	m_Indices.resize(faceCount * 3);

	uint32_t k = 0;
	for (uint32_t i = 0; i < m - 1; ++i)
	{
		for (uint32_t j = 0; j < n - 1; ++j)
		{
			m_Indices[k] = i * n + j;
			m_Indices[k + 1] = i * n + j + 1;
			m_Indices[k + 2] = (i + 1) * n + j;
			
			m_Indices[k + 3] = (i + 1) * n + j;
			m_Indices[k + 4] = i * n + j + 1;
			m_Indices[k + 5] = (i + 1) * n + j + 1;

			k += 6;
		}
	}

	CreateRenderResource();
}

void Mesh::CreateAsQuad(float length)
{
	assert(!m_Created);

	m_Vertices.resize(4U);
	m_Indices.resize(6U);

	float halfLen = length / 2.0f;

	/// Position coordinates specified in NDC space.
	m_Vertices[0] = Vertex(
		-halfLen, -halfLen, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f);

	m_Vertices[1] = Vertex(
		-halfLen, +halfLen, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f);

	m_Vertices[2] = Vertex(
		+halfLen, +halfLen, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f);

	m_Vertices[3] = Vertex(
		+halfLen, -halfLen, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f);

	m_Indices[0] = 0;
	m_Indices[1] = 1;
	m_Indices[2] = 2;
	
	m_Indices[3] = 0;
	m_Indices[4] = 2;
	m_Indices[5] = 3;

	CreateRenderResource();
}

void Mesh::CreateAsQuad(float left, float top, float width, float height)
{
	assert(!m_Created);

	m_Vertices.resize(4U);
	m_Indices.resize(6U);

	/// Position coordinates specified in NDC space.
	m_Vertices[0] = Vertex(
		left, top - height, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f);

	m_Vertices[1] = Vertex(
		left, top, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f);

	m_Vertices[2] = Vertex(
		left + width, top, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f);

	m_Vertices[3] = Vertex(
		left + width, top - height, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f);

	m_Indices[0] = 0;
	m_Indices[1] = 1;
	m_Indices[2] = 2;

	m_Indices[3] = 0;
	m_Indices[4] = 2;
	m_Indices[5] = 3;

	CreateRenderResource();
}

void ObjMesh::Create(const char *pFileName)
{
	assert(pFileName);

	std::string meshFilePath = System::ResourceFilePath(pFileName, System::eObjMesh);

	///vcg::tri::io::ImporterOBJ<CMeshO>::Info oi;
	///oi.cb = nullptr;
	///bool maskLoaded = vcg::tri::io::ImporterOBJ<CMeshO>::LoadMask(meshFilePath.c_str(), oi);
	///if (!maskLoaded)
	///{
	///	assert(!"Failed to load file mask!!!");
	///}
	
	///CMeshO cm;
	///int result = vcg::tri::io::ImporterOBJ<CMeshO>::Open(cm, meshFilePath.c_str(), oi);
	///if (result != vcg::tri::io::ImporterOBJ<CMeshO>::E_NOERROR)
	///{
	///	const char *pErrMsg = vcg::tri::io::ImporterOBJ<CMeshO>::ErrorMsg(result);
	///	OutputDebugStringA(pErrMsg);
	///	assert(!"Failed to load obj file!!!");
	///}

	std::ifstream meshFile(meshFilePath.c_str(), std::ios::in);
	if (meshFile.good())
	{
		char line[MAX_PATH] = { 0 };
		std::vector<Vec3> vertices;
		std::vector<ObjIndex> indices;
		std::vector<Vec3> normals;
		std::vector<Vec2> uvs;

		while (meshFile >> line)
		{
			if (0 == strcmp(line, "#"))
			{
				meshFile.getline(line, MAX_PATH);
			}
			else if (0 == strcmp(line, "v"))   		/// Read vertices
			{
				Vec3 v;
				meshFile >> v.x >> v.y >> v.z;
				vertices.push_back(v);
			}
			else if (0 == strcmp(line, "vn"))       /// Read normals
			{
				Vec3 vn;
				meshFile >> vn.x >> vn.y >> vn.z;
				normals.push_back(vn);
			}
			else if (0 == strcmp(line, "vt"))   	/// Read uvs
			{
				Vec2 vt;
				meshFile >> vt.x >> vt.y;
				uvs.push_back(vt);
			}
			else if (0 == strcmp(line, "f"))  		/// Read faces
			{
				meshFile.getline(line, MAX_PATH);

				/// f 1      i
				/// f 1/1    i/t
				/// f 1//1   i/n
				/// f 1/1/1  i/t/n
				ObjIndex index[4];
				const char *pStrBeg = &line[1];
				
				std::stringstream ss(pStrBeg);
				uint32_t m = 0U;
				while (!ss.eof())
				{
					ss >> index[m].i;
					const char *pStr = pStrBeg + ss.tellg();

					if (' ' == *pStr)
					{
						++m;
						continue;
					}
					else if ('/' == *pStr)
					{
						ss.get();
						pStr = pStrBeg + ss.tellg();
						if ('/' == *pStr)
						{
							ss.get();
							ss >> index[m].n;
						}
						else
						{
							ss >> index[m].t;
							pStr = pStrBeg + ss.tellg();
							if (' ' == *pStr)
							{
								++m;
								continue;
							}
							else if ('/' == *pStr)
							{
								ss.get();
								ss >> index[m].n;
							}
							else
							{
								assert(!"Invalid obj file!!");
							}
						}
					}
					else
					{
						assert(!"Invalid obj file!!");
					}

					++m;
				}

				indices.insert(indices.end(), &index[0], &index[0] + 4);
			}
		}

		meshFile.close();

		CreateVertexData(vertices, indices, normals, uvs);

		CreateRenderResource();
	}
	else
	{
		assert(0);
	}
}

void ObjMesh::CreateVertexData(
	const std::vector<Vec3> &srcVertices, 
	const std::vector<ObjIndex> &objIndices, 
	const std::vector<Vec3> &normals, 
	const std::vector<Vec2> &uvs)
{
	for (uint32_t i = 0U; i < objIndices.size(); i += 4)
	{
		/*
		*  A face polygon composed of more than three vertices is triangulated
		*  according to the following schema:
		*                      v5
		*                     /  \
		*                    /    \
		*                   /      \
		*                  v1------v4
		*                  |\      /
		*                  | \    /
		*                  |  \  /
		*                 v2---v3
		*
		*  As shown above, the 5 vertices polygon (v1,v2,v3,v4,v5)
		*  has been split into the triangles (v1,v2,v3), (v1,v3,v4) e (v1,v4,v5).
		*  This way vertex v1 becomes the common vertex of all newly generated
		*  triangles, and this may lead to the creation of very thin triangles.
		*/
		/// v0 v1 v2
		for (uint32_t j = i; j < i + 3U; ++j)
		{
			const ObjIndex &face = objIndices.at(j);

			assert(face.i >= 0 && face.n >= 0U && face.t >= 0U);

			m_Indices.push_back((uint32_t)m_Vertices.size());

			Vertex v;
			v.Position = srcVertices.at(face.i - 1U);
			///v.Position.x *= -1.0f;
			v.Normal = face.n > 0U ? normals.at(face.n - 1U) : Vec3(0.0f, 0.0f, 0.0f);
			v.UV = face.t > 0 ? uvs.at(face.t - 1U) : Vec2(0.0f, 0.0f);
			m_Vertices.push_back(v);
		}

		/// v3
		if (objIndices.at(i + 3U).i > 0U)  /// Quad ?
		{
			const ObjIndex &face = objIndices.at(i + 3U);

			assert(face.i >= 0 && face.n >= 0U && face.t >= 0U);

			Vertex v;
			v.Position = srcVertices.at(face.i - 1U);
			///v.Position.x *= -1.0f;
			v.Normal = face.n > 0U ? normals.at(face.n - 1U) : Vec3(0.0f, 0.0f, 0.0f);
			v.UV = face.t > 0 ? uvs.at(face.t - 1U) : Vec2(0.0f, 0.0f);
			m_Vertices.push_back(v);

			uint32_t vertexCount = (uint32_t)m_Vertices.size();

			m_Indices.push_back(vertexCount - 4U);
			m_Indices.push_back(vertexCount - 2U);
			m_Indices.push_back(vertexCount - 1U);
		}
	}
}

NamespaceEnd(Geometry)