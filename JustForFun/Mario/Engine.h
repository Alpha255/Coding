#pragma once

#include "Common.h"

#include <vector>

class Engine
{
public:
	Engine(const Engine &) = delete;
	void operator= (const Engine &) = delete;

	static Engine &Instance()
	{
		if (nullptr == s_Instance)
		{
			s_Instance = new Engine();
			assert(s_Instance);
		}

		return *s_Instance;
	}

	static void Destory()
	{
		SafeDelete(s_Instance);
	}

	void Init(HWND hWnd, uint32_t width, uint32_t height);

	void Update(float elapseTime, float totalTime);
	void RenderScene();

	void HandleInput(uint32_t msg, WPARAM lParam, LPARAM wParam);

	inline const class Image *GetImage(uint32_t index) const
	{
		assert(index < m_Textures.size());
		return m_Textures.at(index);
	}
protected:
	Engine() = default;
	~Engine();

	void LoadTextures();
	void LoadMaps();

	void DrawMap();
	void DrawObjects();
private:
	static Engine *s_Instance;

	class Renderer *m_Renderer = nullptr;

	std::vector<class Image *> m_Textures;
	std::vector<class Object2D *> m_Objects;
	std::vector<class Map *> m_Maps;
};
