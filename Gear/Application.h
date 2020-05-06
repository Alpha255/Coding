#pragma once

#include "Gear/Gear.h"
#include "Gear/Window.h"

namespaceStart(Gear)

class Application
{
public:
	Application() = default;
	virtual ~Application() = default;

	void initialize(const std::string& windowTitle);

	virtual void postInitialize() {}

	virtual void loop();

	virtual void finalize();
protected:
	Configurations m_Config;
	WindowPtr m_Window = nullptr;
private:
};

namespaceEnd(Gear)
