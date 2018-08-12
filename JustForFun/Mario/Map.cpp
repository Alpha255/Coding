#include "Map.h"
#include "System.h"
#include "Image.h"

#include <string>
#include <fstream>

void Map::Create(const char *pFileName)
{
	assert(pFileName);

	std::string filePath = System::ResourceFilePath(pFileName, System::eRawData);
	std::ifstream mapFile(filePath, std::ios::in);

	assert(mapFile.good());

	mapFile >> m_DarkBg;

	mapFile >> m_Width >> m_Height;
	assert(m_Width && m_Height);
	size_t size = m_Width * m_Height;
	char *pStaticMarks = new char[size]();
	char *pDynamicMarks = new char[size]();
	assert(pStaticMarks && pDynamicMarks);

	/// Static objects, such cloud, hill, etc.
	char *pObject = pStaticMarks;
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
	pObject = pDynamicMarks;
	for (uint32_t i = 0U; i < m_Height; ++i)
	{
		for (uint32_t j = 0U; j < m_Width; ++j)
		{
			mapFile >> content;
			*pObject++ = (char)content - 1;
		}
	}

	/// NPCs

	m_StaticMarks.reset(pStaticMarks);
	m_DynamicMarks.reset(pDynamicMarks);
	m_ImageObject = Object2D(Object2D::eTile);
}
