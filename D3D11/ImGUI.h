#pragma once

#include "Common.h"
#include "D3DShader.h"
#include "D3DView.h"
#include "D3DState.h"
#include "D3DBuffer.h"

#include <GUI/ocornut/imgui.h>

class ImGUI
{
public:
	static ImGUI &Instance()
	{
		if (!s_Instance)
		{
			s_Instance = std::unique_ptr<ImGUI>(new ImGUI());
		}

		return *s_Instance;
	}

	static void Destroy()
	{
		ImGui::DestroyContext();
	}

	void Initialize(::HWND hWnd);

	::LRESULT WinProc(::HWND hWnd, uint32_t uMsg, ::WPARAM wParam, ::LPARAM lParam);

	void RenderBegin(bool bDraw = true, const char *pPanelName = "imGUI");
	void RenderEnd(bool bDraw = true);
	void UpdateAndRender();

	inline bool IsFocus() const 
	{
		const ImGuiIO &io = ImGui::GetIO();
		return io.WantCaptureMouse || io.WantCaptureKeyboard;
	}
protected:
	ImGUI() = default;

	struct RenderResource
	{
		D3DInputLayout VertexLayout;

		D3DVertexShader VertexShader;
		D3DPixelShader PixelShader;

		D3DBuffer VertexBuffer;
		D3DBuffer IndexBuffer;
		D3DBuffer ConstantBufferVS;

		D3DBlendState ClrWriteBlend;

		D3DShaderResourceView FontTexture;
	};
	
	void UpdateDrawData(bool bRecreateVB, bool bRecreateIB, const ImDrawData *pDrawData);

	bool IsMouseBtnDown()
	{
		ImGuiIO &io = ImGui::GetIO();
		for (uint32_t i = 0U; i < ARRAYSIZE(io.MouseDown); ++i)
		{
			if (io.MouseDown[i])
			{
				return true;
			}
		}

		return false;
	}
private:
	static std::unique_ptr<ImGUI> s_Instance;

	int32_t m_VertexCount = 0;
	int32_t m_IndexCount = 0;

	std::shared_ptr<ImDrawVert> m_Vertices;
	std::shared_ptr<ImDrawIdx> m_Indices;

	RenderResource m_Resource;

	bool m_Inited = false;
};
