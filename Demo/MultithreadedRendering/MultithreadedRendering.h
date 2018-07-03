#pragma once

#include "D3DApp.h"
#include "D3DGeometry.h"
#include "D3DView.h"

class AppMultithreadedRendering : public D3DApp
{
public:
	AppMultithreadedRendering() = default;
	~AppMultithreadedRendering() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
protected:
	enum eRenderingMode
	{
		eSingleThread_IMContext,             /// Traditional rendering, one thread, immediate device context
		eSingleThread_DefContext_PerScene,   /// One thread, multiple deferred device contexts, one per scene 
		eSingleThread_DefContext_PerChunk,   /// One thread, multiple deferred device contexts, one per physical processor
		eMultiThread_DefContext_PerScene,    /// Multiple threads, one per scene, each with one deferred device context 
		eMultiThread_DefContext_PerChunk,    /// Multiple threads, one per physical processor, each with one deferred device context
	};

	enum eConstants
	{
		eNumShadows = 1U,
		eNumMirrors = 4U,
		eNumScenes = 1U + eNumShadows + eNumMirrors
	};

	inline bool IsDeferredPerSceneMode()
	{
		return eSingleThread_DefContext_PerScene == m_RenderingMode ||
			eMultiThread_DefContext_PerScene == m_RenderingMode;
	}

	inline bool IsMultithreadedPerSceneMode()
	{
		return eMultiThread_DefContext_PerScene == m_RenderingMode;
	}

	inline bool IsDeferredPerChunkMode()
	{
		return eSingleThread_DefContext_PerChunk == m_RenderingMode ||
			eMultiThread_DefContext_PerChunk == m_RenderingMode;
	}

	inline bool IsMultithreadedDeferredPerChunkMode()
	{
		return eMultiThread_DefContext_PerChunk == m_RenderingMode;
	}

	inline bool IsDeferredMode()
	{
		return IsDeferredPerSceneMode() || IsDeferredPerChunkMode();
	}

	void PerSceneRenderTask(uint32_t taskID);
	void PerChunkRenderTask();

	void InitShadowResource();
	void InitMirrorResource();
	void InitWorkerThreads();
private:
	eRenderingMode m_RenderingMode = eSingleThread_IMContext;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;
	D3DBuffer m_CBufferVS;

	D3DInputLayout m_Layout;
	D3DInputLayout m_LayoutMirror;

	D3DContext m_SceneDefContexts[eNumScenes];
	D3DCommandList m_SceneCmdList[eNumScenes];

	D3DShaderResourceView m_ShadowSRV[eNumShadows];
	D3DDepthStencilView m_ShadowDSV[eNumShadows];
	D3DViewport m_ShadowViewport[eNumShadows];

	Geometry::SDKMesh m_SquidRoom;
};
