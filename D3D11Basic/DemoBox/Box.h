#pragma once

#include "D3DApplication.h"

class ApplicationBox : public D3DApplication
{
public:
	ApplicationBox();
	~ApplicationBox() {}

	virtual void SetupScene();
	virtual void RenderScene();
protected:
private:
};
