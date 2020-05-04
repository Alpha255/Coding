#pragma once

#include "Colorful/Public/RInterface.h"
#include <ThirdParty/ImGUI/imgui.h>

class ImGuiRenderer
{
public:
	void initialize(uint64_t windowHandle, rEngine *renderEngine);

	void finalize()
	{
		ImGui::DestroyContext();
	}

	void processEvent();

	void begin(float32_t displayWidth, float32_t displayHeight);
	void end();

	bool8_t isMouseButtonDown(ImGuiIO &io);
protected:
	void frame();
	bool8_t update();

	struct UniformBuffer
	{
		vec4 ScaleTranslate;
	};
private:
	rEngine *m_Renderer = nullptr;
	rGraphicsPipelineState m_PipelineState{};
	rRenderPass *m_Renderpass = nullptr;
	rBuffer *m_UniformBuffer = nullptr;
	uint64_t m_WindowHandle = 0u;
};