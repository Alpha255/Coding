#pragma once

#include "Colorful/Public/GfxEngine.h"
#include <ThirdParty/ImGUI/imgui.h>

class ImGuiRenderer
{
public:
	void initialize(uint64_t windowHandle, GfxEngine *renderEngine);

	void finalize()
	{
		ImGui::DestroyContext();
	}

	void processEvent();

	void begin(float32_t displayWidth, float32_t displayHeight);
	void end();

	bool8_t isMouseButtonDown(ImGuiIO &io);

	inline bool8_t isFocus() const
	{
		if (!m_WindowHandle)
		{
			return false;
		}
		const ImGuiIO &io = ImGui::GetIO();
		return io.WantCaptureMouse || io.WantCaptureKeyboard;
	}
protected:
	void frame();
	bool8_t update();

	struct UniformBuffer
	{
		Vec4 ScaleTranslate;
	};
private:
	GfxEngine *m_Renderer = nullptr;
	GfxPipelineState m_PipelineState{};
	GfxRenderPass *m_Renderpass = nullptr;
	rBuffer *m_UniformBuffer = nullptr;
	uint64_t m_WindowHandle = 0u;
};