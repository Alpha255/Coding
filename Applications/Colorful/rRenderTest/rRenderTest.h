#pragma once

#include "Gear/Public/Extension/Application.h"

class rRenderTest : public application
{
public:
	void postInitialize() override;

	void renterToWindow() override;
protected:
private:
	rShader *m_VertexShader = nullptr;
	rShader *m_FragmentShader = nullptr;
};