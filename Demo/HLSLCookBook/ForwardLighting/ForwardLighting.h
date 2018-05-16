#pragma once

#include "D3DApp.h"
#include "D3DGeometry.h"
#include "D3DView.h"

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
		return color * color;
	}
private:
	enum eLightingType
	{
		eHemisphericAmbient
	};

	Geometry::ObjMesh m_StanfordBunny;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;
	D3DBuffer m_CBufferVS;
	D3DBuffer m_CBufferPS;

	int32_t m_LightingType = eHemisphericAmbient;
	bool m_Wireframe = false;
};
