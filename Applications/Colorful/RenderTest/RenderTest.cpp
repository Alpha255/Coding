#include "RenderTest.h"

using namespace Gfx;

void RenderTest::initialize()
{
	m_Cube = Model::cube(1.0f);
}

void RenderTest::renderScene()
{
	auto cmdBuffer = Gfx::GRenderer->device()->getActiveCommandBuffer();

	cmdBuffer->executeRenderPass(RenderPassDesc{ "Opaque" }, m_Cube->graphicsPipelineState(), [&]() {
		cmdBuffer->drawModel(*m_Cube);
	});
}

void RenderTest::renderGUI()
{
	ImGui::Button("Test");
}

RUN_APPLICATION(RenderTest)
