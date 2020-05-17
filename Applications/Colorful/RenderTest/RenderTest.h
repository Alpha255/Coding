#pragma once

#include "Gear/Application.h"

class RenderTest : public Application
{
public:
	void postInitialize() override final;
	void renderFrame() override final;
protected:
private:
};