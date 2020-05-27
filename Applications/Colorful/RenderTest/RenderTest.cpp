#include "RenderTest.h"

GfxGpuBufferPtr mUniformBuffer = nullptr;
GfxPipelineState mGraphicsPipelineState{};

struct UniformBufferVS
{
	Matrix World;
	Matrix View;
	Matrix Proj;
};

void RenderTest::postInitialize()
{
#if 0
	auto dirty = mGraphicsPipelineState.isDirty(GfxPipelineState::eViewport);
	mGraphicsPipelineState.setDirty(GfxPipelineState::eViewport);
	dirty = mGraphicsPipelineState.isDirty(GfxPipelineState::eViewport);

	dirty = mGraphicsPipelineState.isDirty(GfxPipelineState::eScissor);
	mGraphicsPipelineState.setDirty(GfxPipelineState::eScissor);
	dirty = mGraphicsPipelineState.isDirty(GfxPipelineState::eScissor);

	dirty = mGraphicsPipelineState.isDirty(GfxPipelineState::eVertexBuffer);
	mGraphicsPipelineState.setDirty(GfxPipelineState::eVertexBuffer);
	dirty = mGraphicsPipelineState.isDirty(GfxPipelineState::eVertexBuffer);

	dirty = mGraphicsPipelineState.isDirty(GfxPipelineState::eIndexBuffer);
	mGraphicsPipelineState.setDirty(GfxPipelineState::eIndexBuffer);
	dirty = mGraphicsPipelineState.isDirty(GfxPipelineState::eIndexBuffer);

	dirty = mGraphicsPipelineState.isDirty(GfxPipelineState::eViewport);
	dirty = mGraphicsPipelineState.isDirty(GfxPipelineState::eScissor);
	dirty = mGraphicsPipelineState.isDirty(GfxPipelineState::eVertexBuffer);
	dirty = mGraphicsPipelineState.isDirty(GfxPipelineState::eIndexBuffer);
#endif
	auto vertexShader = m_GfxEngine->createVertexShader("RenderTest.shader");
	auto fragmentShader = m_GfxEngine->createFragmentShader("RenderTest.shader");
	auto texture = m_GfxEngine->createTexture("brickwall.dds");

	GfxSamplerDesc samplerDesc{};
	auto sampler = m_GfxEngine->createSampler(samplerDesc);
	texture->bindSampler(sampler);
	fragmentShader->bindTexture(texture);

	mUniformBuffer = m_GfxEngine->createUniformBuffer(sizeof(UniformBufferVS), nullptr);

	struct vertex
	{
		Vec3 Position;
		Vec2 UV;
	};
	std::vector<vertex> vertices{
		{
			{ 1.0f, 1.0f, 0.0f },
			{ 1.0f, 1.0f }
		},
		{
			{ -1.0f, 1.0f, 0.0f },
			{  0.0f, 1.0f }
		},
		{
			{  -1.0f, -1.0f, 0.0f },
			{  0.0f,  0.0f }
		},
		{
			{  1.0f, -1.0f, 0.0f },
			{  1.0f,  0.0f }
		}
	};
	std::vector<uint32_t> indices{
		0u, 1u, 2u, 2u, 3u, 0u
	};
	auto vertexBuffer = m_GfxEngine->createVertexBuffer(eGpuReadWrite, vertices.size() * sizeof(vertex), vertices.data());
	auto indexBuffer = m_GfxEngine->createIndexBuffer(eGpuReadWrite, indices.size() * sizeof(uint32_t), indices.data());

	std::vector<GfxVertexAttributes> vertexAttrs
	{
		{
			ePosition,
			eRGB32_Float,
			eVertex
		},
		{
			eTexcoord,
			eRGB32_Float,
			eVertex
		}
	};
	/// alignment ??? Try to create based on shader reflection(Format) ??? 
	vertexShader->setInputLayout(vertexAttrs, alignof(vertex));

	vertexShader->bindUniformBuffer(mUniformBuffer);
	mGraphicsPipelineState.setShader(vertexShader);
	mGraphicsPipelineState.setShader(fragmentShader);
	mGraphicsPipelineState.bindVertexBuffer(vertexBuffer);
	mGraphicsPipelineState.bindIndexBuffer(indexBuffer);

	m_Camera.setPerspective(Math::PI_Div4, (float32_t)m_Window->width() / m_Window->height(), 0.1f, 500.0f);
	m_Camera.setView(Vec3(0.0f, 0.0f, 4.0f), Vec3(0.0f, 0.0f, 0.0f));
	m_Camera.setScalers(0.01f, 15.0f);
}

void RenderTest::renderFrame()
{
	Matrix world = Matrix();
	Matrix view = m_Camera.viewMatrix();
	Matrix proj = m_Camera.projectionMatrix();

	UniformBufferVS uboVS
	{
		world,
		view,
		proj
	};
	mUniformBuffer->update(&uboVS, sizeof(UniformBufferVS), 0u);

	GfxViewport viewport
	{
		0.0f,
		0.0f,
		(float32_t)m_Window->width(),
		(float32_t)m_Window->height()
	};
	GfxScissor scissor
	{
		0.0f,
		0.0f,
		(float32_t)m_Window->width(),
		(float32_t)m_Window->height()
	};
	mGraphicsPipelineState.setViewport(viewport);
	mGraphicsPipelineState.setScissor(scissor);
	mGraphicsPipelineState.setRenderArea(viewport);

	m_GfxEngine->opaqueRenderPass()->bindGfxPipeline(mGraphicsPipelineState);
	m_GfxEngine->opaqueRenderPass()->drawIndexed(6u, 0u, 0);

	static bool8_t checked = true;
	ImGui::Checkbox("TestCheckBox", &checked);
	ImGui::Text("Mouse pos x: %.2f, y: %.2f", m_Window->message().Mouse.Pos.x, m_Window->message().Mouse.Pos.y);
	ImGui::Text("Mouse delta pos x: %.2f, y: %.2f", m_Window->message().Mouse.DeltaPos.x, m_Window->message().Mouse.DeltaPos.y);
	ImGui::Text("Mouse wheel delta: %.d", m_Window->message().Mouse.WheelDelta);
	ImGui::Text("ElapsedTime: %.2f", m_CpuTimer.elapsedTime());
	ImGui::Text("ElapsedTimeInSeconds: %.2f", m_CpuTimer.elapsedTimeInSeconds());
	ImGui::Text("FPS: %.2f", m_FPS);
}

appMainEntry(RenderTest)
