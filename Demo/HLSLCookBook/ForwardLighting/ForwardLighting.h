#pragma once

#include "D3DApp.h"
#include "D3DGeometry.h"
#include "D3DView.h"
#include "D3DLighting.h"

class AppForwardLighting : public D3DApp
{
public:
	AppForwardLighting() = default;
	~AppForwardLighting() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
protected:
	inline Vec4 GammaToLinear(const Vec4 &color)
	{
		/// Approximation
		return color * color;
	}
private:
	enum eLightingType
	{
		eHemisphericAmbient,
		eDirectional,
		ePoint,
		eSpot,
		eCount
	};

	Geometry::ObjMesh m_StanfordBunny;
	Geometry::Mesh m_Floor;
	///Geometry::SDKMesh m_StanfordBunnyMesh;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader[eCount];
	D3DBuffer m_CBufferVS;
	D3DBuffer m_CBufferPS;

	Material m_BunnyMaterial;

	int32_t m_LightingType = eHemisphericAmbient;
	bool m_Wireframe = false;
};
