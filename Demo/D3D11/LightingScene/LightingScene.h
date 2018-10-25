#pragma once

#include "D3DApp.h"
#include "D3DGeometry.h"
#include "D3DView.h"
#include "D3DLighting.h"

class AppLightingScene : public D3DApp
{
public:
	AppLightingScene() = default;
	~AppLightingScene() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
protected:
private:
#if 0
	Geometry::ObjMesh m_House;
	Geometry::ObjMesh m_Lamp;

	Geometry::Mesh m_Ground;

	D3DShaderResourceView m_HouseTexDiffuse;
	D3DShaderResourceView m_HouseTexNormal;
	D3DShaderResourceView m_HouseTexSpecular;

	D3DShaderResourceView m_LampTexDiffuse;
	D3DShaderResourceView m_LampTexNormal;
	D3DShaderResourceView m_LampTexSpecular;

	D3DShaderResourceView m_GroundTexDiffuse;
	D3DShaderResourceView m_GroundTexNormal;
	D3DShaderResourceView m_GroundTexSpecular;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;

	D3DBuffer m_CBufferVS;
	D3DBuffer m_CBufferPS;

	bool m_Wireframe = false;
#else
	Geometry::Mesh m_Wall;

	Material m_WallMat;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;

	D3DBuffer m_CBufferVS;
	D3DBuffer m_CBufferPS;
#endif
};
