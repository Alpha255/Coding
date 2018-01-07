#pragma once

#include "Common.h"

class Map
{
public:
	Map() = default;
	~Map()
	{
		SafeDeleteArray(m_StaticObjects);
		SafeDeleteArray(m_DynamicObjects);
	}

	void Create(const char *pFileName);

	void Update(int32_t x, int32_t y, char data);

	inline void HorizontalScrolling(int32_t delta)
	{
		m_Left += delta;
	}
protected:
	void SetObject(int32_t x, int32_t y, char data);
private:
	uint32_t m_DarkBg = 0U;
	uint32_t m_Width = 0U;
	uint32_t m_Height = 0U;

	uint32_t m_Left = 0U;

	class Image *m_pImage = nullptr;

	char *m_StaticObjects = nullptr;
	char *m_DynamicObjects = nullptr;
};
