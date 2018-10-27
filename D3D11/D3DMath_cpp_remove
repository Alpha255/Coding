#include "D3DMath.h"

#include "D3DGraphic.h"
#include "Camera.h"

extern D3DGraphic* g_Renderer;

//NamespaceBegin(Math)
//NamespaceBegin(Geometry)
//
//void Waves::Create(uint32_t row, uint32_t col, float spatialStep, float timeStep, float speed, float damping)
//{
//	m_Row = row;
//	m_Column = col;
//
//	m_VertexNum = row * col;
//	m_TriangleNum = (row - 1) * (col - 1) * 2;
//
//	m_TimeStep = timeStep;
//	m_SpatialStep = spatialStep;
//
//	float d = damping * timeStep + 2.0f;
//	float e = (speed * speed) * (timeStep * timeStep) / (spatialStep * spatialStep);
//	m_K1 = (damping * timeStep - 2.0f) / d;
//	m_K2 = (4.0f - 8.0f * e) / d;
//	m_K3 = (2.0f * e) / d;
//
//	SafeDeleteArray(m_pCurSolution);
//	SafeDeleteArray(m_pPrevSolution);
//	SafeDeleteArray(m_pNormals);
//
//	m_pCurSolution = new Vec3[row * col];
//	m_pPrevSolution = new Vec3[row * col];
//	m_pNormals = new Vec3[row * col];
//
//	// Generate grid vertices in system memory.
//	float halfWidth = (col - 1) * spatialStep * 0.5f;
//	float halfDepth = (row - 1) * spatialStep * 0.5f;
//	for (uint32_t i = 0; i < row; ++i)
//	{
//		float z = halfDepth - i * spatialStep;
//		for (uint32_t j = 0; j < col; ++j)
//		{
//			float x = -halfWidth + j * spatialStep;
//
//			m_pPrevSolution[i * col + j] = Vec3(x, 0.0f, z);
//			m_pCurSolution[i * col + j] = Vec3(x, 0.0f, z);
//			m_pNormals[i * col + j] = Vec3(0.0f, 1.0f, 0.0f);
//		}
//	}
//}
//
//void Waves::Update(float fElapsed)
//{
//	static float time = 0.0f;
//
//	/// Accumulate time.
//	time += fElapsed;
//
//	/// Only update the simulation at the specified time step.
//	if (time >= m_TimeStep)
//	{
//		/// Only update interior points; we use zero boundary conditions.
//		for (uint32_t i = 1; i < m_Row - 1; ++i)
//		{
//			for (uint32_t j = 1; j < m_Column - 1; ++j)
//			{
//				/// After this update we will be discarding the old previous
//				/// buffer, so overwrite that buffer with the new update.
//				/// Note how we can do this inplace (read/write to same element) 
//				/// because we won't need prev_ij again and the assignment happens last.
//
//				/// Note j indexes x and i indexes z: h(x_j, z_i, t_k)
//				/// Moreover, our +z axis goes "down"; this is just to 
//				/// keep consistent with our row indices going down.
//				m_pPrevSolution[i * m_Column + j].y =
//					m_K1 * m_pPrevSolution[i * m_Column + j].y +
//					m_K2 * m_pCurSolution[i * m_Column + j].y +
//					m_K3 * (m_pCurSolution[(i + 1) * m_Column + j].y +
//						m_pCurSolution[(i - 1) * m_Column + j].y +
//						m_pCurSolution[i * m_Column + j + 1].y +
//						m_pCurSolution[i * m_Column + j - 1].y);
//			}
//		}
//
//		/// We just overwrote the previous buffer with the new data, so
//		/// this data needs to become the current solution and the old
//		/// current solution becomes the new previous solution.
//		std::swap(m_pPrevSolution, m_pCurSolution);
//
//		time = 0.0f; /// reset time
//
//		for (uint32_t i = 1; i < m_Row - 1; ++i)
//		{
//			for (uint32_t j = 1; j < m_Column - 1; ++j)
//			{
//				/// float l = m_vpCurSolution[i * m_nCountColumn + j - 1].y;  for Tangent
//				float r = m_pCurSolution[i * m_Column + j + 1].y;
//				float t = m_pCurSolution[(i - 1) * m_Column + j].y;
//				float b = m_pCurSolution[(i + 1) * m_Column + j].y;
//
//				m_pNormals[i * m_Column + j].x = 1 - r;
//				m_pNormals[i * m_Column + j].y = 2.0f * m_SpatialStep;
//				m_pNormals[i * m_Column + j].z = b - t;
//
//				DirectX::XMVECTOR n = DirectX::XMVector3Normalize(XMLoadFloat3(&m_pNormals[i * m_Column + j]));
//				DirectX::XMStoreFloat3(&m_pNormals[i * m_Column + j], n);
//			}
//		}
//	}
//}
//
//void Waves::Disturb(uint32_t i, uint32_t j, float magnitude)
//{
//	/// Don't disturb boundaries.
//	assert(i > 1 && i < m_Row - 2);
//	assert(j > 1 && j < m_Column - 2);
//
//	float halfMag = 0.5f * magnitude;
//
//	/// Disturb the ijth vertex height and its neighbors.
//	m_pCurSolution[i * m_Column + j].y += magnitude;
//	m_pCurSolution[i * m_Column + j + 1].y += halfMag;
//	m_pCurSolution[i * m_Column + j - 1].y += halfMag;
//	m_pCurSolution[(i + 1) * m_Column + j].y += halfMag;
//	m_pCurSolution[(i - 1) * m_Column + j].y += halfMag;
//}
//
//
//struct MeshResource
//{
//	Ref<ID3D11InputLayout> VertexLayout;
//
//	Ref<ID3D11Buffer> VertexBuffer;
//	Ref<ID3D11Buffer> IndexBuffer;
//
//	Ref<ID3D11VertexShader> VertexShader;
//	Ref<ID3D11PixelShader> PixelShader;
//
//	Ref<ID3D11Buffer> CBufferVS;
//
//	uint32_t VertexCount = 0U;
//	bool Inited = false;
//};
//
//static MeshResource s_MeshResource;
//
//void Mesh::InitResource(bool bFlatSphere)
//{
//	if (s_MeshResource.Inited)
//	{
//		return;
//	}
//
//	assert(g_Renderer && !s_MeshResource.Inited);
//
//	D3D11_INPUT_ELEMENT_DESC layout[] =
//	{
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
//	};
//	g_Renderer->CreateVertexShaderAndInputLayout(s_MeshResource.VertexShader, s_MeshResource.VertexLayout, layout,
//		ARRAYSIZE(layout), "MeshNormal.hlsl", "VSMain");
//	g_Renderer->CreatePixelShader(s_MeshResource.PixelShader, "MeshNormal.hlsl", "PSMain");
//
//	g_Renderer->CreateConstantBuffer(s_MeshResource.CBufferVS, sizeof(Matrix), D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);
//
//	std::vector<Vec3> lineVertices;
//	if (bFlatSphere)
//	{
//		for (uint32_t i = 0U; i < Indices.size(); i += 3)
//		{
//			Vec3 v0(0.0f, 0.0f, 0.0f);
//			Vec3 v1 = Vertices[Indices[i]].Normal * 0.7f;
//			lineVertices.push_back(v0);
//			lineVertices.push_back(v1);
//		}
//	}
//	else
//	{
//		for (uint32_t i = 0U; i < Vertices.size(); i += 3)
//		{
//			Vec3 v0 = Vertices[i].Position;
//			Vec3 v1 = Vertices[i].Normal * 0.65f;
//			lineVertices.push_back(v0);
//			lineVertices.push_back(v1);
//		}
//	}
//
//	g_Renderer->CreateVertexBuffer(s_MeshResource.VertexBuffer, (uint32_t)(sizeof(Math::Geometry::Vertex) * lineVertices.size()), D3D11_USAGE_IMMUTABLE, &lineVertices[0]);
//
//	s_MeshResource.VertexCount = (uint32_t)lineVertices.size();
//
//	s_MeshResource.Inited = true;
//}
//
//void Mesh::DrawNormal(const Camera &cam, bool bFlatSphere)
//{
//	InitResource(bFlatSphere);
//
//	g_Renderer->SetInputLayout(s_MeshResource.VertexLayout);
//
//	g_Renderer->SetVertexBuffer(s_MeshResource.VertexBuffer, sizeof(Vec3), 0U, 0U);
//
//	Matrix wvp = cam.GetWorldMatrix() * cam.GetViewMatrix() * cam.GetProjMatrix();
//	wvp = wvp.Transpose();
//	g_Renderer->UpdateBuffer(s_MeshResource.CBufferVS, &wvp, sizeof(Matrix));
//	g_Renderer->SetConstantBuffer(s_MeshResource.CBufferVS, 0U, D3DGraphic::eVertexShader);
//
//	g_Renderer->SetVertexShader(s_MeshResource.VertexShader);
//	g_Renderer->SetPixelShader(s_MeshResource.PixelShader);
//
//	g_Renderer->Draw(s_MeshResource.VertexCount, 0U, D3D_PRIMITIVE_TOPOLOGY_LINELIST);
//}
//
//NamespaceEnd(Geometry)
//NamespaceEnd(Math)
//
//NamespaceBegin(Lighting)
//struct LightResource
//{
//	Ref<ID3D11InputLayout> VertexLayout;
//
//	Ref<ID3D11Buffer> VertexBuffer;
//	Ref<ID3D11Buffer> IndexBuffer;
//
//	Ref<ID3D11Buffer> CBufferVS;
//
//	Ref<ID3D11VertexShader> VertexShader;
//	Ref<ID3D11PixelShader> PixelShader;
//
//	Ref<ID3D11RasterizerState> Solid;
//
//	Matrix TransformMatrix;
//
//	///Ref<ID3D11BlendState> AlphaBlend;
//
//	bool Inited = false;
//
//	uint32_t IndexCount = 0U;
//};
//
//static LightResource s_LightResource;
//
//void InitResource(const Vec3 &pos)
//{
//	if (s_LightResource.Inited)
//	{
//		return;
//	}
//
//	assert(g_Renderer);
//
//	D3D11_INPUT_ELEMENT_DESC layout[] =
//	{
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
//	};
//	g_Renderer->CreateVertexShaderAndInputLayout(s_LightResource.VertexShader, s_LightResource.VertexLayout, layout,
//		ARRAYSIZE(layout), "Lamp.hlsl", "VSMain");
//	g_Renderer->CreatePixelShader(s_LightResource.PixelShader, "Lamp.hlsl", "PSMain");
//
//	g_Renderer->CreateConstantBuffer(s_LightResource.CBufferVS, sizeof(Matrix), D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);
//
//	g_Renderer->CreateRasterizerState(s_LightResource.Solid, D3D11_FILL_SOLID);
//
//	///D3D11_BLEND_DESC blendDesc = {};
//	///blendDesc.AlphaToCoverageEnable = false;
//	///blendDesc.IndependentBlendEnable = false;
//	///blendDesc.RenderTarget[0].BlendEnable = true;
//	///blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
//	///blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
//	///blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
//	///blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
//	///blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
//	///blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
//	///blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
//	///g_Renderer->CreateBlendState(s_LightResource.AlphaBlend, &blendDesc);
//
//	Math::Geometry::Mesh cylinder;
//	Math::Geometry::MakeCylinder(0.01f, 0.01f, 0.12f, 10U, 10U, cylinder);
//	
//	Math::Geometry::Mesh cone;
//	Math::Geometry::MakeCylinder(0.0f, 0.02f, 0.07f, 10U, 10U, cone);
//
//	for (uint32_t i = 0U; i < cone.Vertices.size(); ++i)
//	{
//		cone.Vertices[i].Position.y -= 0.12f * 0.8f;
//	}
//
//	const uint32_t cylinderVSize = (uint32_t)cylinder.Vertices.size();
//	for (uint32_t i = 0U; i < cone.Indices.size(); ++i)
//	{
//		cone.Indices[i] += cylinderVSize;
//	}
//
//	Math::Geometry::Mesh arrow;
//	arrow.Vertices.insert(arrow.Vertices.end(), cylinder.Vertices.begin(), cylinder.Vertices.end());
//	arrow.Vertices.insert(arrow.Vertices.end(), cone.Vertices.begin(), cone.Vertices.end());
//
//	arrow.Indices.insert(arrow.Indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());
//	arrow.Indices.insert(arrow.Indices.end(), cone.Indices.begin(), cone.Indices.end());
//
//	g_Renderer->CreateVertexBuffer(s_LightResource.VertexBuffer, (uint32_t)(sizeof(Math::Geometry::Vertex) * arrow.Vertices.size()), D3D11_USAGE_IMMUTABLE, &arrow.Vertices[0]);
//	g_Renderer->CreateIndexBuffer(s_LightResource.IndexBuffer, (uint32_t)(sizeof(uint32_t) * arrow.Indices.size()), D3D11_USAGE_IMMUTABLE, &arrow.Indices[0]);
//
//	s_LightResource.IndexCount = (uint32_t)arrow.Indices.size();
//
//	Vec3 v0(0.0f, 1.0f, 0.0f);
//	Vec3 v1 = pos;
//	Vec3 coord = Vec3::Cross(v0, v1);
//	float angle = acosf(v0 * v1);
//	Matrix rotate = Matrix::RotationAxis(coord.x, coord.y, coord.z, angle);
//	Matrix translation = Matrix::Translation(pos.x, pos.y, pos.z);
//	s_LightResource.TransformMatrix = rotate * translation;
//
//	s_LightResource.Inited = true;
//}
//
//void DrawLight(const Vec3 &pos, const Camera &cam)
//{
//	InitResource(pos);
//
//	g_Renderer->SetInputLayout(s_LightResource.VertexLayout);
//
//	g_Renderer->SetVertexBuffer(s_LightResource.VertexBuffer, sizeof(Math::Geometry::Vertex), 0U, 0U);
//	g_Renderer->SetIndexBuffer(s_LightResource.IndexBuffer, DXGI_FORMAT_R32_UINT, 0U);
//
//	Matrix wvp = cam.GetWorldMatrix() * s_LightResource.TransformMatrix * cam.GetViewMatrix() * cam.GetProjMatrix();
//	wvp = wvp.Transpose();
//	g_Renderer->UpdateBuffer(s_LightResource.CBufferVS, &wvp, sizeof(Matrix));
//	g_Renderer->SetConstantBuffer(s_LightResource.CBufferVS, 0U, D3DGraphic::eVertexShader);
//
//	g_Renderer->SetVertexShader(s_LightResource.VertexShader);
//	g_Renderer->SetPixelShader(s_LightResource.PixelShader);
//
//	g_Renderer->SetRasterizerState(s_LightResource.Solid);
//
//	g_Renderer->DrawIndexed(s_LightResource.IndexCount, 0U, 0);
//}
//NamespaceEnd(Lighting)