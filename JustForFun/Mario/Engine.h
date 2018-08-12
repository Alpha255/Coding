#pragma once

#include "Common.h"
#include "Image.h"
#include "Objects.h"
#include "Map.h"
#include "Renderer.h"

class Engine
{
public:
	Engine(const Engine &) = delete;
	void operator= (const Engine &) = delete;

	static Engine &Instance()
	{
		if (!s_Instance)
		{
			s_Instance = std::unique_ptr<Engine, std::function<void(Engine *)>>
				(new Engine(), [](Engine *pEngine) { SafeDelete(pEngine) });
			assert(s_Instance);
		}

		return *s_Instance;
	}

	void Init(HWND hWnd, uint32_t width, uint32_t height);

	void Update(float elapseTime, float totalTime);
	void RenderScene();

	void HandleInput(uint32_t msg, WPARAM wParam, LPARAM lParam);

	void Resize(uint32_t width, uint32_t height);

	inline const class Image *GetImage(uint32_t index) const
	{
		assert(index < m_Textures.size());
		return &m_Textures[index];
	}
protected:
	Engine() = default;
	~Engine() = default;

	void LoadTextures();
	void LoadMaps();

	void DrawMap();
	void DrawObjects();
private:
	static std::unique_ptr<Engine, std::function<void(Engine *)>> s_Instance;

	Map *m_CurrentMap = nullptr;

	std::unique_ptr<Renderer> m_Renderer;

	std::array<Image, Object2D::eTypeCount> m_Textures;
	std::array<Map, Map::eMapCount> m_Maps;
};
