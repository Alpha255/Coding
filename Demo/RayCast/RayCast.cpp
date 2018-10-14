
#include "RayCast.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "ImGui.h"
#include "System.h"
#include <fstream>

void AppRayCast::LoadMesh()
{
	std::string meshPath = ResourceFilePath("car.txt", System::eTxtMesh);
	std::ifstream meshFile(meshPath.c_str(), std::ios::in);
	assert(meshFile.good());

	uint32_t vertexCount = 0U, indexCount = 0U;
	std::string ignore;
	meshFile >> ignore >> vertexCount;
	meshFile >> ignore >> indexCount;
	meshFile >> ignore >> ignore >> ignore >> ignore;

	Vec3 vMin(FLT_MAX, FLT_MAX, FLT_MAX);
	Vec3 vMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	m_Verties.resize(vertexCount);
	for (uint32_t i = 0U; i < vertexCount; ++i)
	{
		meshFile >> m_Verties[i].Pos.x >> m_Verties[i].Pos.y >> m_Verties[i].Pos.z;
		meshFile >> m_Verties[i].Normal.x >> m_Verties[i].Normal.y >> m_Verties[i].Normal.z;

		vMin = Vec3::Min(vMin, m_Verties[i].Pos);
		vMax = Vec3::Max(vMax, m_Verties[i].Pos);
	}

	m_MeshBox.Center = (vMax + vMin) * 0.5f;
	m_MeshBox.Extents = (vMax - vMin) * 0.5f;

	meshFile >> ignore;
	meshFile >> ignore;
	meshFile >> ignore;

	m_Indices.resize(indexCount * 3U);
	for (uint32_t i = 0U; i < indexCount; ++i)
	{
		meshFile >> m_Indices[i * 3 + 0] >> m_Indices[i * 3 + 1] >> m_Indices[i * 3 + 2];
	}

	meshFile.close();

	m_VertexBuffer.CreateAsVertexBuffer(sizeof(Vertex) * vertexCount, D3DBuffer::eGpuReadOnly, &m_Verties[0]);
	m_IndexBuffer.CreateAsIndexBuffer(sizeof(uint32_t) * indexCount * 3, D3DBuffer::eGpuReadOnly, &m_Indices[0]);
}

void AppRayCast::Initialize()
{
	LoadMesh();
	m_VertexShader.Create("RayCast.hlsl", "VSMain");
	m_PixelShader.Create("RayCast.hlsl", "PSMain");

	m_ConstantBuffer.CreateAsConstantBuffer(sizeof(Matrix), D3DBuffer::eGpuReadCpuWrite);

	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	m_VertexLayout.Create(m_VertexShader.GetBlob(), layout, _countof(layout));
}

void AppRayCast::RenderScene()
{
    D3DEngine::Instance().ResetDefaultRenderSurfaces();
    D3DEngine::Instance().SetViewport(D3DViewport(0.0f, 0.0f, (float)m_Width, (float)m_Height));

	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);

	D3DEngine::Instance().SetVertexBuffer(m_VertexBuffer, sizeof(Vertex), 0U, 0U);
	D3DEngine::Instance().SetIndexBuffer(m_IndexBuffer, eR32_UInt, 0U);
	D3DEngine::Instance().SetInputLayout(m_VertexLayout);

	D3DEngine::Instance().SetRasterizerState(D3DStaticState::Wireframe);

	Matrix wvp = Matrix::Transpose(m_Camera->GetWVPMatrix());
	m_ConstantBuffer.Update(&wvp, sizeof(Matrix));
	D3DEngine::Instance().SetConstantBuffer(m_ConstantBuffer, 0U, D3DShader::eVertexShader);

	D3DEngine::Instance().DrawIndexed((uint32_t)m_Indices.size(), 0U, 0, eTriangleList);
}

void AppRayCast::Pick(int32_t x, int32_t y)
{
	/// Compute picking ray in view space.
	Matrix proj = m_Camera->GetProjMatrix();
	
	float vx = (2.0f * x / m_Width - 1.0f) / proj.m[0][0];
	float vy = (-2.0f * y / m_Height + 1.0f) / proj.m[1][1];

	/// Ray definition in view space.
	Vec4 rayOrigin(0.0f, 0.0f, 0.0f, 1.0f);
	Vec4 rayDir(vx, vy, 1.0f, 0.0f);

	/// Tranform ray to local space of Mesh.
	Matrix view = m_Camera->GetViewMatrix();
	view.Inverse();

	Matrix world = m_Camera->GetWorldMatrix();
	world.Inverse();

	Matrix toLocal = view * world;

	rayOrigin.TransformCoord(toLocal);
	rayDir.TransformNormal(toLocal);

	/// Make the ray direction unit length for the intersection tests.
	rayDir.Normalize();

	/// If we hit the bounding box of the Mesh, then we might have picked a Mesh triangle,
	/// so do the ray/triangle tests.
	/// If we did not hit the bounding box, then it is impossible that we hit 
	/// the Mesh, so do not waste effort doing ray/triangle tests.

	/// Assume we have not picked anything yet, so init to -1.
	m_PickedTriangleIndex = UINT_MAX;
	float tmin0 = 0.0f;
	if (!IsIntersectRayAxisAlignedBox(rayOrigin, rayDir, tmin0))
	{
		return;
	}

	/// Find the nearest ray/triangle intersection.
	tmin0 = FLT_MAX;
	for (uint32_t i = 0U; i < m_Indices.size() / 3U; ++i)
	{
		/// We have to iterate over all the triangles in order to find the nearest intersection.
		Vec3 v0 = m_Verties[m_Indices[i * 3 + 0]].Pos;
		Vec3 v1 = m_Verties[m_Indices[i * 3 + 1]].Pos;
		Vec3 v2 = m_Verties[m_Indices[i * 3 + 2]].Pos;

		float tmin1 = FLT_MAX;
		if (IsIntersectRayTriangle(rayOrigin, rayDir, v0, v1, v2, tmin1))
		{
			if (tmin1 < tmin0)
			{
				/// This is the new nearest picked triangle.
				tmin0 = tmin1;
				m_PickedTriangleIndex = i;
			}
		}
	}
}

void AppRayCast::MouseButtonDown(::WPARAM wParam, int32_t x, int32_t y)
{
	Base::MouseButtonDown(wParam, x, y);

	if (wParam & MK_RBUTTON)
	{
		Pick(x, y);
	}
}

bool AppRayCast::IsIntersectRayAxisAlignedBox(const Vec4 &origin, const Vec4 &dir, float &min)
{
	Vec4 vEpsilon(1e-20f, 1e-20f, 1e-20f, 1e-20f);
	Vec4 vFloatMin(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);
	Vec4 vFloatMax(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);

	/// Adjust ray origin to be relative to center of the box.

	return false;
}

bool AppRayCast::IsIntersectRayTriangle(const Vec4 &origin, const Vec4 &dir, const Vec3 &v0, const Vec3 &v1, const Vec3 &v2, float &min)
{
	return false;
}
