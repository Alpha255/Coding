
#include "MultithreadedRendering.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "D3DGUI_imGui.h"
#include "D3DTexture.h"

#include <thread>

struct ConstantBufferVS
{
	Matrix WVP;
	Matrix World;
	Matrix WorldInverse;
};

ConstantBufferVS g_CBufferVS;

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

	Matrix world = m_Camera->GetWorldMatrix();
	g_CBufferVS.WVP = Matrix::Transpose(world * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	g_CBufferVS.World = Matrix::Transpose(world);
	g_CBufferVS.WorldInverse = Matrix::InverseTranspose(world);
	m_CBufferVS.Update(&g_CBufferVS, sizeof(ConstantBufferVS));

	///m_SquidRoom.Draw(false);
	m_SquidRoom.DrawEx(false);

	ImGui::Text("%.2f FPS", m_FPS);
}
