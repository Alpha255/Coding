#include "Cubemap.h"
#include "Camera.h"
#include "D3DLighting.h"
#include "D3DEngine.h"
#include "D3DTexture.h"

struct ConstantBufferVS
{
	Matrix World;
	Matrix WorldInverse;
	Matrix WVP;
};

struct ConstantBufferPS
{
	Vec4 EyePos;

	DirectionalLight DirLights;
	Material Mat;
};

void AppCubemap::InitCubemapCameras(float cx, float cy, float cz)
{
	Vec3 worldCenter = Vec3(cx, cy, cz);

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
		m_CubemapCamera[i].SetViewParams(worldCenter, lookAts[i], upDirs[i]);
		m_CubemapCamera[i].SetProjParams(Math::XM_PIDIV2, 1.0f, 0.1f, 1000.0f);
	}
}

void AppCubemap::Initialize()
{
	m_CubeMesh.CreateAsCube(1.0f);
	m_DiffuseTex.Create("stone.dds");

	m_VertexShader.Create("Cubemap.hlsl", "VSMain");
	m_PixelShader.Create("Cubemap.hlsl", "PSMain");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite, nullptr);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite, nullptr);

	InitCubemapCameras(0.0f, 2.0f, 0.0f);

	const uint32_t CubemapSize = 1024U;
	D3DTexture2D cubemapTex;
	cubemapTex.Create(eRGBA8_UNorm, CubemapSize, CubemapSize, D3DBuffer::eBindAsRenderTarget | D3DBuffer::eBindAsShaderResource, 
		0U, 6U, 0U, D3DBuffer::eMiscGenerateMips | D3DBuffer::eMiscCubemap);
	m_CubeMap.CreateAsTexture(D3DView::eTextureCube, cubemapTex, eRGBA8_UNorm, 0U, 1U);

	for (uint32_t i = 0U; i < 6U; ++i)
	{
		m_CubemapRTV[i].CreateAsTextureArray(D3DView::eTexture2DArray, cubemapTex, eRGBA8_UNorm, 0U, i, 1U);
	}

	m_CubemapDSV.Create(eD32_Float, CubemapSize, CubemapSize);

	m_CubemapViewport = { 0.0f, 0.0f, (float)CubemapSize, (float)CubemapSize, 0.0f, 1.0f };

	m_Camera->Translation(0.0f, 2.0f, -15.0f);
}

void AppCubemap::DrawScene(const Camera &cam)
{
	//s_Resource.Skull.Draw(cam);

	//ConstantsBufVS cbVS;
	//memset(&cbVS, 0, sizeof(ConstantsBufVS));

	//Vec4 eyePos = cam.GetEyePos();
	//s_CBPS.EyePos = Vec3(eyePos.x, eyePos.y, eyePos.z);

	//Matrix world, view, proj, wvp;
	//view = cam.GetViewMatrix();
	//proj = cam.GetProjMatrix();

	//g_Renderer->SetInputLayout(s_Resource.Layout);
	//
	//g_Renderer->SetVertexShader(s_Resource.VertexShader);
	//g_Renderer->SetPixelShader(s_Resource.PixelShader);

	//g_Renderer->SetConstantBuffer(s_Resource.ConstantsBufVS, 0U, D3DGraphic::eVertexShader);
	//g_Renderer->SetConstantBuffer(s_Resource.ConstantsBufPS, 0U, D3DGraphic::ePixelShader);

	//g_Renderer->SetVertexBuffer(s_Resource.GeometriesVBuf, sizeof(Vertex), 0U);
	//g_Renderer->SetIndexBuffer(s_Resource.GeometriesIBuf, DXGI_FORMAT_R32_UINT, 0U);  

	//g_Renderer->SetSamplerStates(s_Resource.Sampler, 0U, D3DGraphic::ePixelShader);
	//
	//{
	//	/// Draw Grid
	//	world = s_Geometries.WorldGrid;
	//	wvp = world * view * proj;
	//	cbVS.World = world.Transpose();
	//	cbVS.WorldInverse = cbVS.World.Inverse();
	//	cbVS.WVP = wvp.Transpose();
	//	cbVS.TexTransform = Matrix::Scaling(6.0f, 8.0f, 1.0f)/*.Transpose()*/;
	//	g_Renderer->UpdateBuffer(s_Resource.ConstantsBufVS, &cbVS, sizeof(ConstantsBufVS));

	//	memcpy(&s_CBPS.Mat, &s_Geometries.MatGrid, sizeof(Lighting::Material));
	//	g_Renderer->UpdateBuffer(s_Resource.ConstantsBufPS, &s_CBPS, sizeof(ConstantsBufPS));

	//	g_Renderer->SetShaderResource(s_Resource.FloorTex, 0U, D3DGraphic::ePixelShader);

	//	g_Renderer->DrawIndexed(s_Geometries.GridIndexCount, s_Geometries.GridIndexOffset, s_Geometries.GridVertexOffset);
	//}

	//{
	//	/// Draw Box
	//	world = s_Geometries.WorldBox;
	//	wvp = world * view * proj;
	//	cbVS.World = world.Transpose();
	//	cbVS.WorldInverse = cbVS.World.Inverse();
	//	cbVS.WVP = wvp.Transpose();
	//	cbVS.TexTransform.Identity();
	//	g_Renderer->UpdateBuffer(s_Resource.ConstantsBufVS, &cbVS, sizeof(ConstantsBufVS));

	//	memcpy(&s_CBPS.Mat, &s_Geometries.MatBox, sizeof(Lighting::Material));
	//	g_Renderer->UpdateBuffer(s_Resource.ConstantsBufPS, &s_CBPS, sizeof(ConstantsBufPS));

	//	g_Renderer->SetShaderResource(s_Resource.StoneTex, 0U, D3DGraphic::ePixelShader);

	//	g_Renderer->DrawIndexed(s_Geometries.BoxIndexCount, s_Geometries.BoxIndexOffset, s_Geometries.BoxVertexOffset);
	//}

	//{
	//	/// Draw Spheres
	//	for (uint32_t i = 0U; i < 10U; ++i)
	//	{
	//		world = s_Geometries.WorldSphere[i];
	//		wvp = world * view * proj;
	//		cbVS.World = world.Transpose();
	//		cbVS.WorldInverse = cbVS.World.Inverse();
	//		cbVS.WVP = wvp.Transpose();
	//		cbVS.TexTransform.Identity();
	//		g_Renderer->UpdateBuffer(s_Resource.ConstantsBufVS, &cbVS, sizeof(ConstantsBufVS));

	//		memcpy(&s_CBPS.Mat, &s_Geometries.MatSphere, sizeof(Lighting::Material));
	//		g_Renderer->UpdateBuffer(s_Resource.ConstantsBufPS, &s_CBPS, sizeof(ConstantsBufPS));

	//		g_Renderer->SetShaderResource(s_Resource.StoneTex, 0U, D3DGraphic::ePixelShader);

	//		g_Renderer->DrawIndexed(s_Geometries.SphereIndexCount, s_Geometries.SphereIndexOffset, s_Geometries.SphereVertexOffset);
	//	}
	//}

	//if (bDrawCenterSphere)
	//{
	//	world = s_Geometries.WorldCenterSphere;
	//	wvp = world * view * proj;
	//	cbVS.World = world.Transpose();
	//	cbVS.WorldInverse = cbVS.World.Inverse();
	//	cbVS.WVP = wvp.Transpose();
	//	cbVS.TexTransform.Identity();
	//	g_Renderer->UpdateBuffer(s_Resource.ConstantsBufVS, &cbVS, sizeof(ConstantsBufVS));

	//	memcpy(&s_CBPS.Mat, &s_Geometries.MatCenterSphere, sizeof(Lighting::Material));
	//	g_Renderer->UpdateBuffer(s_Resource.ConstantsBufPS, &s_CBPS, sizeof(ConstantsBufPS));

	//	g_Renderer->SetShaderResource(s_Resource.StoneTex, 0U, D3DGraphic::ePixelShader);

	//	g_Renderer->SetShaderResource(s_Resource.DynamicCubeMap, 1U, D3DGraphic::ePixelShader);

	//	g_Renderer->DrawIndexed(s_Geometries.SphereIndexCount, s_Geometries.SphereIndexOffset, s_Geometries.SphereVertexOffset);

	//	Ref<ID3D11ShaderResourceView> nullCubemap;
	//	g_Renderer->SetShaderResource(nullCubemap, 1U, D3DGraphic::ePixelShader);
	//}

	//s_Resource.Sky.Draw(cam);
}

void AppCubemap::DrawDynamicCubemap()
{
	//g_Renderer->SetViewports(&s_Resource.DynamicCubemapVP);

	//for (uint32_t i = 0U; i < 6U; ++i)
	//{
	//	g_Renderer->ClearRenderTarget(s_Resource.DynamicCubemapRTVs[i], reinterpret_cast<const float*>(&Color::Silver));
	//	g_Renderer->ClearDepthStencil(s_Resource.DynamicCubemapDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);

	//	g_Renderer->SetRenderTarget(s_Resource.DynamicCubemapRTVs[i]);
	//	g_Renderer->SetDepthStencil(s_Resource.DynamicCubemapDSV);

	//	DrawScene(s_CubemapCamera[i]);
	//}
}

void AppCubemap::RenderScene()
{
	//DrawDynamicCubemap();

	//g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget());
	//g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);

	//g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	//g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	//g_Renderer->SetViewports(&s_Resource.VP);

	//DrawScene(*m_Camera, true);

	//ImGui::Checkbox("Reflection", &m_bEnableReflection);
	//s_CBPS.EnableReflection = m_bEnableReflection ? 1U : 0U;
}