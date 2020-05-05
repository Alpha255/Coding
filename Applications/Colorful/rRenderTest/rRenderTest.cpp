#include "rRenderTest.h"

rBuffer *mUniformBuffer = nullptr;
rGraphicsPipelineState mGraphicsPipelineState{};

struct uniformBufferVS
{
	matrix World;
	matrix View;
	matrix Proj;
};

void rRenderTest::postInitialize()
{
	auto vertexShader = m_Renderer->createVertexShader("rRenderTest.shader");
	auto fragmentShader = m_Renderer->createFragmentShader("rRenderTest.shader");
	auto texture = m_Renderer->createTexture("brickwall.dds");

	rSamplerDesc samplerDesc{};
	auto sampler = m_Renderer->createSampler(samplerDesc);
	texture->bindSampler(sampler);
	fragmentShader->bindTexture(texture);

	mUniformBuffer = m_Renderer->createUniformBuffer(sizeof(uniformBufferVS), nullptr);

	struct vertex
	{
		vec3 Position;
		vec2 UV;
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

	m_Camera.setProjParams(math::g_pi_div4, m_WindowSize.x / m_WindowSize.y, 0.1f, 500.0f);
	m_Camera.setViewParams(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f));
	m_Camera.setScalers(0.01f, 15.0f);
}

void rRenderTest::renterToWindow()
{
	matrix world = matrix();
	matrix view = m_Camera.getViewMatrix();
	matrix proj = m_Camera.getProjMatrix();

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

	m_Camera.update(m_cpuTimer.getElapsedTime());
}

appMainEntry(rRenderTest)
