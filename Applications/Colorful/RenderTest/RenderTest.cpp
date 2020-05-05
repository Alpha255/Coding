#include "RenderTest.h"

rBuffer *mUniformBuffer = nullptr;
GfxPipelineState mGraphicsPipelineState{};

struct uniformBufferVS
{
	Matrix World;
	Matrix View;
	Matrix Proj;
};

void RenderTest::postInitialize()
{
	auto vertexShader = m_Renderer->createVertexShader("RenderTest.shader");
	auto fragmentShader = m_Renderer->createFragmentShader("RenderTest.shader");
	auto texture = m_Renderer->createTexture("brickwall.dds");

	GfxSamplerDesc samplerDesc{};
	auto sampler = m_Renderer->createSampler(samplerDesc);
	texture->bindSampler(sampler);
	fragmentShader->bindTexture(texture);

	mUniformBuffer = m_Renderer->createUniformBuffer(sizeof(uniformBufferVS), nullptr);

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
	auto vertexBuffer = m_Renderer->createVertexBuffer(eGpuReadWrite, vertices.size() * sizeof(vertex), vertices.data());
	auto indexBuffer = m_Renderer->createIndexBuffer(eGpuReadWrite, indices.size() * sizeof(uint32_t), indices.data());

	std::vector<rVertexAttributes> vertexAttrs
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

	rViewport viewport
	{
		0.0f,
		0.0f,
		m_WindowSize.x,
		m_WindowSize.y
	};
	rScissor scissor
	{
		0.0f,
		0.0f,
		m_WindowSize.x,
		m_WindowSize.y
	};

	vertexShader->setUniformBuffer(mUniformBuffer);
	mGraphicsPipelineState.setShader(vertexShader);
	mGraphicsPipelineState.setShader(fragmentShader);
	mGraphicsPipelineState.setViewport(viewport);
	mGraphicsPipelineState.setScissor(scissor);
	mGraphicsPipelineState.bindVertexBuffer(vertexBuffer);
	mGraphicsPipelineState.bindIndexBuffer(indexBuffer);
	mGraphicsPipelineState.setRenderArea(viewport);
	///m_Renderer->getOpaqueRenderPass()->pendingGfxPipline(mGraphicsPipelineState);

	m_Camera.setProjParams(Math::PI_Div4, m_WindowSize.x / m_WindowSize.y, 0.1f, 500.0f);
	m_Camera.setViewParams(Vec3(0.0f, 0.0f, 4.0f), Vec3(0.0f, 0.0f, 0.0f));
	m_Camera.setScalers(0.01f, 15.0f);
}

void RenderTest::renterToWindow()
{
	Matrix world = Matrix();
	Matrix view = m_Camera.viewMatrix();
	Matrix proj = m_Camera.projMatrix();

	uniformBufferVS uboVS
	{
		world,
		view,
		proj
	};
	m_Renderer->updateGpuBuffer(mUniformBuffer, &uboVS, sizeof(uniformBufferVS), 0u);

	m_Renderer->getOpaqueRenderPass()->bindGfxPipeline(mGraphicsPipelineState);
	m_Renderer->getOpaqueRenderPass()->drawIndexed(mGraphicsPipelineState, 6u, 0u, 0);

	static bool8_t checked = true;
	ImGui::Checkbox("TestCheckBox", &checked);
	ImGui::Text("FPS %.2f", m_FPS);

	m_Camera.update(m_cpuTimer.elapsedTime());
}

appMainEntry(RenderTest)
