#pragma once

#include <IApplication.h>

class ApplicationBox : public IApplication
{
public:
	ApplicationBox();
	~ApplicationBox() {}

	virtual void SetupScene();
	virtual void RenderScene();
protected:
private:
};
