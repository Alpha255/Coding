#include "vkTest.h"
#include "Colorful/Vulkan/VulkanEngine.h"

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
	matrix proj;
	matrix view;
};

VulkanVertexShader vkVS;
VulkanPixelShader vkPS;

VulkanBuffer VertexBuffer;
VulkanBuffer IndexBuffer;
VulkanBuffer UniformBuffer;

void vkTest::postInitialize()
{
	VulkanEngine::Instance().Initialize(m_hWnd, (uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y, true);

	vkVS.Create("vkTest.shader", "main");
	vkPS.Create("vkTest.shader", "main");

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
	VertexBuffer.CreateAsVertexBuffer(vertices.size() * sizeof(vertex), eGpuReadOnly, vertices.data());
	IndexBuffer.CreateAsIndexBuffer(indices.size() * sizeof(uint32_t), eGpuReadOnly, indices.data());
	UniformBuffer.CreateAsUniformBuffer(sizeof(uniformBuffer_vs), eGpuReadCpuWrite, nullptr);
}

void vkTest::finalize()
{
	VulkanEngine::Instance().Finalize();
}

void vkTest::resizeWindow()
{
	VulkanEngine::Instance().Resize((uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y);
}

void vkTest::renterToWindow()
{
}
