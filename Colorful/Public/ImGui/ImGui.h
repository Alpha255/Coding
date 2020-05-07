#pragma once

#include "Colorful/Public/GfxRenderState.h"
#include <ThirdParty/ImGUI/imgui.h>

class ImGuiRenderer
{
public:
	void initialize(class GfxEngine *renderEngine);

	void finalize()
	{
		ImGui::DestroyContext();
	}

	void processMessage(const struct WindowMessage& message, uint32_t width, uint32_t height);

	void begin();
	void end();

	void setEnable(bool8_t enable)
	{
		m_Enable = enable;
	}

	bool8_t isMouseButtonDown(ImGuiIO &io);

	inline bool8_t isFocus() const
	{
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
	class GfxEngine *m_Renderer = nullptr;
	GfxPipelineState m_PipelineState{};
	GfxGpuBuffer *m_UniformBuffer = nullptr;
	bool8_t m_Enable = true;
};
using ImGuiRendererPtr = std::unique_ptr<ImGuiRenderer>;