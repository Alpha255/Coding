#pragma once

#include "Gear/Application.h"

class RenderTest : public Application
{
public:
	void postInitialize() override final;
	void renderFrame() override final;
protected:
private:
	GfxGpuBufferPtr m_UniformBufferVS = nullptr;
	GfxGpuBufferPtr m_VertexBuffer = nullptr;
	GfxGpuBufferPtr m_IndexBuffer = nullptr;
	GfxPipelineState m_PipelineState{};
};