#include "StatFileReader.h"

#include <assert.h>

StatFileReader::StatFileReader(const char *pInFile)
{
	assert(pInFile);

	std::ifstream fileStream(pInFile, std::ios::in | std::ios::binary);
	assert(fileStream.good());

	ReadHeader(fileStream);

	fileStream.close();
}

StatFileReader::~StatFileReader()
{
}

void StatFileReader::ReadHeader(std::ifstream &inFileStream)
{
	uint32_t magic = 0U;
	inFileStream.read((char*)&magic, sizeof(uint32_t));
	
	bool bHeader = false;
	if (magic == eMagicNoHeader)
	{

	}
	else if (magic == eMagicNoHeaderSwapped)
	{
		m_bByteSwapping = true;
	}
	else if (magic == eMagicHeader)
	{
		bHeader = true;
	}
	else if (magic == eMagicHeaderSwapped)
	{
		bHeader = true;
		m_bByteSwapping = true;
	}
	else
	{
		assert(!"Invalid stat file!!!");
	}

	if (bHeader)
	{

	}
}
