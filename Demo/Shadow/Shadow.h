#pragma once

#include "RenderApp.h"
#include "D3DGeometry.h"
#include "D3DView.h"
#include "D3DLighting.h"

class AppShadow : public RenderApp
{
public:
	AppShadow() = default;
	~AppShadow() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
protected:
	void DrawClutter(bool bDepthOnly, const Matrix &vp);
	void DrawQuad();
private:
	enum eShaderType
	{
		eDrawMain,
		eDrawQuad,
		eShaderCount
	};

	Geometry::Mesh m_FloorMesh;
	Geometry::Mesh m_BoxMesh;
	Geometry::Mesh m_QuadMesh;

	D3DBuffer m_CBufferVS;
	D3DBuffer m_CBufferPS;

	D3DShaderResourceView m_DepthTexture;
	
	D3DDepthStencilView m_DepthSurface;

	Material m_FloorMaterial;
	Material m_BoxMaterial;

	D3DVertexShader m_VertexShaders[eShaderCount];
	D3DPixelShader m_PixelShaders[eShaderCount];

	D3DViewport m_ViewportsDepth;
};