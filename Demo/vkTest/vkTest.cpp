#include "vkTest.h"
#include "Colorful/Vulkan/VulkanEngine.h"
#include "Colorful/Public/Geometry.h"
#include "Colorful/Public/Model.h"

// Static data like vertex and index buffer should be stored on the device memory 
// for optimal (and fastest) access by the GPU
//
// To achieve this we use so-called "staging buffers" :
// - Create a buffer that's visible to the host (and can be mapped)
// - Copy the data to this buffer
// - Create another buffer that's local on the device (VRAM) with the same size
// - Copy the data from the host to the device using a command buffer
// - Delete the host visible (staging) buffer
// - Use the device local buffers for rendering

// Don't use staging
// Create host-visible buffers only and use these for rendering. This is not advised and will usually result in lower rendering performance

struct vertex
{
	vec3 pos;
	vec3 color;
};

struct uniformBuffer_vs
{
	matrix world;
	matrix view;
	matrix proj;
};

VulkanVertexShader VertexShader;
VulkanPixelShader PixelShader;
VulkanInputLayout InputLayout;

VulkanBuffer VertexBuffer;
VulkanBuffer IndexBuffer;
VulkanBuffer UniformBuffer;

void vkTest::postInitialize()
{
	VulkanEngine::Instance().Initialize(m_hWnd, (uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y, true);

	VertexShader.Create("vkTest.shader", "main");
	PixelShader.Create("vkTest.shader", "main");

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

	/// Don't use staging
	VertexBuffer.CreateAsVertexBuffer(vertices.size() * sizeof(vertex), eGpuReadCpuWrite, vertices.data());
	IndexBuffer.CreateAsIndexBuffer(indices.size() * sizeof(uint32_t), eGpuReadCpuWrite, indices.data());
	UniformBuffer.CreateAsUniformBuffer(sizeof(uniformBuffer_vs), eGpuReadCpuWrite, nullptr);

	std::vector<Geometry::VertexLayout> vertexLayout =
	{
		{ "POSITION", sizeof(vertex::pos),   offsetof(vertex, pos),   eRGB32_Float },
		{ "COLOR",    sizeof(vertex::color), offsetof(vertex, color), eRGB32_Float },
	};
	InputLayout.Create(VertexShader.GetBlob(), vertexLayout);

	m_Camera.setProjParams(math::g_pi_div4, m_WindowSize.x / m_WindowSize.y, 0.1f, 500.0f);
	m_Camera.setViewParams(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f));
	m_Camera.setScalers(0.01f, 15.0f);
}

void vkTest::finalize()
{
	VertexBuffer.Destory();
	IndexBuffer.Destory();
	UniformBuffer.Destory();

	VertexShader.Destory();
	PixelShader.Destory();

	VulkanEngine::Instance().Finalize();
}

void vkTest::resizeWindow()
{
	VulkanEngine::Instance().Resize((uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y);
}

void vkTest::renterToWindow()
{
	/// Bind geometry
	/// Bind shaders
	uniformBuffer_vs uniformBuffer{};
	uniformBuffer.world = matrix();
	uniformBuffer.view = m_Camera.getViewMatrix();
	uniformBuffer.proj = m_Camera.getProjMatrix();
	UniformBuffer.Update(&uniformBuffer, sizeof(uniformBuffer_vs));

	VulkanEngine::Instance().ResetDefaultRenderSurfaces(Vec4(0.0f, 0.0f, 0.2f, 1.0f));
	VulkanEngine::Instance().SetViewport(VulkanViewport(0.0f, 0.0f, m_WindowSize.x, m_WindowSize.y));
	VulkanEngine::Instance().SetScissorRect(VulkanRect(0.0f, 0.0f, m_WindowSize.x, m_WindowSize.y));
	VulkanEngine::Instance().SetRasterizerState(VulkanStaticState::SolidNoneCulling);
	VulkanEngine::Instance().SetVertexShader(VertexShader);
	VulkanEngine::Instance().SetPixelShader(PixelShader);
	VulkanEngine::Instance().SetUniformBuffer(UniformBuffer, 0u, eVertexShader);
	VulkanEngine::Instance().SetInputLayout(InputLayout);
	VulkanEngine::Instance().SetVertexBuffer(VertexBuffer, sizeof(vertex), 0u);
	VulkanEngine::Instance().SetIndexBuffer(IndexBuffer, eR32_UInt, 0u);
	VulkanEngine::Instance().DrawIndexed(3u, 0u, 0, eTriangleList);
	VulkanEngine::Instance().Flush();

	m_Camera.update(m_Timer.getElapsedTime());
}
