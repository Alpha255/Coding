#pragma once

#include "D3DApplication.h"

class AppAlphaBlend : public D3DApplication
{
public:
	AppAlphaBlend() = default;
	~AppAlphaBlend() = default;

	virtual void SetupScene();
	virtual void RenderScene();
protected:
private:
};
