#pragma once

#include "D3DApp.h"
#include "D3DGeometry.h"

class AppBox : public D3DApp
{
public:
	AppBox() = default;
	~AppBox() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
protected:
private:
	Geometry::Mesh m_BoxMesh;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;
	D3DBuffer m_CBufferVS;
	D3DShaderResourceView m_DiffuseTex;
};
