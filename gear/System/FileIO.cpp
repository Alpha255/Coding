#include "FileIO.h"

namespaceStart(gear)

void fileIO::read(eReadAs readAs)
{
	m_Data = std::make_shared<byte>();

	int32_t mode = readAs == eBinary ? std::ios::in | std::ios::binary : std::ios::in;

	std::ifstream fileStream(getFullPath(), mode);
	assert(fileStream.is_open());

	byte *pMemory = new byte[getSize()]();
	fileStream.read((char8_t *)pMemory, getSize());
	fileStream.close();

	m_Data.reset(pMemory);
}

namespaceEnd(gear)