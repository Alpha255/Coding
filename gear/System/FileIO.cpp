#include "FileIO.h"
#include <fstream>

namespaceStart(gear)

void fileIO::Read(eRead readType)
{
	if (!m_Data)
	{
		int32_t mode = readType == eBinary ? std::ios::in : std::ios::in | std::ios::binary;

		std::ifstream fileStream(m_File->fullPath(), mode);
		assert(fileStream.is_open());

		byte *pMemory = new byte[m_File->size()]();
		fileStream.read((char8_t *)pMemory, m_File->size());
		fileStream.close();

		m_Data.reset(pMemory);
	}
}

namespaceEnd(gear)