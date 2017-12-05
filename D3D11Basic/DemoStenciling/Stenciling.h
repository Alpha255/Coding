#pragma once

#include <IApplication.h>

class ApplicationStenciling : public IApplication
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

