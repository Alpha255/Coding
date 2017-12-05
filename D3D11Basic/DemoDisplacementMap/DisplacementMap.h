#pragma once

#include <IApplication.h>

class AppDisplacement : public IApplication
{
public:
	AppDisplacement() = default;
	~AppDisplacement() = default;

	virtual void SetupScene();
	virtual void RenderScene();
protected:
private:
	bool m_bWireframe = false;
};
