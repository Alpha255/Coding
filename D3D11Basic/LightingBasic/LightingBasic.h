#pragma once

#include "D3DApplication.h"

class AppLightingBasic : public D3DApplication
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

	AppLightingBasic() = default;
	~AppLightingBasic() = default;

	virtual void SetupScene();
	virtual void RenderScene();
protected:
private:
	eShadingMode m_ShadingMode = eFlat;
	bool m_bWireframe = false;
	bool m_bDrawNormal = false;
};
