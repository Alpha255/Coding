#include "Map.h"

#include <string>
#include <fstream>
#include "Utility.h"
#include "Image.h"

void Map::Create(const char *pFileName)
{
	assert(pFileName);

	std::string filePath = Utility::ResourceFilePath(pFileName, Utility::eRawData);
	std::ifstream mapFile(filePath, std::ios::in);

	if (!mapFile.good())
	{
		assert(0);
	}

	mapFile >> m_DarkBg;

	mapFile >> m_Width >> m_Height;
	assert(m_Width && m_Height);
	size_t size = m_Width * m_Height;
	m_StaticMarks = new char[size]();
	m_DynamicMarks = new char[size]();
	assert(m_StaticMarks && m_DynamicMarks);

	/// Static objects, such cloud, hill, etc.
	char *pObject = m_StaticMarks;
	int content = 0;
	for (uint32_t i = 0U; i < m_Height; ++i)
	{
		for (uint32_t j = 0U; j < m_Width; ++j)
		{
			mapFile >> content;
			*pObject++ = (char)content - 1;
		}
	}

	/// Dynamic objects, such as coin mark, brick, tube, etc.
	pObject = m_DynamicMarks;
	for (uint32_t i = 0U; i < m_Height; ++i)
	{
		for (uint32_t j = 0U; j < m_Width; ++j)
		{
			mapFile >> content;
			*pObject++ = (char)content - 1;
		}
	}

	/// NPCs
}
