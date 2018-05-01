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
protected:
	void DrawClutter();
	void DrawQuad();
private:
	Geometry::Mesh m_FloorMesh;
	Geometry::Mesh m_BoxMesh;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;
	D3DBuffer m_CBufferVS;
	D3DShaderResourceView m_DiffuseTexFloor;
	D3DShaderResourceView m_DiffuseTexBox;
};
