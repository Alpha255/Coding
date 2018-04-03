#pragma once

#include "D3DApp.h"

class AppBox : public D3DApp
{
public:
	AppBox() = default;
	~AppBox() = default;

	virtual void PreInit() override;
	virtual void RenderScene() override;
protected:
private:
};
