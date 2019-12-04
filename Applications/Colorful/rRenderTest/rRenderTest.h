#pragma once

#include "Gear/Public/Extension/Application.h"

class rRenderTest : public application
{
public:
	void postInitialize() override;

	void renterToWindow() override;
protected:
private:
	rShaderPtr m_VertexShader = nullptr;
	rShaderPtr m_FragmentShader = nullptr;
};