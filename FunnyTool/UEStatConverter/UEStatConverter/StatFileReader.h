#pragma once

#include <fstream>

class StatFileReader
{
public:
	StatFileReader(const char *pInFile);
	~StatFileReader();
protected:
	enum eStatMagic : uint32_t
	{
		eMagicNoHeader = 0x7E1B83C1,
		eMagicNoHeaderSwapped = 0xC1831B7E,
		eMagicHeader = 0x10293847,
		eMagicHeaderSwapped = 0x47382910
	};

	void ReadHeader(std::ifstream &inFileStream);
private:
	bool m_bReady = false;
	bool m_bByteSwapping = false;
	struct StatHeader
	{
		uint32_t Version = 0U;
		std::string Platform = "";
		uint64_t FrameTableOffset;
		uint64_t NameTableOffset;
		uint64_t NumNames;
		uint64_t MetaDataMsgOffset;
		uint64_t NumMetaDataMsgs;
		bool RawStatFile;
	}m_Header;
};
