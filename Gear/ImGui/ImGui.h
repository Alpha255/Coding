#pragma once

#include "Colorful/Public/GfxRenderer.h"
#include <ThirdParty/ImGUI/imgui.h>

NAMESPACE_START(Gear)

extern Gfx::GfxRenderer* GRenderer;

DECLARE_UNIQUE_PTR(ImGuiRenderer)
class ImGuiRenderer
{
public:
	ImGuiRenderer();

	~ImGuiRenderer()
	{
		ImGui::DestroyContext();
	}

	void begin()
	{
		if (m_Enable)
		{
			ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_FirstUseEver);

			ImGui::NewFrame();
		}
	}

	void end()
	{
		if (m_Enable)
		{
			ImGui::Render();

			draw();
		}
	}

	void enable(bool8_t enable)
	{
		m_Enable = enable;
	}

	void processInput(const Gear::InputState& inputState);

	//bool8_t isMouseButtonDown(ImGuiIO &io);

	//inline bool8_t isFocus() const
	//{
	//	const ImGuiIO &io = ImGui::GetIO();
	//	return io.WantCaptureMouse || io.WantCaptureKeyboard;
	//}

protected:
	void draw();
	bool8_t update();
private:
	bool8_t m_Enable = true;

	Gfx::GraphicsPipelineState m_GfxPipelineState{};
};

NAMESPACE_END(Gear)