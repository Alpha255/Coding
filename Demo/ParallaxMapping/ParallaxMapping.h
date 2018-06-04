#pragma once

#include "D3DApp.h"
#include "D3DGeometry.h"
#include "D3DView.h"
#include "D3DLighting.h"

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
		eParallaxOcclusionMappingInACL,
		eParallaxMappingWithOffsetLimit,
		eParallaxOcclusionMapping,
		eDisplacementMapping,
		eMappingTypeCount
	};

private:
	Geometry::Mesh m_Floor;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader[eMappingTypeCount];

	D3DBuffer m_CBufferVS;
	D3DBuffer m_CBufferPS;

	Material m_FloorMaterial;

	D3DShaderResourceView m_HeightMap;

	bool m_VSync = false;

	int32_t m_MappingType = eNormalMapping;
};
