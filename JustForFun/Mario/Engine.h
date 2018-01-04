#pragma once

#include "Common.h"

#include <vector>

class Engine
{
public:
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

	inline void LoadResource()
	{
		LoadTextures();

		LoadLevelData();
	}
protected:
	Engine() = default;
	~Engine() = default;

	void LoadTextures();
	void LoadLevelData();
private:
	static Engine *s_Instance;

	std::vector<class Image *> m_Textures;
};
