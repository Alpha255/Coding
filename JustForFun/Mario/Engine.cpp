#include "Engine.h"

#include "Image.h"
#include "Objects.h"
#include "Map.h"
#include "D3DEngine.h"

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

				///m_Renderer->DrawObject(pMapObject);
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

void Engine::DrawObjects()
{
}

void Engine::Init(HWND hWnd, uint32_t width, uint32_t height)
{
	D3DEngine::Instance().Initialize(hWnd, width, height, true);

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
	///m_Renderer->Clear();
#if 0 
	m_Renderer->DrawObject(m_Objects.at(0));
#endif

#if 1
	DrawMap();
#endif

	///m_Renderer->Flip();
}

void Engine::Resize(uint32_t width, uint32_t height)
{
	D3DEngine::Instance().Resize(width, height);
}
