#pragma once

#include "D3DApplication.h"

class AppAdaptiveTessellation : public D3DApplication
{
public:
	AppAdaptiveTessellation() = default;
	~AppAdaptiveTessellation() = default;

	virtual void SetupScene();
	virtual void RenderScene();

	virtual void ResizeWindow(uint32_t width, uint32_t height);
protected:
private:
	bool m_bEnableTess = false;
};
