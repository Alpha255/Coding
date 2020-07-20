#include "RenderTest.h"

#if 0
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

void RenderTest::postInitialize()
{
	auto texture = g_GfxEngine->createTexture("metalplate01_rgba.ktx");

	m_UniformBufferVS = g_GfxEngine->createUniformBuffer(sizeof(UniformBufferVS), nullptr);

	std::vector<Vertex> vertices
	{
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
	std::vector<uint32_t> indices
	{
		0u, 1u, 2u, 2u, 3u, 0u
	};

	m_PipelineState.VertexBuffer = g_GfxEngine->createVertexBuffer(eGpuReadWrite, vertices.size() * sizeof(Vertex), vertices.data());
	m_PipelineState.IndexBuffer = g_GfxEngine->createIndexBuffer(eGpuReadWrite, indices.size() * sizeof(uint32_t), indices.data());
	m_PipelineState.setMaterial("Texture_Test.mat");
	m_PipelineState.setCombinedTextureSampler(eFragmentShader, texture, GfxFactory::instance()->linearSampler(), 1u);
	m_PipelineState.setUniformBuffer(eVertexShader, m_UniformBufferVS, 0u);

	m_Camera.setPerspective(Math::PI_Div4, (float32_t)m_Window->width() / m_Window->height(), 0.1f, 500.0f);
	m_Camera.setView(Vec3(0.0f, 0.0f, 4.0f), Vec3(0.0f, 0.0f, 0.0f));
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

	m_PipelineState.setFrameBuffer(g_GfxEngine->backBuffer());
	m_PipelineState.setViewport(viewport);
	m_PipelineState.setScissor(scissor);
	g_GfxEngine->bindGfxPipelineState(&m_PipelineState);

	GfxScopeGpuMarker(DrawOpaque, Color::randomColor());
	g_GfxEngine->drawIndexed(6u, 1u, 0u, 0);

	ImGui::Checkbox("Wireframe", &m_PipelineState.Wireframe);
	ImGui::Text("Mouse pos x: %.2f, y: %.2f", m_Window->message().Mouse.Pos.x, m_Window->message().Mouse.Pos.y);
	ImGui::Text("Mouse wheel delta: %d", m_Window->message().Mouse.WheelDelta);
	ImGui::Text("FrameTime: %.2f ms, FPS: %.2f", m_Profile.FrameTime, m_Profile.FPS);
}
#endif

#if 0
struct Vertex
{
	Vec3 Position;
	Vec2 UV;
};

struct UniformBufferVS
{
	Matrix Projection;
	Matrix View;

	struct
	{
		Matrix World;
		Vec4 ArrayIndex;
	}Instance[7];
};

void RenderTest::postInitialize()
{
	auto texture = g_GfxEngine->createTexture("texturearray_bc3_unorm.ktx");

	std::vector<Vertex> vertices =
	{
		{ { -1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f } },
		{ {  1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f } },
		{ {  1.0f,  1.0f,  1.0f }, { 1.0f, 1.0f } },
		{ { -1.0f,  1.0f,  1.0f }, { 0.0f, 1.0f } },

		{ {  1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f } },
		{ {  1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f } },
		{ {  1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f } },
		{ {  1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f } },

		{ { -1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f } },
		{ {  1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f } },
		{ {  1.0f,  1.0f, -1.0f }, { 1.0f, 1.0f } },
		{ { -1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f } },

		{ { -1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f } },
		{ { -1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f } },
		{ { -1.0f,  1.0f,  1.0f }, { 1.0f, 1.0f } },
		{ { -1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f } },

		{ {  1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f } },
		{ { -1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f } },
		{ { -1.0f,  1.0f, -1.0f }, { 1.0f, 1.0f } },
		{ {  1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f } },

		{ { -1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f } },
		{ {  1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f } },
		{ {  1.0f, -1.0f,  1.0f }, { 1.0f, 1.0f } },
		{ { -1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f } },
	};
	std::vector<uint32_t> indices = 
	{
		0,1,2, 0,2,3, 4,5,6,  4,6,7, 8,9,10, 8,10,11, 12,13,14, 12,14,15, 16,17,18, 16,18,19, 20,21,22, 20,22,23
	};
	m_PipelineState.VertexBuffer = g_GfxEngine->createVertexBuffer(eGpuReadWrite, vertices.size() * sizeof(Vertex), vertices.data());
	m_PipelineState.IndexBuffer = g_GfxEngine->createIndexBuffer(eGpuReadWrite, indices.size() * sizeof(uint32_t), indices.data());
	m_UniformBufferVS = g_GfxEngine->createUniformBuffer(sizeof(UniformBufferVS), nullptr);

	m_PipelineState.setMaterial("TextureArray_Test.mat");
	GfxDepthStencilStateDesc depthStencilState
	{
		true,
		true,
		eRCompareOp::eLessOrEqual
	};
	m_PipelineState.setDepthStencilState(depthStencilState);
	m_PipelineState.setUniformBuffer(eVertexShader, m_UniformBufferVS, 0u);
	m_PipelineState.setCombinedTextureSampler(eFragmentShader, texture, GfxFactory::instance()->linearSampler(), 1u);

	m_Camera.setPerspective(Math::PI_Div4, (float32_t)m_Window->width() / m_Window->height(), 0.1f, 500.0f);
	m_Camera.setView(Vec3(0.7f, 0.0f, 8.0f), Vec3(0.0f, 0.0f, 0.0f));
}

void RenderTest::renderFrame()
{
	UniformBufferVS uboVS;
	float32_t offset = -1.5f;
	float32_t center = (7 * offset) / 2.0f - (offset * 0.5f);
	for (uint32_t i = 0u; i < 7; ++i)
	{
		uboVS.Projection = m_Camera.projectionMatrix();
		uboVS.View = m_Camera.viewMatrix();
		uboVS.Instance[i].ArrayIndex = Vec4(i * 1.0f);
		uboVS.Instance[i].World = Matrix::scale(0.5f);
		uboVS.Instance[i].World *= Matrix::translate(Vec3(i * offset - center, 0.0f, 0.0f));
	}
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

	m_PipelineState.setFrameBuffer(g_GfxEngine->backBuffer());
	m_PipelineState.setViewport(viewport);
	m_PipelineState.setScissor(scissor);
	g_GfxEngine->bindGfxPipelineState(&m_PipelineState);

	GfxScopeGpuMarker(DrawOpaque, Color::randomColor());
	g_GfxEngine->drawIndexed(36u, 7u, 0u, 0);

	ImGui::Text("FrameTime: %.2f ms, FPS: %.2f", m_Profile.FrameTime, m_Profile.FPS);
}
#endif

#if 1
GfxPipelineState s_Color;
GfxPipelineState s_Outline;

struct UniformBuffer
{
	Matrix Projection;
	Matrix View;
	Vec4 LightPos;
	float32_t OutlineWidth;
};

void RenderTest::postInitialize()
{
	m_Model.load("venus.fbx");

	m_UniformBufferVS = g_GfxEngine->createUniformBuffer(sizeof(UniformBuffer), nullptr);

	s_Color.setMaterial("StencilBuffer_Model.mat");
	s_Color.setUniformBuffer(eVertexShader, m_UniformBufferVS, 0u);
	s_Color.DepthStencilStateDesc = GfxDepthStencilStateDesc
	{
		true,
		true,
		eRCompareOp::eLessOrEqual,
		true,
		0xFF,
		0xFF,
		{
			eRStencilOp::eReplace,
			eRStencilOp::eReplace,
			eRStencilOp::eReplace,
			eRCompareOp::eAlways,
			1u
		},
		{
			eRStencilOp::eReplace,
			eRStencilOp::eReplace,
			eRStencilOp::eReplace,
			eRCompareOp::eAlways,
			1u
		}
	};

	s_Outline.setMaterial("StencilBuffer_Outline.mat");
	s_Outline.setUniformBuffer(eVertexShader, m_UniformBufferVS, 0u);
	s_Outline.DepthStencilStateDesc = GfxDepthStencilStateDesc
	{
		false,
		true,
		eRCompareOp::eLessOrEqual,
		true, 
		0xFF,
		0xFF,
		{
			eRStencilOp::eKeep,
			eRStencilOp::eReplace,
			eRStencilOp::eKeep,
			eRCompareOp::eNotEqual,
			1u
		},
		{
			eRStencilOp::eKeep,
			eRStencilOp::eReplace,
			eRStencilOp::eKeep,
			eRCompareOp::eNotEqual,
			1u
		}
	};

	m_Camera.setPosition(Vec3(0.08f, 4.5f, -12.0f), Vec3(90.0f, 0.0f, 145.0f));
	m_Camera.setPerspective(60.0f, (float32_t)m_Window->width() / m_Window->height(), 0.1f, 500.0f);
}

void RenderTest::renderFrame()
{
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
	s_Color.Viewport = viewport;
	s_Color.Scissor = scissor;
	s_Color.FrameBuffer = g_GfxEngine->backBuffer();

	s_Outline.Viewport = viewport;
	s_Outline.Scissor = scissor;
	s_Outline.FrameBuffer = g_GfxEngine->backBuffer();

	static float32_t s_OutlineWidth = 0.05f;
	UniformBuffer ubo
	{
		m_Camera.projectionMatrix(),
		m_Camera.viewMatrix(),
		Vec4(0.0f, -2.0f, 1.0f, 0.0f),
		s_OutlineWidth
	};
	m_UniformBufferVS->update(&ubo, sizeof(UniformBuffer), 0u);
	
	m_Model.draw(&s_Color);
	m_Model.draw(&s_Outline);

	ImGui::Text("FrameTime: %.2f ms, FPS: %.2f", m_Profile.FrameTime, m_Profile.FPS);
	ImGui::SliderFloat("Outline Width: ", &s_OutlineWidth, 0.01f, 0.05f, "%.2f");

	static Vec3 Trans{ 0.08f, 4.5f, -12.0f };
	static Vec3 Rot{ 90.0f, 0.0f, 145.0f };

	ImGui::SliderFloat3("Trans:", (float32_t*)(&Trans), -0.1f, 20.0f);
	ImGui::SliderFloat3("Rot:", (float32_t*)(&Rot), -360.0f, 360.0f);
	m_Camera.setPosition(Trans, Rot);
}
#endif

appMainEntry(RenderTest)
