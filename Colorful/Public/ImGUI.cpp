#include "ImGUI.h"
#include "Geometry.h"

std::unique_ptr<ImGUI, std::function<void(ImGUI *)>> ImGUI::s_Instance;

void ImGUI::Initialize(::HWND hWnd)
{
	assert(hWnd);

	/// Init key mapping
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = VK_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
	io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
	io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
	io.KeyMap[ImGuiKey_Home] = VK_HOME;
	io.KeyMap[ImGuiKey_End] = VK_END;
	io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
	io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
	io.KeyMap[ImGuiKey_A] = 'A';
	io.KeyMap[ImGuiKey_C] = 'C';
	io.KeyMap[ImGuiKey_V] = 'V';
	io.KeyMap[ImGuiKey_X] = 'X';
	io.KeyMap[ImGuiKey_Y] = 'Y';
	io.KeyMap[ImGuiKey_Z] = 'Z';
	io.RenderDrawListsFn = nullptr;
	io.ImeWindowHandle = hWnd;

	std::vector<Geometry::VertexLayout> layout =
	{
		{ "POSITION", sizeof(ImDrawVert::pos), offsetof(ImDrawVert, pos), eRG32_Float  },
		{ "TEXCOORD", sizeof(ImDrawVert::uv),  offsetof(ImDrawVert, uv),  eRG32_Float  },
		{ "COLOR",    sizeof(ImDrawVert::col), offsetof(ImDrawVert, col), eRGBA8_UNorm }
	};

	m_Resource.VertexShader.Create("ImGUI.shader", "VS_Main");
	m_Resource.VertexLayout.Create(m_Resource.VertexShader.GetBlob(), layout);
	m_Resource.PixelShader.Create("ImGUI.shader", "PS_Main");

	m_Resource.UniformBufferVS.CreateAsUniformBuffer(sizeof(Matrix), eGpuReadCpuWrite, nullptr);

	m_Resource.ClrWriteBlend.Create(false, false, 0U, true,
		eRBlend::eSrcAlpha, eRBlend::eInvSrcAlpha, eRBlendOp::eAdd,
		eRBlend::eInvSrcAlpha, eRBlend::eZero, eRBlendOp::eAdd,
		eColorAll);

	byte *pPixels = nullptr;
	int32_t width = 0;
	int32_t height = 0;
	io.Fonts->GetTexDataAsRGBA32(&pPixels, &width, &height);

	RTexture2D fontTex;
	RSubResourceData subResData
	{
		(const void *)pPixels,
		(uint32_t)width * 4U * sizeof(byte),
		0U,
	};
	fontTex.Create(eRGBA8_UNorm, (uint32_t)width, (uint32_t)height, eBindAsShaderResource, 1U, 1U, 0U, 0U, eGpuReadWrite, &subResData);
	m_Resource.FontTexture.CreateAsTexture(eTexture2D, fontTex, eRGBA8_UNorm, 1U, 1U);
	///m_Resource.FontTexture.BindSampler(RStaticState::LinearSampler);
	io.Fonts->TexID = (void *)m_Resource.FontTexture.Get();

	m_Ready = true;
}

::LRESULT ImGUI::HandleWindowMessage(::HWND hWnd, uint32_t uMsg, ::WPARAM wParam, ::LPARAM lParam)
{
	if (!m_Ready)
	{
		return 0LL;
	}

	ImGuiIO &io = ImGui::GetIO();

	int32_t mouseBtn = -1;
	if (WM_LBUTTONDOWN == uMsg || WM_LBUTTONUP == uMsg)
	{
		mouseBtn = 0;
	}
	if (WM_RBUTTONDOWN == uMsg || WM_RBUTTONUP == uMsg)
	{
		mouseBtn = 1;
	}
	if (WM_MBUTTONDOWN == uMsg || WM_MBUTTONUP == uMsg)
	{
		mouseBtn = 2;
	}

	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		if (!IsMouseBtnDown() && ::GetCapture() == nullptr)
		{
			::SetCapture(hWnd);
		}
		io.MouseDown[mouseBtn] = true;
		return 0LL;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		io.MouseDown[mouseBtn] = false;
		if (!IsMouseBtnDown() && ::GetCapture() == hWnd)
		{
			::ReleaseCapture();
		}
		return 0LL;
	case WM_MOUSEWHEEL:
		io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
		return 0LL;
	case WM_MOUSEMOVE:
		io.MousePos.x = (int16_t)(lParam);
		io.MousePos.y = (int16_t)(lParam >> 16);
		return 0LL;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (wParam < 256)
		{
			io.KeysDown[wParam] = 1;
		}
		return 0LL;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (wParam < 256)
		{
			io.KeysDown[wParam] = 0;
		}
		return 0LL;
	case WM_CHAR:
		/// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
		if (wParam > 0 && wParam < 0x10000)
		{
			io.AddInputCharacter((uint16_t)wParam);
		}
		return 0LL;
	}

	return 0LL;
}

void ImGUI::RenderBegin(const char *pPanelName)
{
	ImGuiIO &io = ImGui::GetIO();
	::HWND hWnd = (::HWND)io.ImeWindowHandle;
	assert(hWnd);

	::RECT rect;
	::GetClientRect(hWnd, &rect);
	io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

	/// Read keyboard modifiers inputs
	io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
	io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
	io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
	io.KeySuper = false;

	/// Set OS mouse position if requested last frame by io.WantMoveMouse flag (used when io.NavMovesTrue is enabled by user and using directional navigation)
	if (io.WantCaptureMouse)
	{
		::POINT pos = { (LONG)io.MousePos.x, (LONG)io.MousePos.y };
		::ClientToScreen(hWnd, &pos);
		::SetCursorPos(pos.x, pos.y);
	}

	/// Hide OS mouse cursor if ImGui is drawing it
	if (io.MouseDrawCursor)
	{
		::SetCursor(nullptr);
	}

	ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_FirstUseEver);

	/// Start the frame. This call will update the io.WantCaptureMouse, 
	/// io.WantCaptureKeyboard flag that you can use to dispatch inputs (or not) to your application.
	ImGui::NewFrame();

	ImGui::Begin(pPanelName);
}

void ImGUI::RenderEnd()
{
	ImGui::End();

	ImGui::Render();
}

bool ImGUI::Update()
{
	ImDrawData *pDrawData = ImGui::GetDrawData();
	if (!pDrawData || pDrawData->CmdListsCount == 0U)
	{
		return false;
	}

	UpdateDrawData(pDrawData);

	float L = pDrawData->DisplayPos.x;
	float R = pDrawData->DisplayPos.x + pDrawData->DisplaySize.x;
	float B = pDrawData->DisplayPos.y + pDrawData->DisplaySize.y;
	float T = pDrawData->DisplayPos.y;
#if defined(UsingVulkan)
	Matrix wvp(
		2.0f / (R - L),    0.0f,               0.0f, 0.0f,
		0.0f,              -2.0f / (T - B),    0.0f, 0.0f,
		0.0f,              0.0f,               1.0f, 0.0f,
		(R + L) / (L - R), -(T + B) / (B - T), 0.0f, 1.0f
	);
#elif defined(UsingD3D11)
	Matrix wvp(
		2.0f / (R - L), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (T - B), 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		(R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f
	);
#endif

	m_Resource.UniformBufferVS.Update(&wvp, sizeof(Matrix));

	RViewport vp(0.0f, 0.0f, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
	REngine::Instance().SetViewport(vp);
	REngine::Instance().SetVertexShader(m_Resource.VertexShader);
	REngine::Instance().SetPixelShader(m_Resource.PixelShader);
	REngine::Instance().SetInputLayout(m_Resource.VertexLayout);
	REngine::Instance().SetVertexBuffer(m_Resource.VertexBuffer, sizeof(ImDrawVert), 0U);
	REngine::Instance().SetIndexBuffer(m_Resource.IndexBuffer, sizeof(ImDrawIdx) == 2U ? eR16_UInt : eR32_UInt, 0U);
	REngine::Instance().SetUniformBuffer(m_Resource.UniformBufferVS, 0U, eVertexShader);
	REngine::Instance().SetSamplerState(RStaticState::LinearSampler, 0U, ePixelShader);
	REngine::Instance().SetShaderResourceView(m_Resource.FontTexture, 0U, ePixelShader);
	REngine::Instance().SetBlendState(m_Resource.ClrWriteBlend);
	REngine::Instance().SetDepthStencilState(RStaticState::DisableDepthStencil, 0U);
	REngine::Instance().SetRasterizerState(RStaticState::SolidNoneCulling);

	for (int32_t i = 0, vOffset = 0, iOffset = 0; i < pDrawData->CmdListsCount; ++i)
	{
		const ImDrawList *pDrawList = pDrawData->CmdLists[i];
		for (int j = 0; j < pDrawList->CmdBuffer.Size; ++j)
		{
			const ImDrawCmd *pDrawCmd = &pDrawList->CmdBuffer[j];
			if (pDrawCmd->UserCallback)
			{
				pDrawCmd->UserCallback(pDrawList, pDrawCmd);
			}
			else
			{
				::RECT scissorRect
				{
					(long)pDrawCmd->ClipRect.x, 
					(long)pDrawCmd->ClipRect.y, 
					(long)pDrawCmd->ClipRect.z, 
					(long)pDrawCmd->ClipRect.w
				};

				REngine::Instance().SetScissorRect(scissorRect);
				REngine::Instance().DrawIndexed(pDrawCmd->ElemCount, iOffset, vOffset, eTriangleList);
			}

			iOffset += pDrawCmd->ElemCount;
		}

		vOffset += pDrawList->VtxBuffer.Size;
	}

	/// Restore D3D state
	::HWND hWnd = (::HWND)ImGui::GetIO().ImeWindowHandle;
	::RECT rect = {};
	::GetClientRect(hWnd, &rect);

	RViewport vpOld(0.0f, 0.0f, (float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
	REngine::Instance().SetViewport(vpOld);
	REngine::Instance().SetScissorRect(rect);
	REngine::Instance().SetRasterizerState(RStaticState::Solid);
	REngine::Instance().SetBlendState(RStaticState::NoneBlendState);
	REngine::Instance().SetDepthStencilState(RStaticState::NoneDepthStencilState, 0U);

	return true;
}

void ImGUI::UpdateDrawData(ImDrawData *pDrawData)
{
	assert(pDrawData);

	size_t vertexBufferSize = pDrawData->TotalVtxCount * sizeof(ImDrawVert);
	size_t indexBufferSize = pDrawData->TotalIdxCount * sizeof(ImDrawIdx);

	if (0U == vertexBufferSize || 0U == indexBufferSize)
	{
		return;
	}

	m_VertexCount = pDrawData->TotalVtxCount;
	m_Vertices.reset(new ImDrawVert[m_VertexCount]());
	if (m_Resource.VertexBuffer.IsValid())
	{
		m_Resource.VertexBuffer.Destory();
	}
	m_Resource.VertexBuffer.CreateAsVertexBuffer(m_VertexCount * sizeof(ImDrawVert), eGpuReadCpuWrite, nullptr);

	m_IndexCount = pDrawData->TotalIdxCount;
	m_Indices.reset(new ImDrawIdx[m_IndexCount]());
	if (m_Resource.IndexBuffer.IsValid())
	{
		m_Resource.IndexBuffer.Destory();
	}
	m_Resource.IndexBuffer.CreateAsIndexBuffer(m_IndexCount * sizeof(ImDrawIdx), eGpuReadCpuWrite, nullptr);

	/// Update vertices and indices
	for (int i = 0, totalVertices = 0, totalIndices = 0; i < pDrawData->CmdListsCount; ++i)
	{
		const ImDrawList *pDrawList = pDrawData->CmdLists[i];

		memcpy(m_Vertices.get() + totalVertices, pDrawList->VtxBuffer.Data, pDrawList->VtxBuffer.Size * sizeof(ImDrawVert));
		totalVertices += pDrawList->VtxBuffer.Size;

		memcpy(m_Indices.get() + totalIndices, pDrawList->IdxBuffer.Data, pDrawList->IdxBuffer.Size * sizeof(ImDrawIdx));
		totalIndices += pDrawList->IdxBuffer.Size;
	}

	m_Resource.VertexBuffer.Update(m_Vertices.get(), sizeof(ImDrawVert) * m_VertexCount);
	m_Resource.IndexBuffer.Update(m_Indices.get(), sizeof(ImDrawIdx) * m_IndexCount);
}
