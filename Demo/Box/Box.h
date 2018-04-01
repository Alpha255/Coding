#pragma once

#include "D3DApp.h"

class AppBox : public D3DApp
{
public:
	AppBox() = default;
	~AppBox() = default;

	virtual void SetupScene();
	virtual void RenderScene();
protected:
private:
};
