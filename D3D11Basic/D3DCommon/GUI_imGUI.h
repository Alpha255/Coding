#pragma once

#include "Common.h"
#include <GUI/ocornut/imgui.h>

class imGUI_D3D11
{
public:
	imGUI_D3D11() = default;

	~imGUI_D3D11();

	void Init(HWND hWnd);

	LRESULT WinProc(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam);

	void RenderBegin(const char *pPanelName = "imGUI");
	void RenderEnd();

	inline bool IsFocus()
	{
		ImGuiIO &io = ImGui::GetIO();
		return io.WantCaptureMouse || io.WantCaptureKeyboard;
	}
protected:
	void InitKeyMap(HWND hWnd);
	void InitD3DResource();
	void InitFontTextures();
	static void RenderListCallback(ImDrawData *pDrawData);
	static void RestoreD3DState();
	static void RecreateVIBuffers(bool bCreateVB, bool bCreateIB, const ImDrawData *pDrawData);

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
	static int32_t m_VertexCount;
	static int32_t m_IndexCount;
};