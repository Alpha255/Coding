#pragma once

#include "RenderApp.h"
#include "D3DGeometry.h"
#include "D3DView.h"
#include "D3DLighting.h"

class AppGammaCorrection : public RenderApp
{
public:
	AppGammaCorrection() = default;
	~AppGammaCorrection() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
protected:
private:
	Geometry::Mesh m_FloorMesh;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;
	
	D3DBuffer m_CBufferVS;
	D3DBuffer m_CBufferPS;
	
	Material m_FloorMaterial;

	bool m_bGammaCorrection = false;
};
