#pragma once

#include "D3DApp.h"
#include "D3DGeometry.h"
#include "D3DView.h"
#include "D3DState.h"

class AppMultithreadedRendering : public D3DApp
{
public:
	AppMultithreadedRendering() = default;
	~AppMultithreadedRendering() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
	virtual void Update(float deltaTime, float totalTime) override;
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

	struct StaticParams
	{
		D3DDepthStencilState DepthStencilState;
		uint8_t StencilRef = 0U;

		D3DRasterizerState RasterizerState;

		D3DDepthStencilView DepthStencilView;
		D3DViewport Viewport;

		Vec4 TintColor = {};
		Vec4 MirrorPlane = {};
	};

	struct MirrorVertex
	{
		Vec3 Position;
		Vec3 Normal;
		Vec2 Texcoord;
		Vec3 Tangent;
	};
	typedef MirrorVertex MirrorRect[4];

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

	void SetupScene(const StaticParams &params, const Matrix &vp);
	void SetupMirror(uint32_t iMirror, const Matrix &vp);
	void DrawScene(const StaticParams &params, const Matrix &vp);
	void DrawShadow(uint32_t iShadow, const D3DContext &ctxInUse);
	void DrawMirror(uint32_t iMirror, const D3DContext &ctxInUse);
private:
	eRenderingMode m_RenderingMode = eSingleThread_IMContext;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;
	D3DBuffer m_CBufferVS;
	D3DBuffer m_CBufferPS;

	D3DInputLayout m_Layout;
	D3DInputLayout m_LayoutMirror;

	D3DContext m_SceneDefContexts[eNumScenes];
	D3DContext m_IMContext;
	D3DCommandList m_SceneCmdList[eNumScenes];

	D3DShaderResourceView m_ShadowSRV[eNumShadows];

	D3DDepthStencilState m_NoStencil;
	D3DDepthStencilState m_DepthTestStencilOverwrite;
	D3DDepthStencilState m_DepthOverwriteStencilTest;
	D3DDepthStencilState m_DepthWriteStencilTest;
	D3DDepthStencilState m_DepthOverwriteStencilClear;

	D3DBuffer m_VertexBufferMirror;

	StaticParams m_StaticParamsDirectly;
	StaticParams m_StaticParamsShadows[eNumShadows];
	StaticParams m_StaticParamsMirrors[eNumMirrors];

	MirrorRect m_MirrorRect[eNumMirrors];
	Matrix m_MirrorWorld[eNumMirrors];

	Geometry::SDKMesh m_SquidRoom;
};
