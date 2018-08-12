#pragma once

#include "Common.h"
#include "Object2D.h"
#include "Image.h"

class Map
{
public:
	enum eMapAttribute
	{
		eObjectWidth = 32U,
		eObjectHeight = 32U,
		eObjectCount = 16U,
		eMapCount = 2U
	};

	Map() = default;
	~Map() = default;

	void Create(const char *pFileName);

	inline void Update(int32_t x, int32_t y, char data)
	{
		uint32_t index = y * m_Width + x;

		m_DynamicMarks.get()[index] = data;
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

	inline char GetStaticMark(uint32_t index)
	{
		assert(m_StaticMarks);
		return m_StaticMarks.get()[index];
	}

	inline Object2D *GetMapObject()
	{
		return &m_ImageObject;
	}

	inline uint32_t GetInvertImageY() const
	{
		return m_ImageObject.GetImage()->Height() - Map::eObjectHeight;
	}

protected:
private:
	uint32_t m_DarkBg = 0U;
	uint32_t m_Width = 0U;
	uint32_t m_Height = 0U;

	int32_t m_Left = 0U;

	Object2D m_ImageObject;

	std::shared_ptr<char> m_StaticMarks = nullptr;
	std::shared_ptr<char> m_DynamicMarks = nullptr;
};
