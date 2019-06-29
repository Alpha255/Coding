#pragma once

#include "Public/IRenderApplication.h"
#include "Public/Definitions.h"
#include "Public/Model.h"

class MultithreadedRendering : public IRenderApplication
{
public:
	MultithreadedRendering() = default;
	~MultithreadedRendering() = default;

	void PrepareScene() override;
	void RenderScene() override;
	void UpdateScene(float elapsedTime, float totalTime) override;
	///void ResizeWindow(uint32_t width, uint32_t height) override;

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
protected:
	struct StaticParams
	{
		RDepthStencilState DepthStencilState;
		uint8_t StencilRef = 0U;

		RRasterizerState RasterizerState;

		RDepthStencilView DepthStencilView;
		RViewport Viewport;

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

	void SetupScene(const StaticParams &params, const Matrix &world, const Matrix &vp);
	void SetupMirror(uint32_t iMirror);

	void DrawScene(const StaticParams &params, const Matrix &world, const Matrix &vp);
	void DrawShadow(uint32_t iShadow);
	void DrawMirror(uint32_t iMirror);
private:
	int32_t m_RenderingMode = eST;

	RVertexShader m_VertexShader;
	RPixelShader m_PixelShader;
	RBuffer m_CBufferVS;
	RBuffer m_CBufferPS;

	RInputLayout m_Layout;
	RInputLayout m_LayoutMirror;

	RShaderResourceView m_ShadowSRV[eNumShadows];

	RDepthStencilState m_NoStencil;
	RDepthStencilState m_DepthTestStencilOverwrite;
	RDepthStencilState m_DepthOverwriteStencilTest;
	RDepthStencilState m_DepthWriteStencilTest;
	RDepthStencilState m_DepthOverwriteStencilClear;

	RBuffer m_VertexBufferMirror;

	StaticParams m_StaticParamsDirectly;
	StaticParams m_StaticParamsShadows[eNumShadows];
	StaticParams m_StaticParamsMirrors[eNumMirrors];

	MirrorRect m_MirrorRect[eNumMirrors];
	Matrix m_MirrorWorld[eNumMirrors];

	Geometry::Model m_SquidRoom;
};
