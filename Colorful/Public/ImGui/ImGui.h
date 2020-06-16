#pragma once

#include "Colorful/Public/GfxRenderState.h"
#include <ThirdParty/ImGUI/imgui.h>

class ImGuiRenderer
{
public:
	ImGuiRenderer(class GfxEngine* gfxEngine);

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
	class GfxEngine* m_GfxEngine = nullptr;
	GfxPipelineState m_PipelineState{};
	GfxGpuBufferPtr m_UniformBuffer = nullptr;
	GfxGpuBufferPtr m_VertexBuffer = nullptr;
	GfxGpuBufferPtr m_IndexBuffer = nullptr;
	bool8_t m_Enable = true;
};
using ImGuiRendererPtr = std::unique_ptr<ImGuiRenderer>;