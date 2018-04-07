#include "D3DLighting.h"
#include "D3DEngine.h"
#include "Camera.h"

Geometry::ObjMesh Light::s_PointLightMesh;
Geometry::ObjMesh Light::s_DirLightMesh;
Geometry::ObjMesh Light::s_SpotLightMesh;

D3DVertexShader Light::VertexShader;
D3DPixelShader Light::PixelShader;
D3DBuffer Light::CBufferVS;

bool Light::Inited = false;

void Light::Initialize()
{
	if (Inited)
	{
		return;
	}

	s_PointLightMesh.Create("PointLight.obj");
	///s_DirLightMesh.Create("");
	///s_SpotLightMesh.Create("");

	VertexShader.Create("DrawLight.hlsl", "VSMain");
	PixelShader.Create("DrawLight.hlsl", "PSMain");

	CBufferVS.CreateAsConstantBuffer(sizeof(Matrix), D3DBuffer::eGpuReadCpuWrite, nullptr);

	Inited = true;
}

void Light::DrawLight(const Vec3 &position, const eLightType type, const Camera &cam)
{
	Initialize();

	D3DEngine::Instance().SetInputLayout(s_PointLightMesh.VertexLayout);
	D3DEngine::Instance().SetVertexShader(VertexShader);
	D3DEngine::Instance().SetPixelShader(PixelShader);

	D3DEngine::Instance().SetVertexBuffer(s_PointLightMesh.VertexBuffer, sizeof(Geometry::Vertex), 0U);
	D3DEngine::Instance().SetIndexBuffer(s_PointLightMesh.IndexBuffer, eR32_UInt, 0U);

	Matrix world = cam.GetWorldMatrix();
	Matrix scale = Matrix::Scaling(0.001f);
	Matrix translation = Matrix::Translation(position.x, position.y, position.z);
	Matrix wvp = Matrix::Transpose(world * scale * translation * cam.GetViewMatrix() * cam.GetProjMatrix());
	CBufferVS.Update(&wvp, sizeof(Matrix));
	D3DEngine::Instance().SetConstantBuffer(CBufferVS, 0U, D3DShader::eVertexShader);

	D3DEngine::Instance().DrawIndexed(s_PointLightMesh.IndexCount, 0U, 0, eTriangleList);
}