#include "Gear/Application.h"
#include "Applications/Resource.h"
#include "Colorful/Public/RAsset.h"
#include "Colorful/D3D/D3D11/D3D11Engine.h"
#include "Colorful/D3D/D3D12/D3D12Engine.h"
#include "Colorful/Vulkan/VulkanEngine.h"

namespaceStart(Gear)

void Application::initialize(const std::string& windowTitle)
{
	m_Config.load();

	uint16_t iconID = IconDirectX;
	m_Window = std::make_unique<Window>(windowTitle, m_Config.WindowWidth, m_Config.WindowHeight, iconID);
	m_Window->setMinSize(640u, 480u);

	rAsset::rAssetBucket::instance().initialize(m_Config.RenderEngine);

	postInitialize();
}

void Application::loop()
{
	while (true)
	{
		auto &message = m_Window->message();
		if (message.State == eWindowState::eDestroy)
		{
			break;
		}

		m_Window->update();
	}
}

void Application::finalize()
{
}

namespaceEnd(Gear)
