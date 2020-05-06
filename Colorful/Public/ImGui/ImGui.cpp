#include "ImGui.h"

void ImGuiRenderer::initialize(uint64_t windowHandle, GfxEngine *renderEngine)
{
	assert(windowHandle && renderEngine);

	m_Renderer = renderEngine;
	m_WindowHandle = windowHandle;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	//io.KeyMap[ImGuiKey_Tab] = VK_TAB;
	//io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
	//io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
	//io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
	//io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
	//io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
	//io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
	//io.KeyMap[ImGuiKey_Home] = VK_HOME;
	//io.KeyMap[ImGuiKey_End] = VK_END;
	//io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
	//io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
	//io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
	//io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
	//io.KeyMap[ImGuiKey_A] = 'A';
	//io.KeyMap[ImGuiKey_C] = 'C';
	//io.KeyMap[ImGuiKey_V] = 'V';
	//io.KeyMap[ImGuiKey_X] = 'X';
	//io.KeyMap[ImGuiKey_Y] = 'Y';
	//io.KeyMap[ImGuiKey_Z] = 'Z';
	//io.RenderDrawListsFn = nullptr;
	//io.ImeWindowHandle = reinterpret_cast<void *>(windowHandle);

	byte8_t *pixels = nullptr;
	int32_t width = 0;
	int32_t height = 0;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
	auto fontTex = m_Renderer->createTexture(
		eTexture2D,
		eRGBA8_UNorm,
		width,
		height,
		1u,
		1u,
		pixels,
		width * height * 4ull
	);

	auto vertexShader = m_Renderer->createVertexShader("ImGui.shader");
	auto fragmentShader = m_Renderer->createFragmentShader("ImGui.shader");

	std::vector<rVertexAttributes> vertexAttrs
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
	m_UniformBuffer = m_Renderer->createUniformBuffer(sizeof(UniformBuffer), nullptr);
	vertexShader->setUniformBuffer(m_UniformBuffer);

	GfxSamplerDesc samplerDesc{};
	auto sampler = m_Renderer->createSampler(samplerDesc);
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

	///m_Renderer->getOpaqueRenderPass()->pendingGfxPipline(m_PipelineState);
}

void ImGuiRenderer::processEvent()
{
	if (!m_WindowHandle)
	{
		return;
	}

	//ImGuiIO &io = ImGui::GetIO();
	//auto &eventer = SystemEventHandler::instance();

	//auto keyEvent = eventer.getKeyboardEvent();
	//auto mouseEvent = eventer.getMouseEvent();

	//int32_t mouseBtn = -1;
	//if (eMouseEvent::eLeftClick == mouseEvent || eMouseEvent::eLeftUp == mouseEvent)
	//{
	//	mouseBtn = 0;
	//}
	//if (eMouseEvent::eRightClick == mouseEvent || eMouseEvent::eRightClick == mouseEvent)
	//{
	//	mouseBtn = 1;
	//}
	//if (eMouseEvent::eMiddleClick == mouseEvent || eMouseEvent::eMiddleUp == mouseEvent)
	//{
	//	mouseBtn = 2;
	//}

	/////bool8_t isFocus = SystemEventHandler::isFocusWindow(m_WindowHandle);

	//switch (mouseEvent)
	//{
	//case eMouseEvent::eLeftClick:
	//case eMouseEvent::eRightClick:
	//case eMouseEvent::eMiddleClick:
	//	//if (!isMouseButtonDown(io) && !isFocus)
	//	//{
	//	//	SystemEventHandler::focusWindow(m_WindowHandle);
	//	//}
	//	io.MouseDown[mouseBtn] = true;
	//	return;

	//case eMouseEvent::eLeftUp:
	//case eMouseEvent::eRightUp:
	//case eMouseEvent::eMiddleUp:
	//	io.MouseDown[mouseBtn] = false;
	//	//if (!isMouseButtonDown(io) && isFocus)
	//	//{
	//	//	SystemEventHandler::unFocusWindow();
	//	//}
	//	return;

	//case eMouseEvent::eMove:
	//{
	//	auto mousePos = eventer.getMousePosition();
	//	io.MousePos = ImVec2
	//	{
	//		mousePos.x,
	//		mousePos.y
	//	};
	//	return;
	//}

	//case eMouseEvent::eWheel:
	//{
	//	auto mouseWheelDelta = eventer.getMouseWheelDelta();
	//	io.MouseWheel += mouseWheelDelta > 0.0f ? 1.0f : -1.0f;
	//	return;
	//}
	//}

	//switch (keyEvent)
	//{
	//case eKeyboardEvent::eKeyDown:
	//	return;
	//case eKeyboardEvent::eKeyUp:
	//	return;
	//}
}

void ImGuiRenderer::begin(float32_t displayWidth, float32_t displayHeight)
{
	ImGuiIO &io = ImGui::GetIO();
	
	io.DisplaySize = ImVec2
	{
		displayWidth,
		displayHeight
	};

	io.KeyCtrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
	io.KeyShift = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
	io.KeyAlt = (::GetKeyState(VK_MENU) & 0x8000) != 0;
	io.KeySuper = false;

	if (io.WantCaptureMouse)
	{
		::POINT pos = { (LONG)io.MousePos.x, (LONG)io.MousePos.y };
		::ClientToScreen((::HWND)m_WindowHandle, &pos);
		::SetCursorPos(pos.x, pos.y);
	}

	if (io.MouseDrawCursor)
	{
		::SetCursor(nullptr);
	}

	ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_FirstUseEver);

	ImGui::NewFrame();

	ImGui::Begin("ImGui");
}

void ImGuiRenderer::end()
{
	ImGui::End();
	ImGui::Render();

	frame();
}

bool8_t ImGuiRenderer::isMouseButtonDown(ImGuiIO &io)
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
	m_Renderer->updateGpuBuffer(m_UniformBuffer, &uniformBuffer, sizeof(UniformBuffer), 0u);

	rViewport viewport
	{
		0.0f,
		0.0f,
		io.DisplaySize.x,
		io.DisplaySize.y
	};
	m_PipelineState.setViewport(viewport);

	m_Renderer->getOpaqueRenderPass()->bindGfxPipeline(m_PipelineState);

	auto drawData = ImGui::GetDrawData();
	assert(drawData);
	for (int32_t i = 0, vertexOffset = 0, indexOffset = 0; i < drawData->CmdListsCount; ++i)
	{
		const ImDrawList *drawList = drawData->CmdLists[i];
		for (int32_t j = 0; j < drawList->CmdBuffer.Size; ++j)
		{
			const ImDrawCmd *drawCmd = &drawList->CmdBuffer[j];

			rScissor scissor
			{
				std::max(drawCmd->ClipRect.x, 0.0f),
				std::max(drawCmd->ClipRect.y, 0.0f),
				drawCmd->ClipRect.z - drawCmd->ClipRect.x,
				drawCmd->ClipRect.w - drawCmd->ClipRect.y,
			};

			m_PipelineState.setScissor(scissor);
			m_Renderer->getOpaqueRenderPass()->drawIndexed(m_PipelineState, drawCmd->ElemCount, (uint32_t)indexOffset, vertexOffset);
			indexOffset += drawCmd->ElemCount;
		}
		vertexOffset += drawList->VtxBuffer.Size;
	}
}

bool8_t ImGuiRenderer::update()
{
	ImDrawData *drawData = ImGui::GetDrawData();
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
		m_Renderer->destroyBuffer(m_PipelineState.VertexBuffer);
	}
	if (m_PipelineState.IndexBuffer)
	{
		m_Renderer->destroyBuffer(m_PipelineState.IndexBuffer);
	}

	auto verties = std::make_unique<ImDrawVert[]>(drawData->TotalVtxCount);
	auto indices = std::make_unique<ImDrawIdx[]>(drawData->TotalIdxCount);

	auto vertexBuffer = m_Renderer->createVertexBuffer(eGpuReadCpuWrite, totalVertexBufferSize, nullptr);
	auto indexBuffer = m_Renderer->createIndexBuffer(eGpuReadCpuWrite, totalIndexBufferSize, nullptr);

	for (int32_t i = 0, vertexOffset = 0, indexOffset = 0; i < drawData->CmdListsCount; ++i)
	{
		const ImDrawList *drawList = drawData->CmdLists[i];

		auto vertexBufferSize = drawList->VtxBuffer.Size * sizeof(ImDrawVert);
		verify(memcpy_s(verties.get() + vertexOffset, vertexBufferSize, drawList->VtxBuffer.Data, vertexBufferSize) == 0);
		vertexOffset += drawList->VtxBuffer.Size;

		auto indexBufferSize = drawList->IdxBuffer.Size * sizeof(ImDrawIdx);
		verify(memcpy_s(indices.get() + indexOffset, indexBufferSize, drawList->IdxBuffer.Data, indexBufferSize) == 0);
		indexOffset += drawList->IdxBuffer.Size;
	}

	m_Renderer->updateGpuBuffer(vertexBuffer, verties.get(), totalVertexBufferSize, 0u);
	m_Renderer->updateGpuBuffer(indexBuffer, indices.get(), totalIndexBufferSize, 0u);

	m_PipelineState.bindVertexBuffer(vertexBuffer);
	m_PipelineState.bindIndexBuffer(indexBuffer, eRIndexType::eUInt16);

	return true;
}
