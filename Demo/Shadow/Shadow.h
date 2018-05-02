#pragma once

#include "D3DApp.h"
#include "D3DGeometry.h"
#include "D3DView.h"

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
	Geometry::Mesh m_FloorMesh;
	Geometry::Mesh m_BoxMesh;
	Geometry::Mesh m_QuadMesh;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;
	D3DBuffer m_CBufferVS;
	D3DShaderResourceView m_DiffuseTexFloor;
	D3DShaderResourceView m_DiffuseTexBox;
	D3DViewport m_Viewport;

	D3DVertexShader m_VertexShaderDepth;
	D3DPixelShader m_PixelShaderDepth;
	D3DShaderResourceView m_DepthTexture;
	D3DDepthStencilView m_DepthDSV;
	D3DRenderTargetView m_DepthSurface;
	D3DViewport m_ViewportDepth;

	D3DVertexShader m_VertexShaderQuad;
	D3DPixelShader m_PixelShaderQuad;
};
