
#include "MultithreadedRendering.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "D3DGUI_imGui.h"
#include "D3DTexture.h"
#include "D3DLighting.h"

#include <mutex>
#include <condition_variable>

Matrix g_LightVP[4];

struct LightParams
{
	Matrix VP;

	Vec4 Position;
	Vec4 Direction;
	Vec4 Color;
	Vec4 Falloff;
};

struct ConstantBufferVS
{
	Matrix World;
	Matrix WorldInverse;
	Matrix VP;
};

struct ConstantBufferPS
{
	Vec4 MirrorPlane;
	Vec4 TintColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);

	Vec4 AmbientColor = Vec4(0.04f * 0.760f, 0.04f * 0.793f, 0.04f * 0.822f, 1.0f);
	LightParams LightParams[4];

	Matrix CalcLightMatrix(uint32_t iLight, float fov, float aspect, float nearPlane, float farPlane)
	{
		const float SceneRadius = 600.0f;

		const Vec4 &LightDir = LightParams[iLight].Direction;
		const Vec4 &LightPos = LightParams[iLight].Position;
		const Vec3 Up = Vec3(0.0f, 1.0f, 0.0f);

		Vec4 lookAt = LightPos + LightDir * SceneRadius;

		Matrix view = Matrix::LookAtLH(Vec3(LightPos.x, LightPos.y, LightPos.z), Vec3(lookAt.x, lookAt.y, lookAt.z), Up);
		Matrix proj = Matrix::PerspectiveFovLH(fov, aspect, nearPlane, farPlane);

		return view * proj;
	}

	ConstantBufferPS()
	{
		const float SceneRadius = 600.0f;
		const Vec4 SceneCenter = Vec4(0.0f, 350.0f, 0.0f, 1.0f);
		const float FOV[4] = { Math::XM_PI / 4.0f , 65.0f * (Math::XM_PI / 180.0f), 65.0f * (Math::XM_PI / 180.0f), 65.0f * (Math::XM_PI / 180.0f) };

		///LightParams[0].Color = Vec4(3.0f * 0.160f, 3.0f * 0.341f, 3.0f * 1.000f, 1.0f);
		LightParams[0].Color = Vec4(1.0f, 0.8f, 1.0f, 1.0f);
		LightParams[0].Direction = Vec4(-0.67f, -0.71f, +0.21f, 0.0);
		LightParams[0].Position = SceneCenter - LightParams[0].Direction * SceneRadius;
		
		LightParams[1].Color = Vec4(0.4f * 0.895f, 0.4f * 0.634f, 0.4f * 0.626f, 1.0f);
		LightParams[1].Direction = Vec4(0.00f, -1.00f, 0.00f, 0.0f);
		LightParams[1].Position = Vec4(0.0f, 400.0f, -250.0f, 1.0f);
		
		LightParams[2].Color = Vec4(0.5f * 0.388f, 0.5f * 0.641f, 0.5f * 0.401f, 1.0f);
		LightParams[2].Direction = Vec4(0.00f, -1.00f, 0.00f, 0.0f);
		LightParams[2].Position = Vec4(0.0f, 400.0f, 0.0f, 1.0f);
		
		LightParams[3].Color = Vec4(0.4f * 1.000f, 0.4f * 0.837f, 0.4f * 0.848f, 1.0f);
		LightParams[3].Direction = Vec4(0.00f, -1.00f, 0.00f, 0.0f);
		LightParams[3].Position = Vec4(0.0f, 400.0f, 250.0f, 1.0f);

		for (uint32_t i = 0U; i < 4U; ++i)
		{
			g_LightVP[i] = CalcLightMatrix(i, FOV[i], 1.0f, 100.0f, 2.0f * SceneRadius);
			LightParams[i].Falloff = Vec4(2.0f * SceneRadius, 100.0f, cosf(FOV[i] / 2.0f), 0.1f);
		}
	}
};

ConstantBufferVS g_CBufferVS;
ConstantBufferPS g_CBufferPS;

void AppMultithreadedRendering::InitShadowResource()
{
	const uint32_t ShadowMapSize = 2048U;

	for (uint32_t i = 0U; i < eNumShadows; ++i)
	{
		D3DTexture2D shadowMap;
		shadowMap.Create(eR32_Typeless,
			ShadowMapSize, ShadowMapSize,
			D3DBuffer::eBindAsShaderResource | D3DBuffer::eBindAsDepthStencil,
			1U);

		m_ShadowSRV[i].CreateAsTexture(D3DView::eTexture2D, shadowMap, eR32_Float, 0U, 1U);

		m_StaticParamsShadows[i].DepthStencilState = m_NoStencil;
		m_StaticParamsShadows[i].StencilRef = 0U;
		m_StaticParamsShadows[i].RasterizerState = D3DStaticState::Solid;
		m_StaticParamsShadows[i].Viewport = { 0.0f, 0.0f, (float)ShadowMapSize, (float)ShadowMapSize };
		m_StaticParamsShadows[i].DepthStencilView.CreateAsTexture(D3DView::eTexture2D, shadowMap, eD32_Float, 0U, 0U);
		m_StaticParamsShadows[i].TintColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		m_StaticParamsShadows[i].MirrorPlane = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}
}

void AppMultithreadedRendering::InitMirrorResource()
{
	m_DepthTestStencilOverwrite.Create(
		true, D3DState::eDepthMaskZero, D3DState::eLessEqual,
		true, 0U, D3DState::eStencilDefaultWriteMask,
		D3DState::eStencilReplace, D3DState::eStencilKeep, D3DState::eStencilReplace, D3DState::eAlways,
		D3DState::eStencilReplace, D3DState::eStencilKeep, D3DState::eStencilReplace, D3DState::eAlways);

	m_DepthOverwriteStencilTest.Create(
		true, D3DState::eDepthMaskAll, D3DState::eAlways,
		true, D3DState::eStencilDefaultReadMask, 0U,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eEqual,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eEqual);

	m_DepthWriteStencilTest.Create(
		true, D3DState::eDepthMaskAll, D3DState::eLessEqual, /// eAlways
		true, D3DState::eStencilDefaultReadMask, 0U,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eEqual,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eEqual);

	m_DepthOverwriteStencilClear.Create(
		true, D3DState::eDepthMaskAll, D3DState::eAlways,
		true, D3DState::eStencilDefaultReadMask, D3DState::eStencilDefaultWriteMask,
		D3DState::eStencilZero, D3DState::eStencilKeep, D3DState::eStencilZero, D3DState::eEqual,
		D3DState::eStencilZero, D3DState::eStencilKeep, D3DState::eStencilZero, D3DState::eEqual);

	/// These values are hard-coded based on the sdkmesh contents, plus some
	/// hand-fiddling, pending a better solution in the pipeline.
	Vec3 MirrorCenters[eNumMirrors];
	MirrorCenters[0] = Vec3(-35.1688f, 89.279683f, -0.7488765f);
	MirrorCenters[1] = Vec3(41.2174f, 89.279683f, -0.7488765f);
	MirrorCenters[2] = Vec3(3.024275f, 89.279683f, -54.344299f);
	MirrorCenters[3] = Vec3(3.024275f, 89.279683f, 52.8466f);

	Vec2 MirrorSize[eNumMirrors];
	MirrorSize[0] = Vec2(104.190895f, 92.19922656f);
	MirrorSize[1] = Vec2(104.190899f, 92.19922656f);
	MirrorSize[2] = Vec2(76.3862f, 92.3427325f);
	MirrorSize[3] = Vec2(76.386196f, 92.34274043f);

	Vec3 MirrorNormals[eNumMirrors];
	MirrorNormals[0] = Vec3(-0.998638464f, -0.052165297f, 0.0f);
	MirrorNormals[1] = Vec3(0.998638407f, -0.052166381f, 3.15017E-08f);
	MirrorNormals[2] = Vec3(0.0f, -0.076278878f, -0.997086522f);
	MirrorNormals[3] = Vec3(-5.22129E-08f, -0.076279957f, 0.99708644f);

	Vec2 MirrorResolution[eNumMirrors];
	MirrorResolution[0].x = MirrorResolution[1].x = MirrorResolution[2].x = MirrorResolution[3].x = 320.0f;
	MirrorResolution[0].y = (MirrorResolution[0].x * MirrorSize[0].y / MirrorSize[0].x);
	MirrorResolution[1].y = (MirrorResolution[1].x * MirrorSize[1].y / MirrorSize[1].x);
	MirrorResolution[2].y = (MirrorResolution[2].x * MirrorSize[2].y / MirrorSize[2].x);
	MirrorResolution[3].y = (MirrorResolution[3].x * MirrorSize[3].y / MirrorSize[3].x);

	Vec3 MirrorCorners[eNumMirrors];
	MirrorCorners[0] = Vec3(-1.0f, -1.0f, 0.0f);
	MirrorCorners[1] = Vec3(1.0f, -1.0f, 0.0f);
	MirrorCorners[2] = Vec3(-1.0f, 1.0f, 0.0f);
	MirrorCorners[3] = Vec3(1.0f, 1.0f, 0.0f);

	Vec3 MirrorPointAt[eNumMirrors];
	const Vec3 Up(0.0f, 1.0f, 0.0f);

	m_VertexBufferMirror.CreateAsVertexBuffer(sizeof(MirrorRect), D3DBuffer::eGpuReadCpuWrite, nullptr);

	for (uint32_t i = 0U; i < eNumMirrors; ++i)
	{
		m_StaticParamsMirrors[i].DepthStencilState = m_DepthWriteStencilTest;
		m_StaticParamsMirrors[i].StencilRef = 0x01U;
		m_StaticParamsMirrors[i].RasterizerState = D3DStaticState::SolidFrontCCW;
		m_StaticParamsMirrors[i].TintColor = Vec4(0.3f, 0.5f, 1.0f, 1.0f);
		m_StaticParamsMirrors[i].MirrorPlane = Math::GetPlaneFromPointNormal(MirrorCenters[i], MirrorNormals[i]);
		m_StaticParamsMirrors[i].Viewport = { 0.0f, 0.0f, (float)m_Width, (float)m_Height };

		MirrorPointAt[i] = MirrorNormals[i] + MirrorCenters[i];
		m_MirrorWorld[i] = Matrix::Transpose(Matrix::LookAtLH(MirrorPointAt[i], MirrorCenters[i], Up));
		m_MirrorWorld[i]._41 = MirrorCenters[i].x;
		m_MirrorWorld[i]._42 = MirrorCenters[i].y;
		m_MirrorWorld[i]._43 = MirrorCenters[i].z;
		m_MirrorWorld[i]._14 = m_MirrorWorld[i]._24 = m_MirrorWorld[i]._34 = 0.0f;
		m_MirrorWorld[i]._44 = 1.0f;

		for (uint32_t j = 0U; j < eNumMirrors; ++j)
		{
			m_MirrorRect[i][j].Position = Vec3(0.5f * MirrorSize[i].x * MirrorCorners[j].x, 0.5f * MirrorSize[i].y * MirrorCorners[j].y, MirrorCorners[j].z);
			m_MirrorRect[i][j].Normal = Vec3(0.0f, 0.0f, 0.0f);
			m_MirrorRect[i][j].Texcoord = Vec2(0.0f, 0.0f);
			m_MirrorRect[i][j].Tangent = Vec3(0.0f, 0.0f, 0.0f);
		}
	}
}

std::condition_variable g_Notifyer;
std::mutex g_Mutex[AppMultithreadedRendering::eNumScenes];
bool g_TaskStart[AppMultithreadedRendering::eNumScenes] = {};
void AppMultithreadedRendering::InitWorkerThreads()
{
	for (uint32_t i = 0U; i < 1U; ++i)
	{
		m_RenderThreads[i].Start(std::thread(&AppMultithreadedRendering::PerSceneRenderTask, this, i));
	}
}

void AppMultithreadedRendering::Initialize()
{
	///m_SquidRoom.Create("SquidRoom\\SquidRoom.sdkmesh", false);
	m_SquidRoom.CreateEx("SquidRoom\\SquidRoom.sdkmesh");

	m_VertexShader.Create("MultithreadedRendering.hlsl", "VSMain");
	m_PixelShader.Create("MultithreadedRendering.hlsl", "PSMain");

	const D3D11_INPUT_ELEMENT_DESC UncompressedLayout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,      0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	const D3D11_INPUT_ELEMENT_DESC CompressedLayout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",    0, DXGI_FORMAT_R10G10B10A2_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R16G16_FLOAT,      0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",   0, DXGI_FORMAT_R10G10B10A2_UNORM, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	m_Layout.Create(m_VertexShader.GetBlob(), CompressedLayout, _countof(CompressedLayout));
	m_LayoutMirror.Create(m_VertexShader.GetBlob(), UncompressedLayout, _countof(UncompressedLayout));
	m_SquidRoom.SetInputLayout(m_Layout);

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite);

	m_Camera->SetViewRadius(400.0f);

	m_NoStencil.Create(
		true, D3DState::eDepthMaskAll, D3DState::eLessEqual,
		false, 0U, 0U,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eNever,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eNever);

	m_StaticParamsDirectly.DepthStencilState = m_NoStencil;
	m_StaticParamsDirectly.RasterizerState = D3DStaticState::Solid;
	m_StaticParamsDirectly.TintColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_StaticParamsDirectly.Viewport = { 0.0f, 0.0f, (float)m_Width, (float)m_Height };

	InitShadowResource();

	InitMirrorResource();

	InitWorkerThreads();
}

void AppMultithreadedRendering::PerSceneRenderTask(uint32_t taskID)
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(g_Mutex[taskID]);
		g_Notifyer.wait(lock, [taskID] { return g_TaskStart[taskID]; });

		if (taskID < eNumShadows)
		{
			DrawShadow(taskID);
		}
		else if (taskID < eNumShadows + eNumMirrors)
		{
			DrawMirror(taskID);
		}
		else
		{
			DrawScene(m_StaticParamsDirectly, m_Camera->GetWorldMatrix(), m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
		}

		g_TaskStart[taskID] = false;

		lock.unlock();
		g_Notifyer.notify_one();
	}
}

void AppMultithreadedRendering::PerChunkRenderTask()
{

}

void AppMultithreadedRendering::SetupScene(const StaticParams &params, const Matrix &world, const Matrix &vp)
{
	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetViewport(params.Viewport, 0U);

	D3DEngine::Instance().SetDepthStencilState(params.DepthStencilState, params.StencilRef);
	D3DEngine::Instance().SetRasterizerState(params.RasterizerState);

	D3DEngine::Instance().SetSamplerState(D3DStaticState::PointClampSampler, 1U, D3DShader::ePixelShader);

	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);
	g_CBufferVS.World = Matrix::Transpose(world);
	g_CBufferVS.WorldInverse = Matrix::InverseTranspose(world);
	g_CBufferVS.VP = Matrix::Transpose(vp);
	m_CBufferVS.Update(&g_CBufferVS, sizeof(ConstantBufferVS));

	bool bShadow = params.DepthStencilView.IsValid();
	if (bShadow)
	{
		D3DPixelShader EmptyPS;
		D3DEngine::Instance().SetPixelShader(EmptyPS);

		D3DRenderTargetView EmptyRTV;
		D3DEngine::Instance().SetRenderTargetView(EmptyRTV, 0U);
		D3DEngine::Instance().SetDepthStencilView(params.DepthStencilView);

		D3DShaderResourceView EmptySRV;
		for (uint32_t i = 0U; i < eNumShadows; ++i)
		{
			D3DEngine::Instance().SetShaderResourceView(EmptySRV, 2, D3DShader::ePixelShader);
		}

		D3DDepthStencilView dsv = params.DepthStencilView;
		D3DEngine::Instance().ClearDepthStencilView(dsv, D3DDepthStencilView::eDepthStencil, 1.0f, 0U);
	}
	else
	{
		D3DEngine::Instance().SetPixelShader(m_PixelShader);

		/// Set Constant Buffer PS
		D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);
		for (uint32_t i = 0U; i < 4U; ++i)
		{
			g_CBufferPS.LightParams[i].VP = Matrix::Transpose(g_LightVP[i]);
		}
		g_CBufferPS.MirrorPlane = params.MirrorPlane;
		m_CBufferPS.Update(&g_CBufferPS, sizeof(ConstantBufferPS));

		for (uint32_t i = 0U; i < eNumShadows; ++i)
		{
			D3DEngine::Instance().SetShaderResourceView(m_ShadowSRV[i], 2, D3DShader::ePixelShader);
		}
	}
}

void AppMultithreadedRendering::SetupMirror(uint32_t iMirror)
{
	D3DPixelShader EmptyPixelShader;

	D3DEngine::Instance().SetRasterizerState(D3DStaticState::SolidFrontCCW);

	D3DEngine::Instance().SetInputLayout(m_LayoutMirror);
	D3DEngine::Instance().SetVertexBuffer(m_VertexBufferMirror, sizeof(MirrorVertex), 0U, 0U);
	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(EmptyPixelShader);
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	g_CBufferVS.World = Matrix::Transpose(m_MirrorWorld[iMirror]);
	g_CBufferVS.WorldInverse = Matrix::InverseTranspose(m_MirrorWorld[iMirror]);
	g_CBufferVS.VP = Matrix::Transpose(m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	m_CBufferVS.Update(&g_CBufferVS, sizeof(ConstantBufferVS));
}

void AppMultithreadedRendering::DrawScene(const StaticParams &params, const Matrix &world, const Matrix &vp)
{
	SetupScene(params, world, vp);

	m_SquidRoom.DrawEx(false);
}

void AppMultithreadedRendering::DrawShadow(uint32_t iShadow)
{
	DrawScene(m_StaticParamsShadows[iShadow], m_Camera->GetWorldMatrix(), g_LightVP[iShadow]);
}

void AppMultithreadedRendering::DrawMirror(uint32_t iMirror)
{
	/// Test for back-facing mirror (from whichever pov we are using)
	const Vec4 &plane = m_StaticParamsMirrors[iMirror].MirrorPlane;
	if (Math::PlaneDotCoord(plane, m_Camera->GetEyePos()) < 0.0f)
	{
		return;
	}

	D3DEngine::Instance().SetViewport(m_StaticParamsMirrors[iMirror].Viewport, 0U);

	m_VertexBufferMirror.Update(m_MirrorRect[iMirror], sizeof(MirrorRect));

	/// Draw the mirror quad into the stencil buffer, setting the stencil ref value
	SetupMirror(iMirror);
	D3DEngine::Instance().SetDepthStencilState(m_DepthTestStencilOverwrite, 0x01U);
	D3DEngine::Instance().Draw(4U, 0U, eTriangleStrip);

	/// Set up the transform matrices to alway output depth equal to the far plane (z = w of output)
	Matrix vp = m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix();
	g_CBufferVS.VP._31 = vp._14;
	g_CBufferVS.VP._32 = vp._24;
	g_CBufferVS.VP._33 = vp._34;
	g_CBufferVS.VP._34 = vp._44;
	m_CBufferVS.Update(&g_CBufferVS, sizeof(ConstantBufferVS));
	D3DEngine::Instance().SetDepthStencilState(m_DepthOverwriteStencilTest, 0x01U);
	D3DEngine::Instance().Draw(4U, 0U, eTriangleStrip);

	/// Draw the mirrored world into the stencilled area
	Matrix reflect = Matrix::Reflect(plane);
	DrawScene(m_StaticParamsMirrors[iMirror], m_Camera->GetWorldMatrix(), reflect * vp);

	SetupMirror(iMirror);
	D3DEngine::Instance().SetDepthStencilState(m_DepthOverwriteStencilClear, 0x01U);
	D3DEngine::Instance().Draw(4U, 0U, eTriangleStrip);
}

void AppMultithreadedRendering::RenderScene()
{
	if (eST == m_RenderingMode)
	{
		for (uint32_t i = 0U; i < eNumShadows; ++i)
		{
			DrawShadow(i);
		}

		D3DEngine::Instance().ResetDefaultRenderSurfaces();
		D3DEngine::Instance().SetViewport(D3DViewport(0.0f, 0.0f, (float)m_Width, (float)m_Height));
		D3DEngine::Instance().ForceCommitState();
		for (uint32_t i = 0U; i < eNumMirrors; ++i)
		{
			DrawMirror(i);
		}

		DrawScene(m_StaticParamsDirectly, m_Camera->GetWorldMatrix(), m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	}
	else if (eMT_PerScene == m_RenderingMode)
	{
		for (uint32_t i = 0U; i < 1U; ++i)
		{
			std::lock_guard<std::mutex> lock(g_Mutex[i]);
			g_TaskStart[i] = true;
		}
		g_Notifyer.notify_all();

		for (uint32_t i = 0U; i < 1U; ++i)
		{
			std::unique_lock<std::mutex> lock(g_Mutex[i]);
			g_Notifyer.wait(lock, [i] { return !g_TaskStart[i]; });
			lock.unlock();
		}
	}
	else if (eMT_PerChunk == m_RenderingMode)
	{

	}

	ImGui::Text("%.2f FPS", m_FPS);
	ImGui::Combo("RenderingMode", &m_RenderingMode, "SingleThread\0MultithreadedPerScene\0MultithreadedPerChunk");
}

void AppMultithreadedRendering::Update(float deltaTime, float totalTime)
{
	Vec4 Down(0.0f, -1.0f, 0.0f, 0.0f);
	totalTime += deltaTime;

	float fCycle1X = 0.0f;
	float fCycle1Z = 0.20f * sinf(2.0f * (totalTime + 0.0f * Math::XM_PI));
	g_CBufferPS.LightParams[1].Direction = Down + Vec4(fCycle1X, 0.0f, fCycle1Z, 0.0f);

	float fCycle2X = 0.10f * cosf(1.6f * (totalTime + 0.3f * Math::XM_PI));
	float fCycle2Z = 0.10f * sinf(1.6f * (totalTime + 0.0f * Math::XM_PI));
	g_CBufferPS.LightParams[2].Direction = Down + Vec4(fCycle2X, 0.0f, fCycle2Z, 0.0f);

	float fCycle3X = 0.30f * cosf(2.4f * (totalTime + 0.3f * Math::XM_PI));
	float fCycle3Z = 0.0f;
	g_CBufferPS.LightParams[3].Direction = Down + Vec4(fCycle3X, 0.0f, fCycle3Z, 0.0f);
}

void AppMultithreadedRendering::ResizeWindow(uint32_t width, uint32_t height)
{
	Base::ResizeWindow(width, height);

	m_StaticParamsDirectly.Viewport.Width = (float)width;
	m_StaticParamsDirectly.Viewport.Height = (float)height;

	for (uint32_t i = 0U; i < eNumMirrors; ++i)
	{
		m_StaticParamsMirrors[i].Viewport.Width = (float)width;
		m_StaticParamsMirrors[i].Viewport.Height = (float)height;
	}
}
