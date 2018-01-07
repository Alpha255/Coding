#include "Engine.h"

#include "Image.h"
#include "Object.h"
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
}

void Engine::Init(HWND hWnd, uint32_t width, uint32_t height)
{
	assert(!m_Renderer);
	m_Renderer = new Renderer();
	m_Renderer->Init(hWnd, width, height);

	LoadTextures();

	LoadMaps();

#if 1 
	Object2D *pTest = new Object2D(Object2D::eMario);
	m_Objects.push_back(pTest);
#endif
}

void Engine::Update(float elapseTime, float totalTime)
{

}

void Engine::HandleInput(uint32_t msg, WPARAM lParam, LPARAM wParam)
{
	if (WM_KEYDOWN != msg)
	{
		return;
	}
}

void Engine::RenderScene()
{
	m_Renderer->Clear();
#if 1 
	m_Renderer->DrawObject(m_Objects.at(0));
#endif
	m_Renderer->Flip();
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
