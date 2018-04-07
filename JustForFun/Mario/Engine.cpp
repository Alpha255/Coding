#include "Engine.h"

#include "Image.h"
#include "Objects.h"
#include "Map.h"
#include "Renderer.h"

Engine *Engine::s_Instance = nullptr;

void Engine::LoadTextures()
{
	static const char *const s_TexNames[Object2D::eTypeCount] = 
	{
		"brick.bmp",
		"bullet.bmp",
		"coin.bmp",
		"explode.bmp",
		"flower.bmp",
		"mario.bmp",
		"monster.bmp",
		"mushroom.bmp",
		"tile.bmp",
		"turtle.bmp"
	};

	for (uint32_t i = 0U; i < Object2D::eTypeCount; ++i)
	{
		Image *pImage = new Image();
		assert(pImage);
		pImage->Create(s_TexNames[i]);

		m_Textures.push_back(pImage);
	}
}

void Engine::LoadMaps()
{
	static const char *const s_MapNames[] =
	{
		"level0.dat",
		"level1.dat"
	};

	for (uint32_t i = 0U; i < ARRAYSIZE(s_MapNames); ++i)
	{
		Map *pMap = new Map();
		assert(pMap);
		pMap->Create(s_MapNames[i]);

		m_Maps.push_back(pMap);
	}

	m_CurrentMap = m_Maps.at(0U);
}

void Engine::DrawMap()
{
	assert(m_CurrentMap);

	static const uint32_t s_MapObjectW = 32U;
	static const uint32_t s_MapObjectH = 32U;
	static const uint32_t s_TileTypeCount = 16U;
	static Object2D s_MapObject(Object2D::eTile);
	static const uint32_t s_InvertImageY = (s_MapObject.GetImage()->Height() - s_MapObjectH);

	uint32_t min = m_CurrentMap->Left() / s_MapObjectW;
	uint32_t max = min + 20U;  /// WindowWidth / s_MapObjectW
	int32_t deltaLeft = m_CurrentMap->Left() - min * s_MapObjectW;

	for (uint32_t i = 0U; i < m_CurrentMap->Height(); ++i)
	{
		uint32_t left = 0U;

		for (uint32_t j = min; j <= max; ++j)
		{
			const char mark = m_CurrentMap->StaticMark(i * m_CurrentMap->Width() + j);

			if (mark > 0)
			{
				uint32_t imageX = mark % s_TileTypeCount * s_MapObjectW;
				uint32_t imageY = mark / s_TileTypeCount * s_MapObjectH;

				uint32_t width = s_MapObjectW;
				uint32_t height = s_MapObjectH;

				if (min == j)
				{
					imageX += deltaLeft;
				}
				else if (max == j)
				{
					width -= (s_MapObjectW - deltaLeft);
				}

				s_MapObject.UpdateArea(i * s_MapObjectH, left, width, height, imageX, imageY + s_InvertImageY);

				m_Renderer->DrawObject(&s_MapObject);
			}

			if (min == j)
			{
				left += (s_MapObjectW - deltaLeft);
			}
			else
			{
				left += s_MapObjectW;
			}
		}
	}
}

void Engine::DrawObjects()
{
}

void Engine::Init(HWND hWnd, uint32_t width, uint32_t height)
{
	assert(!m_Renderer);
	m_Renderer = new Renderer();
	m_Renderer->Init(hWnd, width, height);

	LoadTextures();

	LoadMaps();

#if 0
	Object2D *pTest = new Object2D(Object2D::eTile);
	pTest->UpdateArea(64U, 128U, 32U, 32U, 256U, 32U);
	m_Objects.push_back(pTest);
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
	m_Renderer->Clear();
#if 0 
	m_Renderer->DrawObject(m_Objects.at(0));
#endif

#if 1
	DrawMap();
#endif

	m_Renderer->Flip();
}

void Engine::Resize(uint32_t width, uint32_t height)
{
	if (m_Renderer)
	{
		m_Renderer->Resize(width, height);
	}
}

Engine::~Engine()
{
	for (size_t i = 0U; i < m_Textures.size(); ++i)
	{
		SafeDelete(m_Textures.at(i));
	}
	m_Textures.clear();

	for (size_t i = 0U; i < m_Objects.size(); ++i)
	{
		SafeDelete(m_Objects.at(i));
	}
	m_Objects.clear();

	for (size_t i = 0U; i < m_Maps.size(); ++i)
	{
		SafeDelete(m_Maps.at(i));
	}
	m_Maps.clear();

	SafeDelete(m_Renderer);
}
