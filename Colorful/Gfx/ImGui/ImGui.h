#pragma once

#include "Colorful/Gfx/GfxRenderer.h"
#include "Colorful/Gfx/GfxRenderState.h"

struct ImGuiContext;

NAMESPACE_START(Gfx)

class ImGuiRenderer : public InputHandler
{
public:
	ImGuiRenderer();
	~ImGuiRenderer();

	void beginFrame(uint32_t width, uint32_t height);
	void endFrame();

	void processInput(const InputState& inputState) override final;

	bool8_t hasFocus() const override final;
protected:
	bool8_t update();

	void renderFrame();
private:
	bool8_t m_Enable = true;
	int32_t m_VertexCount = 0u;
	int32_t m_IndexCount = 0u;

	ImGuiContext* m_Context = nullptr;
	GraphicsPipelineStatePtr m_PipelineState;

	GpuBufferPtr m_VertexBuffer = nullptr;
	GpuBufferPtr m_IndexBuffer = nullptr;
};

NAMESPACE_END(Gfx)