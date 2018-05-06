#pragma once

#include "D3DApp.h"
#include "D3DGeometry.h"
#include "D3DView.h"

class AppLightingScene : public D3DApp
{
public:
	AppLightingScene() = default;
	~AppLightingScene() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
protected:
private:
	Geometry::ObjMesh m_House;

	D3DShaderResourceView m_HouseTexDiffuse;
	D3DShaderResourceView m_HouseTexNormal;
	D3DShaderResourceView m_HouseTexSpecular;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;

	D3DBuffer m_CBufferVS;
	D3DBuffer m_CBufferPS;
};
