#include "Engine.h"

#include "Image.h"
#include "Object.h"

Engine *Engine::s_Instance = nullptr;

void Engine::LoadTextures()
{
	static const char *const s_Textures[Object2D::eTypeCount] = 
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
		pImage->Create(s_Textures[i]);

		m_Textures.push_back(pImage);
	}
}

void Engine::LoadLevelData()
{
}
