#pragma once

#include "Common.h"
#include <ImGUI/imgui.h>

class ImGUI : public NoneCopyable
{
public:
	static ImGUI &Instance()
	{
		if (!s_Instance)
		{
			s_Instance = std::unique_ptr<ImGUI>(new ImGUI());
		}
	}

	inline static void Destroy()
	{
		ImGui::DestroyContext();
	}

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
protected:
	ImGUI() = default;
	~ImGUI() = default;

	void InitRenderResource();

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
private:
	struct RenderResource
	{
		RInputLayout VertexLayout;

		RVertexShader VertexShader;
		RPixelShader PixelShader;

		RBuffer VertexBuffer;
		RBuffer IndexBuffer;
		RBuffer ConstantBufferVS;

		RBlendState ClrWriteBlend;

		RShaderResourceView FontTexture;
	};

	RenderResource m_Resource;

	static std::unique_ptr<ImGUI> s_Instance;

	int32_t m_VertexCount = 0;
	int32_t m_IndexCount = 0;

	std::shared_ptr<ImDrawVert> m_Vertices;
	std::shared_ptr<ImDrawIdx> m_Indices;
	bool m_Inited = false;
};
