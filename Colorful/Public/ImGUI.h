#pragma once

#include "Base/Base.h"
#include "Definitions.h"
#include <ImGUI/imgui.h>

class ImGUI : public Base::NoneCopyable
{
public:
	static ImGUI &Instance()
	{
		if (!s_Instance)
		{
			s_Instance = std::unique_ptr<ImGUI, std::function<void(ImGUI *)>>
				(new ImGUI(), [](ImGUI *pImGUI) { SafeDelete(pImGUI) });
		}

		return *s_Instance;
	}

	inline void Finalize()
	{
		ImGui::DestroyContext();
	}

	void Initialize(::HWND hWnd);

	::LRESULT HandleWindowMessage(::HWND hWnd, uint32_t uMsg, ::WPARAM wParam, ::LPARAM lParam);

	void RenderBegin(const char *pPanelName = "imGUI");
	void RenderEnd();
	bool Update();

	inline bool IsFocus() const 
	{
		const ImGuiIO &io = ImGui::GetIO();
		return io.WantCaptureMouse || io.WantCaptureKeyboard;
	}
protected:
	ImGUI() = default;
	~ImGUI() = default;

	void InitRenderResource();
	
	void UpdateDrawData(ImDrawData *pDrawData);

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
		RBuffer UniformBufferVS;

		RBlendState ClrWriteBlend;

		RShaderResourceView FontTexture;
	};

	static std::unique_ptr<ImGUI, std::function<void(ImGUI *)>> s_Instance;

	int32_t m_VertexCount = 0;
	int32_t m_IndexCount = 0;

	std::shared_ptr<ImDrawVert> m_Vertices;
	std::shared_ptr<ImDrawIdx> m_Indices;

	RenderResource m_Resource;

	bool m_Ready = false;
};
