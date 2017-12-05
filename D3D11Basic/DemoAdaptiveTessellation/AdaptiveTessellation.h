#pragma once

#include <IApplication.h>

class AppAdaptiveTessellation : public IApplication
{
public:
	AppAdaptiveTessellation() = default;
	~AppAdaptiveTessellation() = default;

	virtual void SetupScene();
	virtual void RenderScene();
protected:
private:
};
