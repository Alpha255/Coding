#include "GUI_imGUI.h"

#include "D3DGraphic.h"

extern D3DGraphic *g_Renderer;

struct imGUIResource
{
	Ref<ID3D11InputLayout> VertexLayout;

	Ref<ID3D11VertexShader> VertexShader;
	Ref<ID3D11PixelShader> PixelShader;

	Ref<ID3D11Buffer> VertexBuffer;
	Ref<ID3D11Buffer> IndexBuffer;

	Ref<ID3D11Buffer> CBufVS;

	Ref<ID3D11BlendState> ClrWriteBlend;

	Ref<ID3D11RasterizerState> NoneCulling;

	Ref<ID3D11RasterizerState> BackFaceCulling;

	Ref<ID3D11DepthStencilState> DepthStencilOp;

	Ref<ID3D11ShaderResourceView> FontTexture;
	Ref<ID3D11SamplerState> SamplerLinear;
};

struct ConstantsBufferVS
{
	Matrix WVP;
};

static imGUIResource s_imResource;

int32_t imGUI_D3D11::m_VertexCount = 0U;
int32_t imGUI_D3D11::m_IndexCount = 0U;

void imGUI_D3D11::Init(HWND hWnd)
{
	assert(hWnd && g_Renderer);

	InitKeyMap(hWnd);
	
	InitD3DResource();
}

LRESULT imGUI_D3D11::WinProc(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam)
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

void imGUI_D3D11::RenderBegin(const char *pPanelName)
{
	ImGuiIO &io = ImGui::GetIO();
	HWND hWnd = (HWND)io.ImeWindowHandle;
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
	if (io.WantMoveMouse)
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

	/// Start the frame. This call will update the io.WantCaptureMouse, 
	/// io.WantCaptureKeyboard flag that you can use to dispatch inputs (or not) to your application.
	ImGui::NewFrame();

	ImGui::Begin(pPanelName);
}

void imGUI_D3D11::RenderEnd()
{
	ImGui::End();

	ImGui::Render();
}

void imGUI_D3D11::InitKeyMap(HWND hWnd)
{
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

	/// Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
	io.RenderDrawListsFn = RenderListCallback;
	io.ImeWindowHandle = hWnd;
}

void imGUI_D3D11::InitD3DResource()
{
	static char *const s_ShaderName = "imGUI.hlsl";

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->uv),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (size_t)(&((ImDrawVert*)0)->col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	g_Renderer->CreateVertexShaderAndInputLayout(s_imResource.VertexShader.Reference(), s_imResource.VertexLayout.Reference(),
		layout, ARRAYSIZE(layout), s_ShaderName, "VS_Main");
	g_Renderer->CreatePixelShader(s_imResource.PixelShader.Reference(), s_ShaderName, "PS_Main");

	g_Renderer->CreateConstantBuffer(s_imResource.CBufVS.Reference(), sizeof(ConstantsBufferVS),
		D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

	g_Renderer->CreateRasterizerState(s_imResource.NoneCulling.Reference(), D3D11_FILL_SOLID, D3D11_CULL_NONE, false, true, true);
	g_Renderer->CreateRasterizerState(s_imResource.BackFaceCulling.Reference(), D3D11_FILL_SOLID, D3D11_CULL_BACK);

	D3D11_BLEND_DESC blendDesc{ 0 };
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_Renderer->CreateBlendState(s_imResource.ClrWriteBlend.Reference(), &blendDesc);

	D3D11_DEPTH_STENCIL_DESC depthstencilDesc{ 0 };
	depthstencilDesc.DepthEnable = false;
	depthstencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthstencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthstencilDesc.StencilEnable = false;
	depthstencilDesc.FrontFace.StencilFailOp = depthstencilDesc.FrontFace.StencilDepthFailOp = depthstencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthstencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthstencilDesc.BackFace = depthstencilDesc.FrontFace;
	g_Renderer->CreateDepthStencilState(s_imResource.DepthStencilOp.Reference(), &depthstencilDesc);

	InitFontTextures();
}

void imGUI_D3D11::InitFontTextures()
{
	ImGuiIO &io = ImGui::GetIO();

	unsigned char *pPixels = nullptr;
	int32_t width = 0, height = 0;
	io.Fonts->GetTexDataAsRGBA32(&pPixels, &width, &height);

	Ref<ID3D11Texture2D> fontTex;
	g_Renderer->CreateTexture2D(fontTex.Reference(), DXGI_FORMAT_R8G8B8A8_UNORM,
		(uint32_t)width, (uint32_t)height,
		D3D11_BIND_SHADER_RESOURCE, 1U, 1U, 0U, 1U, 0U, 0U,
		D3D11_USAGE_DEFAULT, (const void*)pPixels, (uint32_t)width * 4, 0U);
	g_Renderer->CreateShaderResourceView(s_imResource.FontTexture.Reference(), fontTex.Ptr(), DXGI_FORMAT_R8G8B8A8_UNORM);

	io.Fonts->TexID = (void *)s_imResource.FontTexture.Ptr();

	D3D11_SAMPLER_DESC samplerDesc;
	memset(&samplerDesc, 0, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.f;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0.f;
	samplerDesc.MaxLOD = 0.f;
	g_Renderer->CreateSamplerState(s_imResource.SamplerLinear.Reference(), &samplerDesc);
}

void imGUI_D3D11::RenderListCallback(ImDrawData * pDrawData)
{
	if (!pDrawData)
	{
		return;
	}

	RecreateVIBuffers(
		(!s_imResource.VertexBuffer.Valid() || m_VertexCount < pDrawData->TotalVtxCount),
		(!s_imResource.IndexBuffer.Valid() || m_IndexCount < pDrawData->TotalIdxCount),
		pDrawData);

	float L = 0.0f;
	float R = ImGui::GetIO().DisplaySize.x;
	float B = ImGui::GetIO().DisplaySize.y;
	float T = 0.0f;
	ConstantsBufferVS cbVS;
	cbVS.WVP = Matrix(
		2.0f / (R - L), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (T - B), 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		(R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f
	);
	g_Renderer->UpdateConstantBuffer(s_imResource.CBufVS.Ptr(), &cbVS, sizeof(ConstantsBufferVS));

	D3D11_VIEWPORT vp{ 0 };
	vp.Width = ImGui::GetIO().DisplaySize.x;
	vp.Height = ImGui::GetIO().DisplaySize.y;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = vp.TopLeftY = 0.0f;
	g_Renderer->SetViewports(&vp);

	g_Renderer->SetVertexShader(s_imResource.VertexShader.Ptr());
	g_Renderer->SetPixelShader(s_imResource.PixelShader.Ptr());
	g_Renderer->SetInputLayout(s_imResource.VertexLayout.Ptr());
	g_Renderer->SetVertexBuffer(s_imResource.VertexBuffer.Ptr(), sizeof(ImDrawVert), 0U);
	g_Renderer->SetIndexBuffer(s_imResource.IndexBuffer.Ptr(), sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0U);
	g_Renderer->SetConstantBuffer(s_imResource.CBufVS.Ptr(), 0U, D3DGraphic::eVertexShader);
	g_Renderer->SetSamplerStates(s_imResource.SamplerLinear.Ptr());
	g_Renderer->SetBlendState(s_imResource.ClrWriteBlend.Ptr(), Vec4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
	g_Renderer->SetDepthStencilState(s_imResource.DepthStencilOp.Ptr(), 0U);
	g_Renderer->SetRasterizerState(s_imResource.NoneCulling.Ptr());

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
				D3D11_RECT rect = { (LONG)pDrawCmd->ClipRect.x, (LONG)pDrawCmd->ClipRect.y, (LONG)pDrawCmd->ClipRect.z, (LONG)pDrawCmd->ClipRect.w };
				g_Renderer->SetShaderResource(s_imResource.FontTexture.Ptr());
				g_Renderer->SetScissorRects(&rect);
				g_Renderer->DrawIndexed(pDrawCmd->ElemCount, iOffset, vOffset);
			}

			iOffset += pDrawCmd->ElemCount;
		}

		vOffset += pDrawList->VtxBuffer.Size;
	}

	RestoreD3DState();
}

void imGUI_D3D11::RecreateVIBuffers(bool bCreateVB, bool bCreateIB, const ImDrawData *pDrawData)
{
	static Ref<ID3D11Buffer> nullBuffer;

	if (bCreateVB)
	{
		m_VertexCount = pDrawData->TotalVtxCount;

		s_imResource.VertexBuffer = nullBuffer;

		uint32_t totalVertexCount = (uint32_t)(pDrawData->TotalVtxCount + 5000);
		ImDrawVert *pVertices = new ImDrawVert[totalVertexCount]();
		for (int i = 0, totalVertices = 0; i < pDrawData->CmdListsCount; ++i)
		{
			const ImDrawList *pDrawList = pDrawData->CmdLists[i];
			memcpy(pVertices + totalVertices, pDrawList->VtxBuffer.Data, pDrawList->VtxBuffer.Size * sizeof(ImDrawVert));
			totalVertices += pDrawList->VtxBuffer.Size;
		}

		g_Renderer->CreateVertexBuffer(s_imResource.VertexBuffer.Reference(), totalVertexCount * sizeof(ImDrawVert),
			D3D11_USAGE_IMMUTABLE, (const void*)pVertices);

		SafeDeleteArray(pVertices);
	}

	if (bCreateIB)
	{
		m_IndexCount = pDrawData->TotalIdxCount;

		s_imResource.IndexBuffer = nullBuffer;

		uint32_t totalIndexCount = (uint32_t)(pDrawData->TotalIdxCount + 10000);
		ImDrawIdx *pIndices = new ImDrawIdx[totalIndexCount]();
		for (int i = 0, totalIndices = 0; i < pDrawData->CmdListsCount; ++i)
		{
			const ImDrawList *pDrawList = pDrawData->CmdLists[i];
			memcpy(pIndices + totalIndices, pDrawList->IdxBuffer.Data, pDrawList->IdxBuffer.Size * sizeof(ImDrawIdx));
			totalIndices += pDrawList->IdxBuffer.Size;
		}

		g_Renderer->CreateIndexBuffer(s_imResource.IndexBuffer.Reference(), totalIndexCount * sizeof(ImDrawIdx),
			D3D11_USAGE_IMMUTABLE, pIndices);

		SafeDeleteArray(pIndices);
	}
}

void imGUI_D3D11::RestoreD3DState()
{
	ImGuiIO &io = ImGui::GetIO();
	HWND hWnd = (HWND)io.ImeWindowHandle;
	assert(hWnd);

	::RECT rect;
	::GetClientRect(hWnd, &rect);

	D3D11_VIEWPORT vp{ 0 };
	vp.Width = (float)(rect.right - rect.left);
	vp.Height = (float)(rect.bottom - rect.top);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = vp.TopLeftY = 0.0f;
	g_Renderer->SetViewports(&vp);

	g_Renderer->SetScissorRects(&rect);

	g_Renderer->SetRasterizerState(s_imResource.BackFaceCulling.Ptr());

	///g_Renderer->SetInputLayout(nullptr);
	///g_Renderer->SetBlendState(nullptr, Vec4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
	g_Renderer->SetDepthStencilState(nullptr, 0U);
}

imGUI_D3D11::~imGUI_D3D11()
{
	ImGui::Shutdown();
}
