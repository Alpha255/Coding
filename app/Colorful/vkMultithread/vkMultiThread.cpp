#include "vkMultiThread.h"
///#include "Colorful/D3D11/D3D11Engine.h"
#include "Colorful/Vulkan/VulkanEngine.h"
#if 0
#include "Colorful/Vulkan/VulkanEngine.h"
#include "Colorful/Public/Geometry.h"
#include "Colorful/Public/Model.h"
#include "Colorful/Public/RObjectTest.h"

struct uniformBuffer_ufo
{
	matrix world;
	matrix view;
	matrix proj;
	vec4 color;
};

struct modelParams
{
	matrix world;
	vec3 pos;
	vec3 rotate;
	float32_t rotateDir;
	float32_t rotateSpeed;
	float32_t scale;
	float32_t delta;
	float32_t state;
};

struct ufoParams
{
	VulkanCommandBuffer CmdBuffer;
	std::vector<modelParams> ModelParams;
	std::vector<uniformBuffer_ufo> UniformBuffers;
};

typedef std::vector<ufoParams> threadParams;
bool8_t g_EnableMultiThread = false;

model ufo;
uint32_t g_ThreadCount = std::thread::hardware_concurrency();
uint32_t g_ufoCountPerThread = 512u / g_ThreadCount;
threadParams g_ThreadParams;

VulkanVertexShader VertexShader;
VulkanPixelShader PixelShader;
VulkanInputLayout InputLayout;

VulkanBuffer VertexBuffer;
VulkanBuffer IndexBuffer;
VulkanBuffer UniformBuffer;

void vkMultiThread::postInitialize()
{
#if 1
	//vkEngine engine_vk;
	//auto instancePtr = engine_vk.createInstance();

	
#endif

	taskScheduler::instance().initialize();

	VulkanEngine::Instance().Initialize(m_hWnd, (uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y, true);

	//g_ThreadParams.resize(g_ThreadCount);
	//for (uint32_t i = 0u; i < g_ThreadCount; ++i)
	//{
	//	g_ThreadParams[i].ModelParams.resize(g_ufoCountPerThread);
	//	g_ThreadParams[i].UniformBuffers.resize(g_ufoCountPerThread);
	//	g_ThreadParams[i].CmdBuffer = VulkanEngine::Instance().AllocCommandBuffer(VulkanCommandPool::eGeneral, VulkanCommandPool::eSecondary, g_ufoCountPerThread);
	//}

	VertexShader.Create("vkMultiThread.shader", "main");
	PixelShader.Create("vkMultiThread.shader", "main");

	ufo.createFromFile("retroufo_red_lowpoly.dae");

	auto &vertices = ufo.getVertices();
	VertexBuffer.CreateAsVertexBuffer(vertices.size() * sizeof(float32_t), eGpuReadCpuWrite, vertices.data());

	auto &indices = ufo.getIndices();
	IndexBuffer.CreateAsIndexBuffer(indices.size() * sizeof(uint32_t), eGpuReadCpuWrite, indices.data());

	InputLayout.create(ufo.getVertexLayouts());

	UniformBuffer.CreateAsUniformBuffer(sizeof(uniformBuffer_ufo), eGpuReadCpuWrite, nullptr);

	vec3 center = ufo.getAABB().getCenter();
	vec3 extents = ufo.getAABB().getExtents();
	vec3 min = center + vec3(-1.0f, -1.0f, 1.0f) * extents;
	vec3 max = center + vec3(1.0f, 1.0f, 1.0f) * extents;
	vec3 size = max - min;

	m_Camera.setProjParams(math::g_pi_div4, m_WindowSize.x / m_WindowSize.y, 0.1f, 500.0f);
	m_Camera.setViewParams(vec3(center.x + size.x * 2.0f, center.y + size.y * 2.0f, center.z + size.z * 2.0f), vec3(center.x, center.y, center.z));
	m_Camera.setScalers(0.01f, 15.0f);
}

void vkMultiThread::finalize()
{
	VertexBuffer.Destory();
	IndexBuffer.Destory();
	UniformBuffer.Destory();

	VertexShader.Destory();
	PixelShader.Destory();

	VulkanEngine::Instance().Finalize();

	taskScheduler::instance().finalize();
}

void vkMultiThread::resizeWindow()
{
	VulkanEngine::Instance().Resize((uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y);
}

void vkMultiThread::renterToWindow()
{
	/// Bind geometry
	/// Bind shaders
	uniformBuffer_ufo uniformBuffer{};
	uniformBuffer.world = matrix();
	uniformBuffer.view = m_Camera.getViewMatrix();
	uniformBuffer.proj = m_Camera.getProjMatrix();
	UniformBuffer.Update(&uniformBuffer, sizeof(uniformBuffer_ufo));

	VulkanEngine::Instance().ResetDefaultRenderSurfaces(Vec4(0.0f, 0.0f, 0.2f, 1.0f));
	VulkanEngine::Instance().SetViewport(VulkanViewport(0.0f, 0.0f, m_WindowSize.x, m_WindowSize.y));
	VulkanEngine::Instance().SetScissorRect(VulkanRect(0.0f, 0.0f, m_WindowSize.x, m_WindowSize.y));
	VulkanEngine::Instance().SetRasterizerState(VulkanStaticState::SolidNoneCulling);
	VulkanEngine::Instance().SetVertexShader(VertexShader);
	VulkanEngine::Instance().SetPixelShader(PixelShader);
	VulkanEngine::Instance().SetUniformBuffer(UniformBuffer, 0u, eVertexShader);
	VulkanEngine::Instance().SetInputLayout(InputLayout);
	VulkanEngine::Instance().SetVertexBuffer(VertexBuffer, ufo.getVertexStride(), 0u);
	VulkanEngine::Instance().SetIndexBuffer(IndexBuffer, eR32_UInt, 0u);
	VulkanEngine::Instance().DrawIndexed((uint32_t)ufo.getIndices().size(), 0u, 0, eTriangleList);
	VulkanEngine::Instance().Flush();

	m_Camera.update(m_Timer.getElapsedTime());
}
#else
void vkMultiThread::postInitialize()
{
	///d3d11Engine::instance().initialize(m_hWnd, (uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y, false);
	vkEngine::instance().initialize(m_hWnd, (uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y, false);
}

void vkMultiThread::finalize()
{

}

void vkMultiThread::resizeWindow()
{

}

void vkMultiThread::renterToWindow()
{

}
#endif

winMainEntry(vkMultiThread)
