#include "StatFileReader.h"

#include <assert.h>

StatFileReader::StatFileReader(const char *pInFile)
{
	assert(pInFile);

	std::ifstream fileStream(pInFile, std::ios::in | std::ios::binary);
	assert(fileStream.good());

	if (IsWithHeader(fileStream))
	{
		ReadHeader(fileStream);
	}

	assert(m_Header.IsFinalized());
	assert(m_Header.HasCompressedData());
	assert(m_Header.Version >= eVersion);

	ReadNamesAndMetaDataMsgs(fileStream);

	fileStream.close();
}

bool StatFileReader::IsWithHeader(std::ifstream & inFileStream)
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

	return bHeader;
}

void StatFileReader::ReadHeader(std::ifstream &inFileStream)
{
	inFileStream.read((char*)&m_Header.Version, sizeof(uint32_t));

	int32_t saveNum = 0;
	inFileStream.read((char*)&saveNum, sizeof(int32_t));
	bool bLoadUCS2Char = saveNum < 0;
	if (bLoadUCS2Char)
	{
		saveNum = -saveNum;
	}
	assert(saveNum >= 0);
	size_t byte = bLoadUCS2Char ? sizeof(unsigned short) : sizeof(char);
	char *pData = new char[saveNum * byte]();
	inFileStream.read(pData, saveNum * byte);
	m_Header.Platform = pData;
	delete pData;
	pData = nullptr;

	inFileStream.read((char*)&m_Header.FrameTableOffset, sizeof(uint64_t));
	inFileStream.read((char*)&m_Header.NameTableOffset, sizeof(uint64_t));
	inFileStream.read((char*)&m_Header.NumNames, sizeof(uint64_t));
	inFileStream.read((char*)&m_Header.MetaDataMsgOffset, sizeof(uint64_t));
	inFileStream.read((char*)&m_Header.NumMetaDataMsgs, sizeof(uint64_t));

	uint32_t rawStatFile = 0;
	inFileStream.read((char*)&rawStatFile, sizeof(uint32_t));
	m_Header.RawStatFile = !!rawStatFile;
}

void StatFileReader::ReadNamesAndMetaDataMsgs(std::ifstream & inFileStream)
{

}

StatFileReader::~StatFileReader()
{
}
