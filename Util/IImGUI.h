#pragma once

#include "Common.h"
#include <ImGUI/imgui.h>

class IImGUI : public NoneCopyable
{
public:
	IImGUI() = default;
	virtual ~IImGUI() = default;

	void Initialize(::HWND hWnd);

	inline void Destroy()
	{
		ImGui::DestroyContext();
	}

	::LRESULT MessageProcFunc(::HWND hWnd, uint32_t uMsg, ::WPARAM wParam, ::LPARAM lParam);

	void RenderBegin(bool bDraw = true, const char *pPanelName = "imGUI");
	void RenderEnd(bool bDraw = true);

	inline bool IsFocus() const 
	{
		const ImGuiIO &io = ImGui::GetIO();
		return io.WantCaptureMouse || io.WantCaptureKeyboard;
	}

	virtual void InitRenderResource(ImGuiIO &) = 0;
	virtual void SetupRenderResource(class Matrix &) = 0;
	virtual void Draw(::RECT &, const ImDrawCmd *, uint32_t, int32_t) = 0;
	virtual void RestoreRenderResource(::RECT &) = 0;
	virtual void ResetVertexBuffer() = 0;
	virtual void ResetIndexBuffer() = 0;
	virtual void UpdateBuffers() = 0;
protected:
	void Update();
	
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

	int32_t m_VertexCount = 0;
	int32_t m_IndexCount = 0;

	std::shared_ptr<ImDrawVert> m_Vertices;
	std::shared_ptr<ImDrawIdx> m_Indices;
private:

	bool m_Inited = false;
};
