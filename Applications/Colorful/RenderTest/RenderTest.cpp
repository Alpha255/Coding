#include "RenderTest.h"

struct UniformBufferVS
{
	Matrix World;
	Matrix View;
	Matrix Proj;
};

struct Vertex
{
	Vec3 Position;
	Vec2 UV;
};

GfxModel ModelTest;

void RenderTest::postInitialize()
{
	auto vertexShader = m_GfxEngine->createVertexShader("RenderTest.shader");
	auto fragmentShader = m_GfxEngine->createFragmentShader("RenderTest.shader");

	GfxSamplerDesc samplerDesc{};
	auto sampler = m_GfxEngine->createSampler(samplerDesc);
	auto texture = m_GfxEngine->createTexture("metalplate01_rgba.ktx");
	fragmentShader->setCombinedTextureSampler(texture, sampler, 1u);

	m_UniformBufferVS = m_GfxEngine->createUniformBuffer(sizeof(UniformBufferVS), nullptr);

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
	vertexShader->setInputLayout(vertexAttrs, alignof(Vertex));
	vertexShader->setUniformBuffer(m_UniformBufferVS, 0u);

	std::vector<Vertex> vertices{
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
	m_VertexBuffer = m_GfxEngine->createVertexBuffer(eGpuReadWrite, vertices.size() * sizeof(Vertex), vertices.data());
	m_IndexBuffer = m_GfxEngine->createIndexBuffer(eGpuReadWrite, indices.size() * sizeof(uint32_t), indices.data());

	m_PipelineState.setShader(vertexShader);
	m_PipelineState.setShader(fragmentShader);

	m_Camera.setPerspective(Math::PI_Div4, (float32_t)m_Window->width() / m_Window->height(), 0.1f, 500.0f);
	m_Camera.setView(Vec3(0.0f, 0.0f, 4.0f), Vec3(0.0f, 0.0f, 0.0f));

	ModelTest.load("wolf.fbx", m_GfxEngine.get());
}

/// Expose command buffer for application ????

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
	m_UniformBufferVS->update(&uboVS, sizeof(UniformBufferVS), 0u);

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

	GfxScopeGpuMarker("Opaque", Color::randomColor());

	m_PipelineState.setFrameBuffer(m_GfxEngine->backBuffer());
	m_PipelineState.setViewport(viewport);
	m_PipelineState.setScissor(scissor);
	m_PipelineState.bindVertexBuffer(m_VertexBuffer);
	m_PipelineState.bindIndexBuffer(m_IndexBuffer);
	m_GfxEngine->bindGfxPipelineState(&m_PipelineState);
	m_GfxEngine->drawIndexed(6u, 0u, 0);

	ModelTest.draw(m_Camera, m_GfxEngine.get(), viewport);

	static bool8_t checked = true;
	ImGui::Checkbox("TestCheckBox", &checked);
	ImGui::Text("Mouse pos x: %.2f, y: %.2f", m_Window->message().Mouse.Pos.x, m_Window->message().Mouse.Pos.y);
	ImGui::Text("Mouse wheel delta: %d", m_Window->message().Mouse.WheelDelta);
	ImGui::Text("FrameTime: %.2f ms, FPS: %.2f", m_Profile.FrameTime, m_Profile.FPS);
}

appMainEntry(RenderTest)
