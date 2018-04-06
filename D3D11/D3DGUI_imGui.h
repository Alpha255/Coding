#pragma once

#include "Common.h"
#include "D3DShader.h"
#include "D3DView.h"
#include "D3DState.h"
#include "D3DBuffer.h"

#include <GUI/ocornut/imgui.h>

class D3DGUI_imGui
{
public:
	static D3DGUI_imGui &Instance()
	{
		if (!s_Instance)
		{
			s_Instance = std::unique_ptr<D3DGUI_imGui>(new D3DGUI_imGui());
		}

		return *s_Instance;
	}

	static void Destroy(D3DGUI_imGui *pGUI)
	{
		if (pGUI)
		{
			ImGui::Shutdown();

			SafeDeleteArray(pGUI->m_pVertices);
			SafeDeleteArray(pGUI->m_pIndices);
		}
	}

	void Initialize(::HWND hWnd);

	::LRESULT WinProc(::HWND hWnd, uint32_t uMsg, ::WPARAM wParam, ::LPARAM lParam);

	void RenderBegin(const char *pPanelName = "imGUI");
	void RenderEnd();
	void UpdateAndRender();

	inline bool IsFocus() const 
	{
		const ImGuiIO &io = ImGui::GetIO();
		return io.WantCaptureMouse || io.WantCaptureKeyboard;
	}
protected:
	D3DGUI_imGui() = default;

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
	static std::unique_ptr<D3DGUI_imGui> s_Instance;

	int32_t m_VertexCount = 0;
	int32_t m_IndexCount = 0;

	ImDrawVert *m_pVertices = nullptr;
	ImDrawIdx *m_pIndices = nullptr;

	RenderResource m_Resource;

	bool m_Inited = false;
};
