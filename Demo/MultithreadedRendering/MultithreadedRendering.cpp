
#include "MultithreadedRendering.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "D3DGUI_imGui.h"
#include "D3DTexture.h"
#include "D3DLighting.h"

#include <thread>

struct LightParams
{
	Matrix WVP;

	Vec4 Position;
	Vec4 Direction;
	Vec4 Color;
	Vec4 Falloff;

	float FOV;
	float Aspect;
	float NearPlane;
	float FarPlane;

	float FalloffDistEnd;
	float FalloffDistRange;
	float FalloffCosAngleEnd;
	float FalloffCosAngleRange;
};

struct ConstantBufferVS
{
	Matrix WVP;
	Matrix World;
	Matrix WorldInverse;
};

struct ConstantBufferPS
{
	Vec4 EyePos;

	PointLight Lights[4];

	LightParams LightParams[4];

	ConstantBufferPS()
	{
		const float SceneRadius = 600.0f;
		const Vec4 SceneCenter = Vec4(0.0f, 350.0f, 0.0f, 1.0f);

		LightParams[0].Color = Vec4(3.0f * 0.160f, 3.0f * 0.341f, 3.0f * 1.000f, 1.0f);
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

		LightParams[0].FOV = Math::XM_PI / 4.0f;
		LightParams[1].FOV = LightParams[2].FOV = LightParams[3].FOV = 65.0f * (Math::XM_PI / 180.0f);

		for (uint32_t i = 0U; i < 4U; ++i)
		{
			LightParams[i].Aspect = 1.0f;
			LightParams[i].NearPlane = 100.0f;
			LightParams[i].FarPlane = 2.0f * SceneRadius;

			LightParams[i].FalloffDistEnd = LightParams[i].FarPlane;
			LightParams[i].FalloffDistRange = 100.0f;

			LightParams[i].FalloffCosAngleEnd = cosf(LightParams[i].FOV / 2.0f);
			LightParams[i].FalloffCosAngleRange = 0.1f;
		}

		Lights[0].Ambient = Vec4(0.04f * 0.760f, 0.04f * 0.793f, 0.04f * 0.822f, 1.000);
		Lights[0].Diffuse = Vec4(3.0f * 0.160f, 3.0f * 0.341f, 3.0f * 1.000f, 1.000f);
		Lights[0].Position = Vec3(0.0f, 350.0f, 0.0f) - Vec3(-0.67f, -0.71f, +0.21f) * SceneRadius;

		Lights[1].Diffuse = Vec4(0.4f * 0.895f, 0.4f * 0.634f, 0.4f * 0.626f, 1.0f);
		Lights[1].Position = Vec3(0.0f, 400.0f, -250.0f);

		Lights[2].Diffuse = Vec4(0.5f * 0.388f, 0.5f * 0.641f, 0.5f * 0.401f, 1.0f);
		Lights[2].Position = Vec3(0.0f, 400.0f, 0.0f);

		Lights[3].Diffuse = Vec4(0.4f * 1.000f, 0.4f * 0.837f, 0.4f * 0.848f, 1.0f);
		Lights[3].Position = Vec3(0.0f, 400.0f, 250.0f);

		Lights[0].Range = 
		Lights[1].Range = 
		Lights[2].Range = 
		Lights[3].Range = 3000.0f;
	}

	Matrix CalcLightMatrix(uint32_t iLight)
	{
		const float SceneRadius = 600.0f;

		const Vec4 &LightDir = LightParams[iLight].Direction;
		const Vec4 &LightPos = LightParams[iLight].Position;
		const Vec4 Up = Vec4(0.0f, 1.0f, 0.0f, 0.0f);

		Vec4 lookAt = LightPos + LightDir * SceneRadius;

		Matrix view = *(Matrix *)&Math::XMMatrixLookAtLH(Math::XMLoadFloat4(&LightPos), 
			Math::XMLoadFloat4(&lookAt), Math::XMLoadFloat4(&Up));
		Matrix proj = *(Matrix *)&Math::XMMatrixPerspectiveFovLH(LightParams[iLight].FOV, 
			LightParams[iLight].Aspect, LightParams[iLight].NearPlane, LightParams[iLight].FarPlane);

		return view * proj;
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
		m_StaticParamsShadows[i].RasterizerState = D3DStaticState::SolidFrontFaceCulling;
		m_StaticParamsShadows[i].Viewport = { 0.0f, 0.0f, (float)ShadowMapSize, (float)ShadowMapSize };
		m_StaticParamsShadows[i].DepthStencilView.CreateAsTexture(D3DView::eTexture2D, shadowMap, eD32_Float, 0U, 0U);
		m_StaticParamsShadows[i].TintColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

void AppMultithreadedRendering::InitMirrorResource()
{
	m_DepthTestStencilOverwrite.Create(
		true, D3DState::eDepthMaskZero, D3DState::eLessEqual,
		true, 0U, 0x01,
		D3DState::eStencilReplace, D3DState::eStencilKeep, D3DState::eStencilReplace, D3DState::eAlways,
		D3DState::eStencilReplace, D3DState::eStencilKeep, D3DState::eStencilReplace, D3DState::eAlways);

	m_DepthOverwriteStencilTest.Create(
		true, D3DState::eDepthMaskAll, D3DState::eAlways,
		true, 0x01, 0U,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eEqual,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eEqual);

	m_DepthWriteStencilTest.Create(
		true, D3DState::eDepthMaskAll, D3DState::eEqual,
		true, 0x01, 0U,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eEqual,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eEqual);

	m_DepthOverwriteStencilClear.Create(
		true, D3DState::eDepthMaskAll, D3DState::eAlways,
		true, 0x01, 0x01,
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

	m_VertexBufferMirror.CreateAsVertexBuffer(sizeof(MirrorRect), D3DBuffer::eGpuReadCpuWrite, nullptr);

	for (uint32_t i = 0U; i < eNumMirrors; ++i)
	{
		m_StaticParamsMirrors[i].DepthStencilState = m_DepthWriteStencilTest;
		m_StaticParamsMirrors[i].StencilRef = 0x01;
		m_StaticParamsMirrors[i].RasterizerState = D3DStaticState::Solid;
		m_StaticParamsMirrors[i].TintColor = Vec4(0.3f, 0.5f, 1.0f, 1.0f);
		m_StaticParamsMirrors[i].MirrorPlane = Math::GetPlaneFromPointNormal(MirrorCenters[i], MirrorNormals[i]);

		for (uint32_t j = 0U; j < eNumMirrors; ++j)
		{
			m_MirrorRect[i][j].Position = Vec3(0.5f * MirrorSize[i].x * MirrorCorners[j].x, 0.5f * MirrorSize[i].y * MirrorCorners[j].y, MirrorCorners[j].z);
			m_MirrorRect[i][j].Normal = Vec3(0.0f, 0.0f, 0.0f);
			m_MirrorRect[i][j].Texcoord = Vec2(0.0f, 0.0f);
			m_MirrorRect[i][j].Tangent = Vec3(0.0f, 0.0f, 0.0f);
		}
	}
}

void AppMultithreadedRendering::InitWorkerThreads()
{
	for (uint32_t i = 0U; i < eNumScenes; ++i)
	{
		std::thread renderTask(&AppMultithreadedRendering::PerSceneRenderTask, this, i);

		renderTask.join();
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
	m_StaticParamsDirectly.RasterizerState = D3DStaticState::SolidFrontFaceCulling;
	m_StaticParamsDirectly.TintColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);

	InitShadowResource();

	InitMirrorResource();
}

void AppMultithreadedRendering::PerSceneRenderTask(uint32_t taskID)
{
	if (false)
	{
		m_SceneDefContexts[taskID]->ClearState();
	}

	D3DEngine::Instance().SetContext(m_SceneDefContexts[taskID]);

	/// RenderShadow

	/// RenderMirror

	/// RenderSceneDirect

	ID3D11CommandList *pCmdList = nullptr;
	m_SceneDefContexts[taskID]->FinishCommandList(true, &pCmdList);
	m_SceneCmdList[taskID].MakeObject(pCmdList);
}

void AppMultithreadedRendering::PerChunkRenderTask()
{

}

void AppMultithreadedRendering::SetupScene(const D3DContext &ctxInUse, const StaticParams &params)
{
	D3DEngine::Instance().SetContext(ctxInUse);

	bool bShadow = params.DepthStencilView.IsValid();

	if (bShadow)
	{
		D3DShaderResourceView EmptySRV;
		D3DEngine::Instance().SetShaderResourceView(EmptySRV, 0U, D3DShader::ePixelShader);
		D3DEngine::Instance().SetShaderResourceView(EmptySRV, 1U, D3DShader::ePixelShader);

		D3DEngine::Instance().SetViewport(params.Viewport, 0U);

		D3DRenderTargetView EmptyRTV;
		D3DEngine::Instance().SetRenderTargetView(EmptyRTV, 0U);
		D3DEngine::Instance().SetDepthStencilView(params.DepthStencilView);
	}
	else
	{
		for (uint32_t i = 0U; i < eNumShadows; ++i)
		{
			D3DEngine::Instance().SetShaderResourceView(m_ShadowSRV[i], i, D3DShader::ePixelShader);
		}
	}

	D3DEngine::Instance().SetDepthStencilState(params.DepthStencilState, params.StencilRef);
	D3DEngine::Instance().SetRasterizerState(params.RasterizerState);

	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	///D3DEngine::Instance().SetInputLayout(m_Layout);

	/// Set Constant Buffer VS

	if (bShadow)
	{
		D3DPixelShader EmptyPS;
		D3DEngine::Instance().SetPixelShader(EmptyPS);
	}
	else
	{
		D3DEngine::Instance().SetPixelShader(m_PixelShader);

		D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);

		/// Set Constant Buffer PS
	}
}

void AppMultithreadedRendering::DrawScene()
{

}

void AppMultithreadedRendering::DrawSceneDirectly(const D3DContext &ctxInUse)
{
	D3DEngine::Instance().SetContext(ctxInUse);
}

void AppMultithreadedRendering::DrawShadow(uint32_t iShadow, const D3DContext &ctxInUse)
{
	D3DEngine::Instance().SetContext(ctxInUse);
}

void AppMultithreadedRendering::DrawMirror(uint32_t iMirror, const D3DContext &ctxInUse)
{
	D3DEngine::Instance().SetContext(ctxInUse);
}

void AppMultithreadedRendering::RenderScene()
{
	if (IsMultithreadedPerSceneMode())
	{

	}
	else if (IsDeferredPerSceneMode())
	{

	}
	else
	{

	}

	if (IsDeferredPerSceneMode())
	{

	}
	else
	{

	}

	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);

	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

	Matrix world = m_Camera->GetWorldMatrix();
	g_CBufferVS.WVP = Matrix::Transpose(world * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	g_CBufferVS.World = Matrix::Transpose(world);
	g_CBufferVS.WorldInverse = Matrix::InverseTranspose(world);
	m_CBufferVS.Update(&g_CBufferVS, sizeof(ConstantBufferVS));

	g_CBufferPS.EyePos = m_Camera->GetEyePos();
	m_CBufferPS.Update(&g_CBufferPS, sizeof(ConstantBufferPS));

	///m_SquidRoom.Draw(false);
	m_SquidRoom.DrawEx(false);

	for (uint32_t i = 0U; i < 4U; ++i)
	{
		Light::DebugDisplay(g_CBufferPS.Lights[i].Position, Light::ePoint, *m_Camera, 10.0f);
	}

	ImGui::Text("%.2f FPS", m_FPS);
}
