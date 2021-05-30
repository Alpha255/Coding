#pragma once

#include "Gear/Gear.h"
#include "Gear/Rockcat.hpp"
#include <ThirdParty/ImGUI/imgui.h>

NAMESPACE_START(Gear)

class Application : public NoneCopyable
{
public:
	Application()
	{
		initialize();
	}

	virtual ~Application()
	{
		finalize();
	}

	virtual void initialize() {}

	virtual void resize(uint32_t, uint32_t) {}

	virtual void update(float32_t) {}

	virtual void renderScene() {}

	virtual void renderGUI() {}

	virtual void finalize() {}
};

NAMESPACE_END(Gear)