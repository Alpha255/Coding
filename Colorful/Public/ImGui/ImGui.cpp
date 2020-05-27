#include "Colorful/Public/ImGui/ImGui.h"
#include "Colorful/Public/GfxEngine.h"
#include "Gear/Window.h"

ImGuiRenderer::ImGuiRenderer(GfxEngine* gfxEngine)
	: m_GfxEngine(gfxEngine)
{
	assert(gfxEngine);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	byte8_t* pixels = nullptr;
	int32_t width = 0;
	int32_t height = 0;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
	auto fontTex = m_GfxEngine->createTexture(
		eTexture2D,
		eRGBA8_UNorm,
		width,
		height,
		1u,
		1u,
		1u,
		pixels,
		width * height * 4ull
	);

	auto vertexShader = m_GfxEngine->createVertexShader("ImGui.shader");
	auto fragmentShader = m_GfxEngine->createFragmentShader("ImGui.shader");

	std::vector<GfxVertexAttributes> vertexAttrs
	{
		{
			ePosition,
			eRG32_Float,
			eVertex
		},
		{
			eTexcoord,
			eRG32_Float,
			eVertex
		},
		{
			eColor,
			eRGBA8_UNorm,
			eVertex
		}
	};
	vertexShader->setInputLayout(vertexAttrs, alignof(ImDrawVert));
	m_UniformBuffer = m_GfxEngine->createUniformBuffer(sizeof(UniformBuffer), nullptr);
	vertexShader->bindUniformBuffer(m_UniformBuffer);

	GfxSamplerDesc samplerDesc{};
	auto sampler = m_GfxEngine->createSampler(samplerDesc);
	fontTex->bindSampler(sampler);
	fragmentShader->bindTexture(fontTex);

	m_PipelineState.setShader(vertexShader);
	m_PipelineState.setShader(fragmentShader);

	GfxBlendStateDesc blendDesc{};
	blendDesc.ColorBlendStates[0] = GfxBlendStateDesc::ColorBlendState
	{
		true,
		eRBlendFactor::eSrcAlpha,
		eRBlendFactor::eInverseSrcAlpha,
		eRBlendOp::eAdd,
		eRBlendFactor::eInverseSrcAlpha,
		eRBlendFactor::eZero,
		eRBlendOp::eAdd,
		eRColorWriteMask::eColorAll
	};
	m_PipelineState.setBlendState(blendDesc);

	GfxDepthStencilStateDesc depthStencilDesc
	{
		false,
		false,
		eRCompareOp::eLessOrEqual
	};
	m_PipelineState.setDepthStencilState(depthStencilDesc);
}

void ImGuiRenderer::processMessage(const WindowMessage& message, uint32_t width, uint32_t height)
{
	ImGuiIO& io = ImGui::GetIO();

	io.DisplaySize = ImVec2
	{
		(float32_t)width,
		(float32_t)height
	};

	io.MouseDown[0] = message.Mouse.Left.KeyDown;
	io.MouseDown[1] = message.Mouse.Right.KeyDown;
	io.MouseDown[2] = message.Mouse.Middle.KeyDown;
	io.MousePos = ImVec2(message.Mouse.Pos.x, message.Mouse.Pos.y);
	io.MouseWheel += message.Mouse.WheelDelta > 0.0f ? 1.0f : -1.0f;

#if 0
	if (io.MouseDown[0])
	{
		Logger::instance().log(Logger::eInfo, "ImGui:: Mouse LButton Down.");
	}
	if (io.MouseDown[1])
	{
		Logger::instance().log(Logger::eInfo, "ImGui:: Mouse RButton Down.");
	}
	if (io.MouseDown[2])
	{
		Logger::instance().log(Logger::eInfo, "ImGui:: Mouse MButton Down.");
	}
#endif

	/// Keyboard
}

void ImGuiRenderer::begin()
{
	if (m_Enable)
	{
		ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_FirstUseEver);

		ImGui::NewFrame();

		ImGui::Begin("ImGui");
	}
}

void ImGuiRenderer::end()
{
	if (m_Enable)
	{
		ImGui::End();
		ImGui::Render();

		frame();
	}
}

bool8_t ImGuiRenderer::isMouseButtonDown(ImGuiIO& io)
{
	for (uint32_t i = 0u; i < ARRAYSIZE(io.MouseDown); ++i)
	{
		if (io.MouseDown[i])
		{
			return true;
		}
	}

	return false;
}

void ImGuiRenderer::frame()
{
	if (!update())
	{
		return;
	}

	ImGuiIO& io = ImGui::GetIO();

	UniformBuffer uniformBuffer;
	uniformBuffer.ScaleTranslate = Vec4(
		2.0f / io.DisplaySize.x, 
		2.0f / io.DisplaySize.y,
		-1.0f,
		-1.0f);
	m_UniformBuffer->update(&uniformBuffer, sizeof(UniformBuffer), 0u);

	GfxViewport viewport
	{
		0.0f,
		0.0f,
		io.DisplaySize.x,
		io.DisplaySize.y
	};
	m_PipelineState.setViewport(viewport);

	m_GfxEngine->opaqueRenderPass()->bindGfxPipeline(m_PipelineState);

	auto drawData = ImGui::GetDrawData();
	assert(drawData);
	for (int32_t i = 0, vertexOffset = 0, indexOffset = 0; i < drawData->CmdListsCount; ++i)
	{
		const ImDrawList* drawList = drawData->CmdLists[i];
		for (int32_t j = 0; j < drawList->CmdBuffer.Size; ++j)
		{
			const ImDrawCmd* drawCmd = &drawList->CmdBuffer[j];

			GfxScissor scissor
			{
				std::max(drawCmd->ClipRect.x, 0.0f),
				std::max(drawCmd->ClipRect.y, 0.0f),
				drawCmd->ClipRect.z - drawCmd->ClipRect.x,
				drawCmd->ClipRect.w - drawCmd->ClipRect.y,
			};

			m_PipelineState.setScissor(scissor);
			m_GfxEngine->opaqueRenderPass()->drawIndexed(drawCmd->ElemCount, (uint32_t)indexOffset, vertexOffset);
			indexOffset += drawCmd->ElemCount;
		}
		vertexOffset += drawList->VtxBuffer.Size;
	}
}

bool8_t ImGuiRenderer::update()
{
	ImDrawData* drawData = ImGui::GetDrawData();
	if (!drawData || drawData->CmdListsCount == 0U)
	{
		return false;
	}

	size_t totalVertexBufferSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
	size_t totalIndexBufferSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);

	if (0u == totalVertexBufferSize || 0u == totalIndexBufferSize)
	{
		return false;
	}

	if (m_PipelineState.VertexBuffer)
	{
		m_PipelineState.VertexBuffer->free();
	}
	if (m_PipelineState.IndexBuffer)
	{
		m_PipelineState.IndexBuffer->free();
	}

	auto verties = std::make_unique<ImDrawVert[]>(drawData->TotalVtxCount);
	auto indices = std::make_unique<ImDrawIdx[]>(drawData->TotalIdxCount);

	auto vertexBuffer = m_GfxEngine->createVertexBuffer(eGpuReadCpuWrite, totalVertexBufferSize, nullptr);
	auto indexBuffer = m_GfxEngine->createIndexBuffer(eGpuReadCpuWrite, totalIndexBufferSize, nullptr);

	for (int32_t i = 0, vertexOffset = 0, indexOffset = 0; i < drawData->CmdListsCount; ++i)
	{
		const ImDrawList* drawList = drawData->CmdLists[i];

		auto vertexBufferSize = drawList->VtxBuffer.Size * sizeof(ImDrawVert);
		verify(memcpy_s(verties.get() + vertexOffset, vertexBufferSize, drawList->VtxBuffer.Data, vertexBufferSize) == 0);
		vertexOffset += drawList->VtxBuffer.Size;

		auto indexBufferSize = drawList->IdxBuffer.Size * sizeof(ImDrawIdx);
		verify(memcpy_s(indices.get() + indexOffset, indexBufferSize, drawList->IdxBuffer.Data, indexBufferSize) == 0);
		indexOffset += drawList->IdxBuffer.Size;
	}

	vertexBuffer->update(verties.get(), totalVertexBufferSize, 0u);
	indexBuffer->update(indices.get(), totalIndexBufferSize, 0u);

	m_PipelineState.bindVertexBuffer(vertexBuffer);
	m_PipelineState.bindIndexBuffer(indexBuffer, eRIndexType::eUInt16);

	return true;
}
