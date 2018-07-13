#pragma once

#include "D3DApp.h"
#include "D3DGeometry.h"
#include "D3DView.h"
#include "D3DState.h"
#include "D3DContext.h"

class AppMultithreadedRendering : public D3DApp
{
public:
	AppMultithreadedRendering() = default;
	~AppMultithreadedRendering() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
	virtual void Update(float deltaTime, float totalTime) override;
	virtual void ResizeWindow(uint32_t width, uint32_t height) override;
protected:
	enum eRenderingMode
	{
		eST,             /// Traditional rendering, one thread, immediate device context
		eMT_PerScene,    /// Multiple threads, one per scene, each with one deferred device context 
		eMT_PerChunk,    /// Multiple threads, one per physical processor, each with one deferred device context
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

	void PerSceneRenderTask(uint32_t taskID);
	void PerChunkRenderTask();

	void InitShadowResource();
	void InitMirrorResource();
	void InitWorkerThreads();

	void SetupScene(const StaticParams &params, const Matrix &world, const Matrix &vp);
	void SetupMirror(uint32_t iMirror);

	void DrawScene(const StaticParams &params, const Matrix &world, const Matrix &vp);
	void DrawShadow(uint32_t iShadow);
	void DrawMirror(uint32_t iMirror);
private:
	int32_t m_RenderingMode = eST;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;
	D3DBuffer m_CBufferVS;
	D3DBuffer m_CBufferPS;

	D3DInputLayout m_Layout;
	D3DInputLayout m_LayoutMirror;

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
