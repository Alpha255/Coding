#pragma once

#include "D3DApplication.h"

class ApplicationBox : public D3DApplication
{
public:
	ApplicationBox() = default;
	~ApplicationBox() = default;

	virtual void SetupScene();
	virtual void RenderScene();
protected:
private:
};
