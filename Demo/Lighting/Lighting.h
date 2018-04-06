#pragma once

#include "D3DApp.h"
#include "D3DGeometry.h"

class AppLighting : public D3DApp
{
public:
	enum eShadingMode
	{
		eFlat,
		eGouraud,
		ePhong,
		eBlinnPhong,
		eShadingModeCount
	};

	AppLighting() = default;
	~AppLighting() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
protected:
private:
	Geometry::Mesh m_FlatSphere;
	Geometry::Mesh m_Sphere;

	eShadingMode m_ShadingMode = eFlat;
	bool m_bWireframe = false;
	bool m_bDrawNormal = false;
};
