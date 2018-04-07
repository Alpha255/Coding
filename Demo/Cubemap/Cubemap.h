#pragma once

#include "Camera.h"
#include "D3DApp.h"
#include "D3DSkyBox.h"

class AppCubemap : public D3DApp
{
public:
	AppCubemap() = default;
	~AppCubemap() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
protected:
	void InitCubemapCameras(float cx, float cy, float cz);

	void DrawScene(const Camera &cam);
	void DrawDynamicCubemap();
private:
	Geometry::Mesh m_CubeMesh;

	D3DSkyBox m_SkyBox;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;

	D3DBuffer m_CBufferVS;
	D3DBuffer m_CBufferPS;

	D3DShaderResourceView m_DiffuseTex;
	D3DShaderResourceView m_CubeMap;

	D3DRenderTargetView m_CubemapRTV[6];
	D3DDepthStencilView m_CubemapDSV;

	D3DViewport m_CubemapViewport;
	Camera m_CubemapCamera[6];
};
