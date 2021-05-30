#include "Colorful/Gfx/ImGui/ImGui.h"
#include "Colorful/Gfx/AssetTool/AssetTool.h"
#include <ThirdParty/ImGUI/imgui.h>
#include <ThirdParty/ImGUI/imgui_internal.h>

NAMESPACE_START(Gfx)

ImGuiRenderer::ImGuiRenderer()
{
	assert(GRenderer);

	IMGUI_CHECKVERSION();
	m_Context = ImGui::CreateContext();

	BlendStateDesc blendDesc;
	blendDesc.ColorBlends[0] = ColorBlendDesc
	{
		0u,
		true,
		EColorWriteMask::All,
		EBlendFactor::SrcAlpha,
		EBlendFactor::InverseSrcAlpha,
		EBlendOp::Add,

		EBlendFactor::InverseSrcAlpha,
		EBlendFactor::Zero,
		EBlendOp::Add,
	};

	DepthStencilStateDesc depthStencilDesc
	{
		true,
		false,
		false,
		false,
		ECompareFunc::LessOrEqual
	};

	m_PipelineState = create_shared<GraphicsPipelineState>(
		AssetTool::instance().loadMaterial("ImGui.xml"),
		RasterizationStateDesc(),
		MultisampleStateDesc(),
		depthStencilDesc,
		blendDesc);

	byte8_t* pixels = nullptr;
	int32_t width = 0;
	int32_t height = 0;
	m_Context->IO.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
	TextureDesc desc;
	desc.Name = "ImGuiFont";
	desc.Format = EFormat::RGBA8_UNorm;
	desc.Width = static_cast<uint32_t>(width);
	desc.Height = static_cast<uint32_t>(height);
	desc.DataSize = static_cast<size_t>(width) * static_cast<size_t>(height) * 4ull;
	desc.Data = pixels;
	desc.Subresources.push_back({
		desc.Width,
		desc.Height,
		desc.Layers,
		0u,
		width * 4u,
		static_cast<uint32_t>(desc.DataSize) });

	auto fontTexture = GRenderer->device()->create2DTexture(desc);
	fontTexture->bindSampler(GRenderer->device()->createSampler(SamplerDesc{}));
	m_PipelineState->setResource<EShaderStage::Fragment>(0u, fontTexture);
	m_PipelineState->setResource<EShaderStage::Vertex>(0u, GRenderer->device()->createUniformBuffer(sizeof(Vec4), nullptr));
}

bool8_t ImGuiRenderer::update()
{
	assert(m_Context);

	auto const drawData = ImGui::GetDrawData();

	if (drawData && drawData->CmdListsCount > 0)
	{
		if (m_VertexCount < drawData->TotalVtxCount)
		{
			m_VertexCount = drawData->TotalVtxCount;
			size_t VertexBufferSize = sizeof(ImDrawVert) * drawData->TotalVtxCount;
			if (m_VertexBuffer)
			{
				m_VertexBuffer.reset();
			}
			m_VertexBuffer = GRenderer->device()->createVertexBuffer(EBufferUsage::Dynamic, VertexBufferSize, nullptr);
		}
		if (m_IndexCount < drawData->TotalIdxCount)
		{
			m_IndexCount = drawData->TotalIdxCount;
			size_t IndexBufferSize = sizeof(ImDrawIdx) * drawData->TotalIdxCount;
			if (m_IndexBuffer)
			{
				m_IndexBuffer.reset();
			}
			m_IndexBuffer = GRenderer->device()->createIndexBuffer(EBufferUsage::Dynamic, IndexBufferSize, nullptr);
		}

		auto vBuffer = reinterpret_cast<ImDrawVert*>(m_VertexBuffer->map());
		auto iBuffer = reinterpret_cast<ImDrawIdx*>(m_IndexBuffer->map());
		assert(vBuffer && iBuffer);

		for (int32_t i = 0; i < drawData->CmdListsCount; ++i)
		{
			auto const drawList = drawData->CmdLists[i];

			auto size = drawList->VtxBuffer.Size * sizeof(ImDrawVert);
			VERIFY(memcpy_s(vBuffer, size, drawList->VtxBuffer.Data, size) == 0);
			vBuffer += drawList->VtxBuffer.Size;

			size = drawList->IdxBuffer.Size * sizeof(ImDrawIdx);
			VERIFY(memcpy_s(iBuffer, size, drawList->IdxBuffer.Data, size) == 0);
			iBuffer += drawList->IdxBuffer.Size;
		}

		m_VertexBuffer->flushMappedRange();
		m_IndexBuffer->flushMappedRange();

		return true;
	}

	return false;
}

void ImGuiRenderer::processInput(const InputState& inputState)
{
	assert(m_Context);
	m_Context->IO.MouseDown[0] = inputState.MouseState == EMouseState::LButtonDown;
	m_Context->IO.MouseDown[1] = inputState.MouseState == EMouseState::RButtonDown;
	m_Context->IO.MouseDown[2] = inputState.MouseState == EMouseState::MButtonDown;
	m_Context->IO.MousePos = ImVec2(inputState.MousePosition.x, inputState.MousePosition.y);
	m_Context->IO.MouseWheel += inputState.MouseWheelDelta > 0 ? 1.0f : -1.0f;

	/// Keyboard
}

void ImGuiRenderer::beginFrame(uint32_t width, uint32_t height)
{
	assert(m_Context);
	if (m_Enable)
	{
		m_Context->IO.DisplaySize = ImVec2(static_cast<float32_t>(width), static_cast<float32_t>(height));
		ImGui::NewFrame();
	}
}

void ImGuiRenderer::renderFrame()
{
	assert(m_Context);
	if (update())
	{
		auto cmdBuffer = GRenderer->device()->getActiveCommandBuffer();
		cmdBuffer->executeRenderPass(RenderPassDesc{ "ImGui" }, *m_PipelineState, [&]() 
		{
			auto drawData = ImGui::GetDrawData();
			assert(drawData);

			Vec4 translateScale(
				2.0f / m_Context->IO.DisplaySize.x,
				2.0f / m_Context->IO.DisplaySize.y,
				-1.0f,
				-1.0f);
			m_PipelineState->undateUniformBuffer<EShaderStage::Vertex>(
				0u,
				&translateScale,
				sizeof(Vec4));
			m_PipelineState->setViewport(
				Viewport
				{
					0.0f,
					0.0f,
					m_Context->IO.DisplaySize.x,
					m_Context->IO.DisplaySize.y
				});

			m_PipelineState->setVertexBuffer(m_VertexBuffer, 0u);
			m_PipelineState->setIndexBuffer(m_IndexBuffer, EIndexFormat::UInt16);

			for (int32_t i = 0, vOffset = 0, iOffset = 0; i < drawData->CmdListsCount; ++i)
			{
				auto const drawList = drawData->CmdLists[i];

				for (int32_t j = 0; j < drawList->CmdBuffer.Size; ++j)
				{
					auto const drawCmd = &drawList->CmdBuffer[j];

					m_PipelineState->setScissor(
						Scissor
						{
							std::max(drawCmd->ClipRect.x, 0.0f),
							std::max(drawCmd->ClipRect.y, 0.0f),
							drawCmd->ClipRect.z - drawCmd->ClipRect.x,
							drawCmd->ClipRect.w - drawCmd->ClipRect.y,
						});

					cmdBuffer->drawIndexed(drawCmd->ElemCount, static_cast<uint32_t>(iOffset), vOffset);
					iOffset += drawCmd->ElemCount;
				}
				vOffset += drawList->VtxBuffer.Size;
			}
		});
	}
}

void ImGuiRenderer::endFrame()
{
	assert(m_Context);
	if (m_Enable)
	{
		ImGui::Render();

		renderFrame();
	}
}

bool8_t ImGuiRenderer::hasFocus() const
{
	assert(m_Context);
	return m_Context->IO.WantCaptureMouse || m_Context->IO.WantCaptureKeyboard || m_Context->IO.WantTextInput;
}

ImGuiRenderer::~ImGuiRenderer()
{
	ImGui::DestroyContext();
}

NAMESPACE_END(Gfx)
