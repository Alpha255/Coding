#include "rRenderTest.h"

void rRenderTest::postInitialize()
{
	auto vertexShader = m_Renderer->createVertexShader("rRenderTest.shader");
	auto fragmentShader = m_Renderer->createFragmentShader("rRenderTest.shader");

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

	auto depthStencilView = m_Renderer->createDepthStencilView(m_WindowSize.x, m_WindowSize.y, eD24_UNorm_S8_UInt);

	rFrameBufferDesc frameBufferDesc{};
	frameBufferDesc.DepthSurface = depthStencilView;
	frameBufferDesc.Width = (uint32_t)m_WindowSize.x;
	frameBufferDesc.Height = (uint32_t)m_WindowSize.y;

	auto renderPass = m_Renderer->createRenderPass(frameBufferDesc);
}

void rRenderTest::renterToWindow()
{
	/// RenderPass.create("Opaque");
	rGraphicsPipelineState graphicsPipelineState{};

	/*
	renderpass.begin(graphicsPipelineState)
	bindVertexBuffer
	bindIndexBuffer
	drawIndexed
	renderpass.end()
	*/
}

appMainEntry(rRenderTest)
