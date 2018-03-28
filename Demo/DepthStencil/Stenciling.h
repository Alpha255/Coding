#pragma once

#include "D3DApplication.h"

class ApplicationStenciling : public D3DApplication
{
public:
	ApplicationStenciling();
	~ApplicationStenciling() = default;

	virtual void SetupScene();
	virtual void RenderScene();
	virtual void UpdateScene(float elapsedTime, float totalTime);
protected:
private:
	bool m_bEnableFog = true;
	bool m_bEnableClip = true;
};

