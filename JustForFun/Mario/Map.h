#pragma once

#include "Common.h"

class Map
{
public:
	Map() = default;
	~Map()
	{
		SafeDeleteArray(m_StaticMarks);
		SafeDeleteArray(m_DynamicMarks);
	}

	void Create(const char *pFileName);

	inline void Update(int32_t x, int32_t y, char data)
	{
		uint32_t index = y * m_Width + x;

		m_DynamicMarks[index] = data;
	}

	inline void HorizontalScrolling(int32_t delta)
	{
		m_Left += delta;
		m_Left = m_Left < 0 ? 0 : m_Left;
	}

	inline int32_t Left()
	{
		return m_Left;
	}

	inline uint32_t Width()
	{
		assert(m_Width);
		return m_Width;
	}

	inline uint32_t Height()
	{
		assert(m_Height);
		return m_Height;
	}

	inline char StaticMark(uint32_t index)
	{
		assert(m_StaticMarks);
		return m_StaticMarks[index];
	}
protected:
private:
	uint32_t m_DarkBg = 0U;
	uint32_t m_Width = 0U;
	uint32_t m_Height = 0U;

	int32_t m_Left = 0U;

	char *m_StaticMarks = nullptr;
	char *m_DynamicMarks = nullptr;
};
