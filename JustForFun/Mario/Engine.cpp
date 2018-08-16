#include "Engine.h"

#include "Image.h"
#include "Objects.h"
#include "Map.h"
#include "D3DEngine.h"
#include "GameApplication.h"

std::unique_ptr<Engine, std::function<void(Engine *)>> Engine::s_Instance;

void Engine::LoadTextures()
{
	static const char *const s_TexNames[Object2D::eTypeCount] = 
	{
		"Mario\\brick.dds",
		"Mario\\bullet.dds",
		"Mario\\coin.dds",
		"Mario\\explode.dds",
		"Mario\\flower.dds",
		"Mario\\mario.dds",
		"Mario\\monster.dds",
		"Mario\\mushroom.dds",
		"Mario\\tile.dds",
		"Mario\\turtle.dds"
	};

	for (uint32_t i = 0U; i < m_Textures.size(); ++i)
	{
		m_Textures[i].Create(s_TexNames[i]);
	}
}

void Engine::LoadMaps()
{
	static const char *const s_MapNames[Map::eMapCount] =
	{
		"level0.dat",
		"level1.dat"
	};

	for (uint32_t i = 0U; i < m_Maps.size(); ++i)
	{
		m_Maps[i].Create(s_MapNames[i]);
	}

	m_CurrentMap = &m_Maps[0];
}

void Engine::InitD3DResource()
{
	const D3D11_INPUT_ELEMENT_DESC VertexLayout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,      0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	m_VertexShader.Create("Mario.hlsl", "VSMain");
	m_PixelShader.Create("Mario.hlsl", "PSMain");
	m_PixelShaderDark.Create("Mario.hlsl", "PSMainDark");
	m_VertexLayout.Create(m_VertexShader.GetBlob(), VertexLayout, _countof(VertexLayout));

	m_VertexBuffer.CreateAsVertexBuffer(sizeof(Vertex) * m_Vertices.size(), D3DBuffer::eGpuReadCpuWrite, nullptr);
	m_IndexBuffer.CreateAsIndexBuffer(sizeof(uint32_t) * m_Indices.size(), D3DBuffer::eGpuReadOnly, m_Indices.data());

	m_AlphaBlend.Create(false, false, 0U, true,
		D3DState::eSrcAlpha, D3DState::eInvSrcAlpha, D3DState::eAdd,
		D3DState::eOne, D3DState::eZero, D3DState::eAdd,
		D3DState::eColorAll);

	m_Viewport = { 0.0f, 0.0f, (float)GameApplication::eWidth, (float)GameApplication::eHeight, 0U };
}

void Engine::DrawMap()
{
	assert(m_CurrentMap);

	Object2D *pMapObject = m_CurrentMap->GetMapObject();

	uint32_t min = m_CurrentMap->Left() / Map::eObjectWidth;
	uint32_t max = min + 20U;  /// WindowWidth / s_MapObjectW
	int32_t deltaLeft = m_CurrentMap->Left() - min * Map::eObjectWidth;

	for (uint32_t i = 0U; i < m_CurrentMap->Height(); ++i)
	{
		uint32_t left = 0U;

		for (uint32_t j = min; j <= max; ++j)
		{
			const char mark = m_CurrentMap->GetStaticMark(i * m_CurrentMap->Width() + j);

			if (mark > 0)
			{
				uint32_t imageX = mark % Map::eObjectCount * Map::eObjectWidth;
				uint32_t imageY = mark / Map::eObjectCount * Map::eObjectHeight;

				uint32_t width = Map::eObjectWidth;
				uint32_t height = Map::eObjectHeight;

				if (min == j)
				{
					imageX += deltaLeft;
				}
				else if (max == j)
				{
					width -= (Map::eObjectWidth - deltaLeft);
				}

				pMapObject->UpdateArea(i * Map::eObjectHeight, left, width, height, imageX, imageY + m_CurrentMap->GetInvertImageY());

				DrawObject(*pMapObject);
			}

			if (min == j)
			{
				left += (Map::eObjectWidth - deltaLeft);
			}
			else
			{
				left += Map::eObjectWidth;
			}
		}
	}
}

void Engine::UpdateVertexBuffer(const Object2D &object)
{
	const Object2D::Area &area = object.GetArea();

	float width = (float)area.Width / GameApplication::eWidth * 2.0f;
	float height = (float)area.Height / GameApplication::eHeight * 2.0f;

	Vec2 lefttop((float)area.Left / GameApplication::eWidth, (float)(GameApplication::eHeight - area.Top) / GameApplication::eHeight);
	lefttop *= 2.0f;
	lefttop -= 1.0f;

	m_Vertices[0].Position = Vec3(lefttop.x, lefttop.y, 0.0f);
	m_Vertices[1].Position = Vec3(lefttop.x + width, lefttop.y, 0.0f);
	m_Vertices[2].Position = Vec3(lefttop.x + width, lefttop.y - height, 0.0f);
	m_Vertices[3].Position = Vec3(lefttop.x, lefttop.y - height, 0.0f);

	uint32_t imageWidth = object.GetImage()->Width();
	uint32_t imageHeight = object.GetImage()->Height();
	float uwidth = (float)area.Width / imageWidth;
	float uheight = (float)area.Height / imageHeight;

	Vec2 ulefttop((float)area.ImageX / imageWidth, (float)area.ImageY / imageHeight);
	m_Vertices[0].UV = Vec2(ulefttop.x, ulefttop.y - uheight);
	m_Vertices[1].UV = Vec2(ulefttop.x + uwidth, ulefttop.y - uheight);
	m_Vertices[2].UV = Vec2(ulefttop.x + uwidth, ulefttop.y);
	m_Vertices[3].UV = ulefttop;
}

void Engine::DrawObject(const Object2D &object)
{
	UpdateVertexBuffer(object);

	m_VertexBuffer.Update(m_Vertices.data(), sizeof(Vertex) * m_Vertices.size());

	D3DEngine::Instance().SetVertexBuffer(m_VertexBuffer, sizeof(Vertex), 0U, 0U);
	D3DEngine::Instance().SetIndexBuffer(m_IndexBuffer, eR32_UInt, 0U);
	D3DEngine::Instance().SetInputLayout(m_VertexLayout);

	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_CurrentMap->IsDarkMode() ? m_PixelShaderDark : m_PixelShader);

	D3DEngine::Instance().SetShaderResourceView(object.GetImage()->GetShaderResourceView(), 0U, D3DShader::ePixelShader);
	
	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetBlendState(m_AlphaBlend);

	D3DEngine::Instance().DrawIndexed((uint32_t)m_Indices.size(), 0U, 0, eTriangleList);
}

void Engine::DrawObjects()
{
}

#if 0
Object2D *pTest = new Object2D(Object2D::eTile);
#endif

void Engine::Init(HWND hWnd, uint32_t width, uint32_t height)
{
	D3DEngine::Instance().Initialize(hWnd, width, height, true);

	InitD3DResource();

	LoadTextures();

	LoadMaps();

#if 0
	pTest->UpdateArea(64U, 128U, 32U, 32U, 256U, 32U);
#endif
}

void Engine::Update(float /*elapseTime*/, float /*totalTime*/)
{

}

void Engine::HandleInput(uint32_t msg, WPARAM wParam, LPARAM /*lParam*/)
{
	if (WM_SYSKEYDOWN != msg && WM_KEYDOWN != msg)
	{
		return;
	}

#if 1
	switch (wParam)
	{
	case VK_RIGHT:
		m_CurrentMap->HorizontalScrolling(5);
		break;
	case VK_LEFT:
		m_CurrentMap->HorizontalScrolling(-5);
		break;
	}
#endif
}

void Engine::RenderScene()
{
	D3DEngine::Instance().ResetDefaultRenderSurfaces(m_BackColor);
	D3DEngine::Instance().SetViewport(m_Viewport);
#if 0 
	DrawObject(*pTest);
#endif

#if 1
	DrawMap();
#endif

	D3DEngine::Instance().Flush();
}

void Engine::Resize(uint32_t width, uint32_t height)
{
	m_Viewport.Width = (float)width;
	m_Viewport.Height = (float)height;

	D3DEngine::Instance().Resize(width, height);
}
