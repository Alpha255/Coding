#include "Box.h"
#include "Public/Definitions.h"
#include "Public/ImGUI.h"

void Box::PrepareScene()
{
	///m_BoxMesh.CreateAsCube(1.0f);

	///m_DiffuseTex.Create("WoodCrate01.dds");

	///m_VertexShader.Create("Box.shader", "main");
	///m_PixelShader[eNone].Create("Box.shader", "main");
#if 0
	m_PixelShader[eInversion].Create("Box.shader", "main_Inversion");
	m_PixelShader[eGrayscale].Create("Box.shader", "main_Grayscale");
	m_PixelShader[eSharpen].Create("Box.shader", "main_Sharpen");
	m_PixelShader[eBlur].Create("Box.shader", "main_Blur");
	m_PixelShader[eEdgeDetection].Create("Box.shader", "main_EdgeDetection");
#endif

	///m_CBufferVS.CreateAsConstantBuffer(sizeof(Matrix), eGpuReadCpuWrite, nullptr);

	///m_Camera.SetViewRadius(5.0f);

#if 1
	///m_CmdBuffer = VulkanEngine::Instance().AllocCommandBuffer(VulkanCommandPool::eGeneral, VulkanCommandPool::eSecondary);
	///m_Context.Create(m_VertexShader, m_PixelShader[eNone], m_BoxMesh.InputLayout);
#endif
}

void Box::RenderScene()
{
	REngine::Instance().ResetDefaultRenderSurfaces();
#if 0
	REngine::Instance().SetViewport(RViewport(0.0f, 0.0f, (float)m_Width, (float)m_Height));

	REngine::Instance().SetInputLayout(m_BoxMesh.InputLayout);
	REngine::Instance().SetVertexShader(m_VertexShader);
	REngine::Instance().SetPixelShader(m_PixelShader[m_Effect]);

	REngine::Instance().SetVertexBuffer(m_BoxMesh.VertexBuffer, sizeof(Geometry::Vertex), 0U);
	REngine::Instance().SetIndexBuffer(m_BoxMesh.IndexBuffer, eR32_UInt, 0U);

	Matrix wvp = Matrix::Transpose(m_Camera.GetWVPMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));
	REngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, eVertexShader);

	REngine::Instance().SetShaderResourceView(m_DiffuseTex, 0U, ePixelShader);
	REngine::Instance().SetSamplerState(RStaticState::LinearSampler, 0U, ePixelShader);

	REngine::Instance().DrawIndexed(m_BoxMesh.IndexCount, 0U, 0, eTriangleList);
#endif

	///ImGui::Combo("SpecialEffect", &m_Effect, "None\0Inversion\0Grayscale\0Sharpen\0Blur\0EdgeDetection");
	ImGui::Checkbox("VSync", &m_bVSync);
	ImGui::Text("\n%.2f FPS", m_FPS);
	ImGui::Text("VulkanTest");
#if 1

#endif
}

#if 0
void AppBox::UpdateScene(float, float)
{
	static bool s_Update = false;

	if (s_Update)
	{
		return;
	}

	VKCheck(vkDeviceWaitIdle(VulkanEngine::Instance().GetDevice()));

	VkCommandBufferInheritanceInfo inherit = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO };
	inherit.framebuffer = VK_NULL_HANDLE;
	inherit.renderPass = VulkanEngine::Instance().GetDefaultRenderPass().Get();

	VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	beginInfo.pInheritanceInfo = &inherit;

	VKCheck(vkBeginCommandBuffer(m_CmdBuffer.Get(), &beginInfo));

	vkCmdBindPipeline(m_CmdBuffer.Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_Context.Get());

	VulkanViewport vp((float)m_Width, (float)m_Height);
	vkCmdSetViewport(m_CmdBuffer.Get(), 0U, 1U, &vp);

	VulkanRect rect;
	rect.offset = {};
	rect.extent = { m_Width, m_Height };
	vkCmdSetScissor(m_CmdBuffer.Get(), 0U, 1U, &rect);

	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(m_CmdBuffer.Get(), 0U, 1U, &m_BoxMesh.VertexBuffer, offsets);
	vkCmdBindIndexBuffer(m_CmdBuffer.Get(), m_BoxMesh.IndexBuffer.Get(), 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(m_CmdBuffer.Get(), m_BoxMesh.IndexCount, 1U, 0U, 0U, 0U);

	m_CmdBuffer.End();

	s_Update = true;
}
#endif