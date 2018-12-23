#include "IImGUI.h"
#include "D3DMath.h"

void IImGUI::Initialize(::HWND hWnd)
{
	assert(hWnd && !m_Inited);

	/// Init key map
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

	InitRenderResource(io);

	m_Inited = true;
}

::LRESULT IImGUI::MessageProcFunc(::HWND hWnd, uint32_t uMsg, ::WPARAM wParam, ::LPARAM lParam)
{
	ImGuiIO &io = ImGui::GetIO();

	int mouseBtn = -1;
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
		io.MousePos.x = (signed short)(lParam);
		io.MousePos.y = (signed short)(lParam >> 16);
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
		// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
		if (wParam > 0 && wParam < 0x10000)
		{
			io.AddInputCharacter((unsigned short)wParam);
		}
		return 0LL;
	}

	return 0LL;
}

void IImGUI::RenderBegin(bool bDraw, const char *pPanelName)
{
	if (!bDraw)
	{
		return;
	}

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

	ImVec2 pos(10.0f, 10.0f);
	ImGui::SetNextWindowPos(pos, ImGuiCond_FirstUseEver);

	/// Start the frame. This call will update the io.WantCaptureMouse, 
	/// io.WantCaptureKeyboard flag that you can use to dispatch inputs (or not) to your application.
	ImGui::NewFrame();

	ImGui::Begin(pPanelName);
}

void IImGUI::RenderEnd(bool bDraw)
{
	if (!bDraw)
	{
		return;
	}

	ImGui::End();

	ImGui::Render();

	Update();
}

void IImGUI::Update()
{
	ImDrawData *pDrawData = ImGui::GetDrawData();
	if (!pDrawData)
	{
		return;
	}

	UpdateDrawData(m_VertexCount < pDrawData->TotalVtxCount, m_IndexCount < pDrawData->TotalIdxCount, pDrawData);

	float L = 0.0f;
	float R = ImGui::GetIO().DisplaySize.x;
	float B = ImGui::GetIO().DisplaySize.y;
	float T = 0.0f;
	Matrix wvp(
		2.0f / (R - L), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (T - B), 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		(R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f
	);

	SetupRenderResource(wvp);

	for (int i = 0, vOffset = 0, iOffset = 0; i < pDrawData->CmdListsCount; ++i)
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

				Draw(scissorRect, pDrawCmd, iOffset, vOffset);
			}

			iOffset += pDrawCmd->ElemCount;
		}

		vOffset += pDrawList->VtxBuffer.Size;
	}

	/// Restore D3D state
	::HWND hWnd = (::HWND)ImGui::GetIO().ImeWindowHandle;
	::RECT rect;
	::GetClientRect(hWnd, &rect);

	::RECT oldViewport
	{
		0,
		0,
		(rect.right - rect.left),
		(rect.bottom - rect.top)
	};
	RestoreRenderResource(oldViewport);
}

void IImGUI::UpdateDrawData(bool bRecreateVB, bool bRecreateIB, const ImDrawData *pDrawData)
{
	if (bRecreateVB)
	{
		m_VertexCount = pDrawData->TotalVtxCount + 5000;
		
		m_Vertices.reset(new ImDrawVert[m_VertexCount]());

		ResetVertexBuffer();
	}

	if (bRecreateIB)
	{
		m_IndexCount = pDrawData->TotalIdxCount + 10000;

		m_Indices.reset(new ImDrawIdx[m_IndexCount]());

		ResetIndexBuffer();
	}

	/// Update vertices and indices
	for (int i = 0, totalVertices = 0, totalIndices = 0; i < pDrawData->CmdListsCount; ++i)
	{
		const ImDrawList *pDrawList = pDrawData->CmdLists[i];

		memcpy(m_Vertices.get() + totalVertices, pDrawList->VtxBuffer.Data, pDrawList->VtxBuffer.Size * sizeof(ImDrawVert));
		totalVertices += pDrawList->VtxBuffer.Size;

		memcpy(m_Indices.get() + totalIndices, pDrawList->IdxBuffer.Data, pDrawList->IdxBuffer.Size * sizeof(ImDrawIdx));
		totalIndices += pDrawList->IdxBuffer.Size;
	}

	UpdateBuffers();
}
