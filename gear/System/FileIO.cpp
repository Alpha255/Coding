#include "FileIO.h"
#include <fstream>

namespaceStart(gear)

void fileIO::Read(eRead readType)
{
	if (!m_Data)
	{
		m_Data = std::make_shared<byte>();

		int32_t mode = readType == eBinary ? std::ios::in : std::ios::in | std::ios::binary;

		std::ifstream fileStream(fullPath(), mode);
		assert(fileStream.is_open());

		byte *pMemory = new byte[size()]();
		fileStream.read((char8_t *)pMemory, size());
		fileStream.close();

		m_Data.reset(pMemory);
	}
}

namespaceEnd(gear)