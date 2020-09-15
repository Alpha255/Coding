#include "Gear/Application.h"

NAMESPACE_START(Gear)

void Application::Configs::bindAttributes()
{

}

Application::Configs::Configs()
{
	bindAttributes();
}

void Application::mountAssetsDirectory()
{
}

void Application::initialize(const std::string& name, const Configs& configs)
{
	m_Window = std::make_unique<Window>(m_Instance, name, configs.WindowSize, configs.MinWindowSize);
	mountAssetsDirectory();
	onInitialize();
}

void Application::run()
{
}

void Application::finalize()
{
	onFinalize();
}

NAMESPACE_END(Gear)
