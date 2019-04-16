#pragma once

#include "D3DShader.h"
#include "D3DBuffer.h"
#include "D3DView.h"
#include "D3DMath.h"
#include "D3DState.h"
#include <unordered_map>

class D3DFontFreeType
{
public:
	static D3DFontFreeType &Instance()
	{
		if (!s_Instance)
		{
			s_Instance = std::unique_ptr<D3DFontFreeType, std::function<void(D3DFontFreeType *)>>
				(new D3DFontFreeType(), [](D3DFontFreeType *pFontEngine) { SafeDelete(pFontEngine) });
		}
		return *s_Instance;
	}

	void Initialize(uint32_t width, uint32_t height);
	void RenderText(const char *pTextContent, uint32_t left, uint32_t top, uint32_t fontSize);
	void Resize(uint32_t width, uint32_t height);
protected:
	struct Character 
	{
		D3DShaderResourceView CharTex;
		Vec2 Size;
		Vec2 Bearing;
		uint32_t Advance;
	};

	D3DFontFreeType() = default;
	~D3DFontFreeType() = default;

	void InitFreeTypeLib(const char *pFontName);
private:
	static std::unique_ptr<D3DFontFreeType, std::function<void(D3DFontFreeType *)>> s_Instance;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PiexlShader;

	D3DInputLayout m_VertexLayout;

	D3DBlendState m_AlphaBlend;

	D3DBuffer m_CBufferVS;

	std::unordered_map<byte, Character> m_CharsMap;

	bool m_Inited = false;
};
