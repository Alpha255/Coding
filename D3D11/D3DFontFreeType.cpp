#include "D3DFontFreeType.h"
#include "D3DTexture.h"
#include "D3DEngine.h"
#include "System.h"

#include <freetype/include/ft2build.h>
#include <freetype/include/freetype/freetype.h>

std::unique_ptr<D3DFontFreeType, std::function<void(D3DFontFreeType *)>> D3DFontFreeType::s_Instance;

void D3DFontFreeType::InitFreeTypeLib(const char *pFontName)
{
	std::string fontPath("C:/Windows/Fonts/");
	fontPath = fontPath + pFontName + ".ttf";

	FT_Library ftLib;
	FT_Error result = 0;
	result = FT_Init_FreeType(&ftLib);
	assert(result == FT_Err_Ok);

	FT_Face ftFace;
	result = FT_New_Face(ftLib, fontPath.c_str(), 0, &ftFace);
	assert(result == FT_Err_Ok);

	///FT_Size_RequestRec ftSize;
	///result = FT_Request_Size(ftFace, &ftSize);
	///assert(result == FT_Err_Ok);

	result = FT_Set_Pixel_Sizes(ftFace, 0, 48);
	assert(result == FT_Err_Ok);

	for (byte c = 0; c < 128; ++c)
	{
		result = FT_Load_Char(ftFace, c, FT_LOAD_RENDER);
		assert(result == FT_Err_Ok);

		if (ftFace->glyph->bitmap.width == 0 || ftFace->glyph->bitmap.rows == 0)
		{
			std::string info("Skip character:");
			info = info + (char)c + '\n';
			OutputDebugStringA(info.c_str());
			continue;
		}

		D3DTexture2D charTex;
		charTex.Create(eR8_Typeless, ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows, D3DBuffer::eBindAsShaderResource, 1U, 1U, 0U, 0U, 0U, ftFace->glyph->bitmap.buffer, ftFace->glyph->bitmap.pitch);

		Character newChar;
		newChar.CharTex.CreateAsTexture(D3DView::eTexture2D, charTex, eR8_UNorm, 0U, 1U);
		newChar.Size = Vec2((float)ftFace->glyph->bitmap.width, (float)ftFace->glyph->bitmap.rows);
		newChar.Bearing = Vec2((float)ftFace->glyph->bitmap_left, (float)ftFace->glyph->bitmap_top);
		newChar.Advance = ftFace->glyph->advance.x;

		m_CharsMap.insert(std::make_pair(c, newChar));
	}

	result = FT_Done_Face(ftFace);
	assert(result == FT_Err_Ok);

	result = FT_Done_FreeType(ftLib);
	assert(result == FT_Err_Ok);
}

void D3DFontFreeType::Initialize(uint32_t width, uint32_t height)
{
	InitFreeTypeLib("arial");

	m_VertexShader.Create("Font.hlsl", "VSMain");
	m_PiexlShader.Create("Font.hlsl", "PSMain");

	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	m_VertexLayout.Create(m_VertexShader.GetBlob(), layout, _countof(layout));

	m_CBufferVS.CreateAsConstantBuffer(sizeof(Matrix), D3DBuffer::eGpuReadCpuWrite);
	Matrix ortho = Matrix::OrthographicLH((float)width, (float)height, 0.0f, 1.0f);
	m_CBufferVS.Update(&ortho, sizeof(Matrix));

	m_AlphaBlend.Create(false, false, 0U, true, 
		D3DState::eSrcAlpha, D3DState::eInvSrcAlpha, D3DState::eAdd,
		D3DState::eInvSrcAlpha, D3DState::eZero, D3DState::eAdd, 
		D3DState::eColorAll);

	m_Inited = true;
}

void D3DFontFreeType::Resize(uint32_t width, uint32_t height)
{
	Matrix ortho = Matrix::OrthographicLH((float)width, (float)height, 0.0f, 1.0f);
	m_CBufferVS.Update(&ortho, sizeof(Matrix));
}

void D3DFontFreeType::RenderText(const char *pTextContent, uint32_t left, uint32_t top, uint32_t fontSize)
{
	assert(m_Inited);

	D3DEngine::Instance().SetInputLayout(m_VertexLayout);
	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PiexlShader);
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	std::string text(pTextContent);
	float strLength = 0.0f;
	for (std::string::const_iterator it = text.begin(); it != text.end(); ++it)
	{
		if (*it == ' ')
		{
			continue;
		}

		Character ch = m_CharsMap[*it];

		float xpos = left + ch.Bearing.x * fontSize;
		float ypos = top - (ch.Size.y - ch.Bearing.y) * fontSize;

		float w = ch.Size.x * fontSize;
		float h = ch.Size.y * fontSize;

		float vertices[6][4] = 
		{
			{ xpos + strLength,     ypos + h,   0.0, 0.0 },
			{ xpos + strLength,     ypos,       0.0, 1.0 },
			{ xpos + w + strLength, ypos,       1.0, 1.0 },

			{ xpos + strLength,     ypos + h,   0.0, 0.0 },
			{ xpos + w + strLength, ypos,       1.0, 1.0 },
			{ xpos + w + strLength, ypos + h,   1.0, 0.0 }
		};

		strLength += w;

		D3DBuffer vertexBuffer;
		vertexBuffer.CreateAsVertexBuffer(sizeof(float) * 24U, D3DBuffer::eGpuReadOnly, vertices, D3DBuffer::eBindAsVertexBuffer);

		D3DEngine::Instance().SetBlendState(m_AlphaBlend);
		D3DEngine::Instance().SetVertexBuffer(vertexBuffer, sizeof(float) * 4U, 0U);
		D3DEngine::Instance().SetShaderResourceView(ch.CharTex, 0U, D3DShader::ePixelShader);

		D3DEngine::Instance().Draw(6U, 0U, eTriangleList);
	}
}