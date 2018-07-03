
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

		m_ShadowDSV[i].CreateAsTexture(D3DView::eTexture2D, shadowMap, eD32_Float, 0U, 0U);

		m_ShadowViewport[i] = { 0.0f, 0.0f, (float)ShadowMapSize, (float)ShadowMapSize };
	}
}

void AppMultithreadedRendering::InitMirrorResource()
{

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
	m_SquidRoom.Create("SquidRoom\\SquidRoom.sdkmesh", false);

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

	m_SquidRoom.Draw(false);

	ImGui::Text("%.2f FPS", m_FPS);
}
