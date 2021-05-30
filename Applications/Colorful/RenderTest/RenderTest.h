#pragma once

#include "Gear/Application.hpp"

class RenderTest final : public Application
{
public:
	void initialize() override final;

	void renderScene() override final;

	void renderGUI() override final;
protected:
private:
	Gfx::ModelPtr m_Cube;
};