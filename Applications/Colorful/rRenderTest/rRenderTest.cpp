#include "rRenderTest.h"

rBuffer *mUniformBuffer = nullptr;
rGraphicsPipelineState mGraphicsPipelineState{};
rRenderPass *mRenderPass = nullptr;

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
			eColor,
			eRGB32_Float,
			eVertex
		}
	};
	/// alignment ??? Try to create based on shader reflection(Format) ??? 
	vertexShader->setInputLayout(vertexAttrs, alignof(vertex));

	auto depthStencilView = m_Renderer->createDepthStencilView((uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y, eD24_UNorm_S8_UInt);

	rFrameBufferDesc frameBufferDesc{};
	frameBufferDesc.DepthSurface = depthStencilView;
	frameBufferDesc.Width = (uint32_t)m_WindowSize.x;
	frameBufferDesc.Height = (uint32_t)m_WindowSize.y;

	mRenderPass = m_Renderer->createRenderPass(frameBufferDesc);

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
	mGraphicsPipelineState.drawIndexed((uint32_t)indices.size(), 0u, 0u);
	mGraphicsPipelineState.setRenderArea(viewport);

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
	m_Renderer->updateUniformBuffer(mUniformBuffer, &uboVS, sizeof(uniformBufferVS), 0u);

	mRenderPass->execute(mGraphicsPipelineState);

	m_Camera.update(m_cpuTimer.getElapsedTime());
}

appMainEntry(rRenderTest)
