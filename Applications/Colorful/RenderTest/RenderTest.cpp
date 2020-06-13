#include "RenderTest.h"

GfxGpuBufferPtr mUniformBuffer = nullptr;
GfxPipelineState mGraphicsPipelineState{};

GfxModel ModelTest;

struct UniformBufferVS
{
	Matrix World;
	Matrix View;
	Matrix Proj;
};

void RenderTest::postInitialize()
{
	auto vertexShader = m_GfxEngine->createVertexShader("RenderTest.shader");
	auto fragmentShader = m_GfxEngine->createFragmentShader("RenderTest.shader");
	auto texture = m_GfxEngine->createTexture("metalplate01_rgba.ktx");

	GfxSamplerDesc samplerDesc{};
	auto sampler = m_GfxEngine->createSampler(samplerDesc);
	texture->bindSampler(sampler);
	fragmentShader->setCombinedTextureSampler(texture, 0u);

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

	vertexShader->setUniformBuffer(mUniformBuffer, 0u);
	mGraphicsPipelineState.setShader(vertexShader);
	mGraphicsPipelineState.setShader(fragmentShader);
	mGraphicsPipelineState.bindVertexBuffer(vertexBuffer);
	mGraphicsPipelineState.bindIndexBuffer(indexBuffer);

	m_Camera.setPerspective(Math::PI_Div4, (float32_t)m_Window->width() / m_Window->height(), 0.1f, 500.0f);
	m_Camera.setView(Vec3(0.0f, 0.0f, 4.0f), Vec3(0.0f, 0.0f, 0.0f));

	///ModelTest.load("Wolf.fbx", m_GfxEngine.get());
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

	///ModelTest.draw(m_Camera, m_GfxEngine.get(), viewport);

	static bool8_t checked = true;
	ImGui::Checkbox("TestCheckBox", &checked);
	ImGui::Text("Mouse pos x: %.2f, y: %.2f", m_Window->message().Mouse.Pos.x, m_Window->message().Mouse.Pos.y);
	ImGui::Text("Mouse wheel delta: %.d", m_Window->message().Mouse.WheelDelta);
	ImGui::Text("FrameTime: %.2f ms, FPS: %.2f", m_Profile.FrameTime, m_Profile.FPS);
}

appMainEntry(RenderTest)
