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
		eLambert,
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
	uint32_t m_IndexCount = 0U;
};
