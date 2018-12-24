#include "D3DLighting.h"
#include "D3DEngine.h"
#include "Camera.h"

Geometry::ObjMesh Light::s_PointLightMesh;

D3DVertexShader Light::s_VertexShader;
D3DPixelShader Light::s_PixelShader;
D3DBuffer Light::s_CBufferVS;

bool Light::s_Inited = false;

void Light::Initialize()
{
	if (s_Inited)
	{
		return;
	}

	s_PointLightMesh.Create("Lightbulb.obj");

	s_VertexShader.Create("Mesh.hlsl", "VSMain");
	s_PixelShader.Create("Mesh.hlsl", "PSMain");

	s_CBufferVS.CreateAsConstantBuffer(sizeof(Matrix), D3DBuffer::eGpuReadCpuWrite, nullptr);

	s_Inited = true;
}

void Light::DebugDisplay(const Vec3 &position, const eLightType, const Camera &cam, const float scale)
{
	Initialize();

	D3DEngine::Instance().SetVertexShader(s_VertexShader);
	D3DEngine::Instance().SetPixelShader(s_PixelShader);

	s_PointLightMesh.Bind();

	Matrix world = cam.GetWorldMatrix();
	Matrix scaling = Matrix::Scaling(scale);
	Matrix translation = Matrix::Translation(position.x, position.y, position.z);
	Matrix rotate = Matrix::RotationAxis(1.0f, 0.0f, 0.0f, -90.0f);
	Matrix wvp = Matrix::Transpose(world * rotate * scaling * translation * cam.GetViewMatrix() * cam.GetProjMatrix());
	s_CBufferVS.Update(&wvp, sizeof(Matrix));
	D3DEngine::Instance().SetConstantBuffer(s_CBufferVS, 0U, D3DShader::eVertexShader);

	D3DEngine::Instance().DrawIndexed(s_PointLightMesh.IndexCount, 0U, 0, eTriangleList);
}