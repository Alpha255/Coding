#pragma once

#include "D3DApp.h"
#include "D3DGeometry.h"
#include "D3DView.h"

class AppParallaxMapping : public D3DApp
{
public:
	AppParallaxMapping() = default;
	~AppParallaxMapping() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
protected:
	enum eMappingType
	{
		eNormalMapping,
		eParallaxOcclusionMapping,
		eDisplacementMapping,
		eMappingTypeCount
	};

private:
	Geometry::Mesh m_CylinderMesh;

	D3DVertexShader m_VertexShader[eMappingTypeCount];
	D3DPixelShader m_PixelShader[eMappingTypeCount];

	D3DBuffer m_CBufferVS;
	D3DBuffer m_CBufferPS;

	D3DShaderResourceView m_DiffuseMap;
	D3DShaderResourceView m_NormalMap;
	D3DShaderResourceView m_HeightMap;

	uint32_t m_MappingType = eNormalMapping;
	bool m_bWireframe = false;
};
