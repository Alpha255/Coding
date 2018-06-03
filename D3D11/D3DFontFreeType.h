#pragma once

#include "D3DShader.h"
#include "D3DBuffer.h"
#include "D3DView.h"

class D3DFontFreeType
{
public:
	void Initialize();
	void RenderText(const char *pTextContent, const char *pFontName, uint32_t left, uint32_t top, uint32_t fontSize);
protected:
	D3DFontFreeType() = default;
	~D3DFontFreeType() = default;
private:
	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PiexlShader;

	D3DBuffer m_CBufferVS;
	D3DBuffer m_CBufferPS;

	D3DShaderResourceView m_FontTexture;

	bool m_Inited = false;
};
