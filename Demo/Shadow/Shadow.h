#pragma once

#include "D3DApp.h"
#include "D3DGeometry.h"
#include "D3DView.h"
#include "D3DLighting.h"

class AppShadow : public D3DApp
{
public:
	AppShadow() = default;
	~AppShadow() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
	virtual void ResizeWindow(uint32_t width, uint32_t height) override;
protected:
	void DrawClutter();
	void DrawQuad();
private:
	enum eShaderType
	{
		eDrawMain,
		eDrawDepth,
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

	D3DViewport m_Viewports[eDrawQuad];
};
