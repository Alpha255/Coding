#include "StatFile.h"

#include <assert.h>

StatFile::StatFile(const char *pInFile)
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

	m_bReady = true;
}

bool StatFile::IsWithHeader(std::ifstream &inFileStream)
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

void StatFile::ReadString(std::ifstream &inFileStream, std::string &outStr)
{
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
	outStr = pData;
	delete pData;
	pData = nullptr;
}

void StatFile::ReadHeader(std::ifstream &inFileStream)
{
	inFileStream.read((char*)&m_Header.Version, sizeof(uint32_t));

	ReadString(inFileStream, m_Header.Platform);

	inFileStream.read((char*)&m_Header.FrameTableOffset, sizeof(uint64_t));
	inFileStream.read((char*)&m_Header.NameTableOffset, sizeof(uint64_t));
	inFileStream.read((char*)&m_Header.NumNames, sizeof(uint64_t));
	inFileStream.read((char*)&m_Header.MetaDataMsgOffset, sizeof(uint64_t));
	inFileStream.read((char*)&m_Header.NumMetaDataMsgs, sizeof(uint64_t));

	uint32_t rawStatFile = 0;
	inFileStream.read((char*)&rawStatFile, sizeof(uint32_t));
	m_Header.RawStatFile = !!rawStatFile;
}

void StatFile::ReadNameInfo(std::ifstream &inFileStream, StatMessage::NameInfo &outNameInfo)
{
	int32_t index = 0;
	inFileStream.read((char*)&index, sizeof(int32_t));

	int32_t number = 0;
	inFileStream.read((char*)&number, sizeof(int32_t));

	if (number & (eSendingName << (eShift + eStatShift)))
	{
		std::string name;
		ReadString(inFileStream, name);
	}
	else
	{

	}
}

void StatFile::ReadMessage(std::ifstream &inFileStream, StatMessage &outMsg)
{
	ReadNameInfo(inFileStream, outMsg.Name);
}

void StatFile::ReadNamesAndMetaDataMsgs(std::ifstream &inFileStream)
{
	/// Read NameInfo, build name-index map
	inFileStream.seekg(m_Header.NameTableOffset, std::ios::beg);
	StatMessage::NameInfo tempName;
	ReadNameInfo(inFileStream, tempName);

	/// Read StatMessages
	inFileStream.seekg(m_Header.MetaDataMsgOffset, std::ios::beg);

	m_StatMsgs.resize(m_Header.NumMetaDataMsgs);
	for (uint32_t i = 0U; i < m_Header.NumMetaDataMsgs; ++i)
	{
		ReadMessage(inFileStream, m_StatMsgs[i]);
	}
}

StatFile::~StatFile()
{
}
