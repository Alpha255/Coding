#include "rRenderTest.h"

rBuffer *mUniformBuffer = nullptr;

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

	mUniformBuffer = m_Renderer->createUniformBuffer(sizeof(uniformBufferVS), nullptr);

	struct vertex
	{
		vec3 Position;
		vec3 Color;
	};
	std::vector<vertex> vertices{
		{
			{ 1.0f, 1.0f, 0.0f },
			{ 1.0f, 0.0f, 0.0f }
		},
		{
			{ -1.0f, 1.0f, 0.0f },
			{  0.0f, 1.0f, 0.0f }
		},
		{
			{  0.0f, -1.0f, 0.0f },
			{  0.0f,  0.0f, 1.0f }
		}
	};
	std::vector<uint32_t> indices{
		0u, 1u, 2u
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
	vertexShader->setInputLayout(vertexAttrs);

	auto depthStencilView = m_Renderer->createDepthStencilView((uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y, eD24_UNorm_S8_UInt);

	rFrameBufferDesc frameBufferDesc{};
	frameBufferDesc.DepthSurface = depthStencilView;
	frameBufferDesc.Width = (uint32_t)m_WindowSize.x;
	frameBufferDesc.Height = (uint32_t)m_WindowSize.y;

	auto renderPass = m_Renderer->createRenderPass(frameBufferDesc);

	rViewport viewport
	{
		0.0f,
		0.0f,
		m_WindowSize.x,
		m_WindowSize.y
	};
	rScissor scissor
	{
		m_WindowSize.x,
		m_WindowSize.y,
		0.0f,
		0.0f
	};

	vertexShader->setUniformBuffer(mUniformBuffer);
	rGraphicsPipelineState graphicsPipelineState{};
	graphicsPipelineState.setShader(vertexShader);
	graphicsPipelineState.setShader(fragmentShader);
	graphicsPipelineState.setViewport(viewport);
	graphicsPipelineState.setScissor(scissor);
	graphicsPipelineState.bindVertexBuffer(vertexBuffer);
	graphicsPipelineState.bindIndexBuffer(indexBuffer);
	graphicsPipelineState.drawIndexed((uint32_t)indices.size(), 0u, 0u);

	renderPass->execute(graphicsPipelineState);
}

void rRenderTest::renterToWindow()
{
	matrix world = m_Camera.getWorldMatrix();
	matrix view = m_Camera.getViewMatrix();
	matrix proj = m_Camera.getProjMatrix();

	uniformBufferVS uboVS
	{
		world,
		view,
		proj
	};
	m_Renderer->updateUniformBuffer(mUniformBuffer, &uboVS, sizeof(uniformBufferVS), 0u);
}

appMainEntry(rRenderTest)
