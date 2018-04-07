#include "DepthStencilTest.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "D3DLighting.h"
#include "D3DGUI_imGui.h"

struct ConstantBufferVS
{
	Matrix World;
	Matrix WorldInverse;
	Matrix WVP;
};

struct ConstantBufferPS
{
	Vec4 EyePos;
	Material Mat;

	uint32_t UseFog = 0U;
	uint32_t UseAlphaClip = 0U;
	float FogStart = 2.0f;
	float FogRange = 40.0f;

	Vec4 FogClr = Color::Black;
};

ConstantBufferVS g_CBufferVS;
ConstantBufferPS g_CBufferPS;

static Matrix s_SkullWorld;
static Matrix s_SkullWorldReflect;
static DirectionalLight s_DirLights[3];
static DirectionalLight s_DirLightsReflect[3];
Material g_MatRoom;
Material g_MatMirror;

//ApplicationStenciling::ApplicationStenciling()
//{
//
//	s_DirLights[0].Ambient = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
//	s_DirLights[0].Diffuse = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
//	s_DirLights[0].Specular = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
//	s_DirLights[0].Direction = Vec4(0.57735f, -0.57735f, 0.57735f, 0.0f);
//
//	s_DirLights[1].Ambient = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
//	s_DirLights[1].Diffuse = Vec4(0.20f, 0.20f, 0.20f, 1.0f);
//	s_DirLights[1].Specular = Vec4(0.25f, 0.25f, 0.25f, 1.0f);
//	s_DirLights[1].Direction = Vec4(-0.57735f, -0.57735f, 0.57735f, 0.0f);
//
//	s_DirLights[2].Ambient = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
//	s_DirLights[2].Diffuse = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
//	s_DirLights[2].Specular = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
//	s_DirLights[2].Direction = Vec4(0.0f, -0.707f, -0.707f, 0.0f);
//
//	memcpy(s_CBPS.DirLights, s_DirLights, sizeof(Lighting::DirectionalLight) * 3U);
//	memcpy(s_DirLightsReflect, s_DirLights, sizeof(Lighting::DirectionalLight) * 3U);
//
//	Matrix reflect = Matrix::Reflect(0.0f, 0.0f, 1.0f);
//	for (uint32_t i = 0U; i < 3U; ++i)
//	{
//		s_DirLightsReflect[i].Direction.TransformNormal(reflect);
//	}
//}

void AppDepthStencilTest::Initialize()
{
	///   |--------------|
	///   |              |
	///   |----|----|----|
	///   |Wall|Mirr|Wall|
	///   |    | or |    |
	///   /--------------/
	///  /   Floor      /
	/// /--------------/
	m_RoomMesh.Create("Room.obj");
	m_SphereMesh.CreateAsGeoSphere(1.0f, 3U);

	m_FloorTex.Create("checkboard.dds");
	m_WallTex.Create("brick01.dds");
	m_MirrorTex.Create("ice.dds");
	m_SphereTex.Create("wood.dds");

	m_VertexShader.Create("DepthStencilTest.hlsl", "VSMain");
	m_PixelShader.Create("DepthStencilTest.hlsl", "PSMain");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite, nullptr);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite, nullptr);

	/// src(sr, sg, sb, sa)  dst(dr, dg, db, da)
	/// output(r, g, b) = 1 * (sr, sg, sb) + 0 * (dr, dg, db)
	/// outout(a) = 1 * sa + 0 * da
	m_NoColorWrite.Create(false, false, 0U, false,
		D3DState::eOne, D3DState::eZero, D3DState::eAdd,
		D3DState::eOne, D3DState::eZero, D3DState::eAdd,
		D3DState::eColorNone);

	/// src(sr, sg, sb, sa)  dst(dr, dg, db, da)
	/// output(r, g, b) = sa * (sr, sg, sb) + (1 - sa) * (dr, dg, db)
	/// output(a) = 1 * sa + 0 * da
	m_AlphaBlend.Create(false, false, 0U, true,
		D3DState::eSrcAlpha, D3DState::eInvSrcAlpha, D3DState::eAdd,
		D3DState::eOne, D3DState::eZero, D3DState::eAdd,
		D3DState::eColorAll);

	/// (StencilRef & StencilReadMask) -CompareFunc- (Value & StencilReadMask)
	m_MarkMirror.Create(true, D3DState::eDepthMaskZero, D3DState::eLess,
		true, D3DState::eStencilDefaultReadMask, D3DState::eStencilDefaultWriteMask,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilReplace, D3DState::eAlways,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilReplace, D3DState::eAlways); /// Only use front face

	m_Reflection.Create(true, D3DState::eDepthMaskAll, D3DState::eLess,
		true, D3DState::eStencilDefaultReadMask, D3DState::eStencilDefaultWriteMask,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eEqual,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eEqual);   /// Only use front face

	g_MatRoom.Ambient = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
	g_MatRoom.Diffuse = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	g_MatRoom.Specular = Vec4(0.4f, 0.4f, 0.4f, 16.0f);

	g_MatMirror.Ambient = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
	g_MatMirror.Diffuse = Vec4(1.0f, 1.0f, 1.0f, 0.5f);
	g_MatMirror.Specular = Vec4(0.4f, 0.4f, 0.4f, 16.0f);
}

void AppDepthStencilTest::UpdateConstantBufferVS(const Matrix &world)
{
	g_CBufferVS.World = Matrix::Transpose(world);
	g_CBufferVS.WorldInverse = Matrix::Inverse(world);
	g_CBufferVS.WVP = Matrix::Transpose(world * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	m_CBufferVS.Update(&g_CBufferVS, sizeof(ConstantBufferVS));
}

void AppDepthStencilTest::DrawFloorAndWalls(const Matrix &world)
{
	UpdateConstantBufferVS(world);

	g_CBufferPS.EyePos = m_Camera->GetEyePos();
	g_CBufferPS.Mat = g_MatRoom;
	m_CBufferPS.Update(&g_CBufferPS, sizeof(ConstantBufferPS));

	D3DEngine::Instance().SetVertexBuffer(m_RoomMesh.VertexBuffer, sizeof(Geometry::Vertex), 0U);
	D3DEngine::Instance().SetIndexBuffer(m_RoomMesh.IndexBuffer, eR32_UInt, 0U);
	D3DEngine::Instance().SetShaderResourceView(m_FloorTex, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().Draw(6U, 0U, eTriangleList);
	D3DEngine::Instance().SetShaderResourceView(m_WallTex, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().Draw(18U, 6U, eTriangleList);
}

void AppDepthStencilTest::DrawSphere(const Matrix &world)
{
	UpdateConstantBufferVS(world);

	D3DEngine::Instance().SetVertexBuffer(m_SphereMesh.VertexBuffer, sizeof(Geometry::Vertex), 0U);
	D3DEngine::Instance().SetIndexBuffer(m_SphereMesh.IndexBuffer, eR32_UInt, 0U);
	D3DEngine::Instance().SetShaderResourceView(m_SphereTex, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().DrawIndexed(m_SphereMesh.IndexCount, 0U, 0, eTriangleList);
}

void AppDepthStencilTest::DrawMirror(const Matrix &world)
{
	UpdateConstantBufferVS(world);

	g_CBufferPS.Mat = g_MatMirror;
	m_CBufferPS.Update(&g_CBufferPS, sizeof(ConstantBufferPS));

	D3DEngine::Instance().SetVertexBuffer(m_RoomMesh.VertexBuffer, sizeof(Geometry::Vertex), 0U);
	D3DEngine::Instance().SetIndexBuffer(m_RoomMesh.IndexBuffer, eR32_UInt, 0U);
	D3DEngine::Instance().SetShaderResourceView(m_MirrorTex, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().Draw(6U, 24U, eTriangleList);
}

void AppDepthStencilTest::RenderScene()
{
	D3DEngine::Instance().SetInputLayout(m_RoomMesh.VertexLayout);
	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);

	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);

	/// Draw floor and walls
	Matrix worldFloorWall = m_Camera->GetWorldMatrix();
	DrawFloorAndWalls(worldFloorWall);

	/// Draw sphere
	Matrix translation = Matrix::Translation(m_SphereTrans.x, 2.0f + m_SphereTrans.y, -2.0f);
	Matrix worldSphere = m_Camera->GetWorldMatrix() * translation;
	DrawSphere(worldSphere);

	/// Draw mirror to stencil buffer, disable depth write, and set stencil value to 1
	/// Do not write mirror depth to depth buffer at this point, otherwise it will occlude the reflection.
	D3DEngine::Instance().SetBlendState(m_NoColorWrite);
	D3DEngine::Instance().SetDepthStencilState(m_MarkMirror, 0xffffffff);
	DrawMirror(worldFloorWall);
	D3DEngine::Instance().SetBlendState(D3DStaticState::NoneBlendState);

	/// Draw sphere reflection, remember reflect light also
	/// Enable depth write, only when stencil value equal to ref value can pass the test
	Matrix reflection = Matrix::Reflect(0.0f, 0.0f, 1.0f);
	worldSphere = worldSphere * reflection;
	D3DEngine::Instance().SetRasterizerState(D3DStaticState::SolidFrontCCW);
	D3DEngine::Instance().SetDepthStencilState(m_Reflection, 0xffffffff);
	DrawSphere(worldSphere);
	D3DEngine::Instance().SetDepthStencilState(D3DStaticState::NoneDepthStencilState, 0U);
	D3DEngine::Instance().SetRasterizerState(D3DStaticState::Solid);

	/// Draw the mirror to the back buffer as usual but with transparency blending so the reflection shows through.
	D3DEngine::Instance().SetBlendState(m_AlphaBlend);
	DrawMirror(worldFloorWall);
	D3DEngine::Instance().SetBlendState(D3DStaticState::NoneBlendState);
}

void AppDepthStencilTest::Update(float elapsedTime, float /*totalTime*/)
{
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_SphereTrans.x -= 1.0f * elapsedTime;
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_SphereTrans.x += 1.0f * elapsedTime;
	}
	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_SphereTrans.y += 1.0f * elapsedTime;
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_SphereTrans.y -= 1.0f * elapsedTime;
	}
}