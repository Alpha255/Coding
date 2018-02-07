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
		eMagicHeaderSwapped = 0x47382910,
		eVersion = 6U,
		eHasCompressedData = 4U
	};

	bool IsWithHeader(std::ifstream &inFileStream);
	void ReadHeader(std::ifstream &inFileStream);
	void ReadNamesAndMetaDataMsgs(std::ifstream &inFileStream);
private:
	bool m_bReady = false;
	bool m_bByteSwapping = false;
	struct StatHeader
	{
		uint32_t Version = 0U;
		std::string Platform = "";
		uint64_t FrameTableOffset = 0U;
		uint64_t NameTableOffset = 0U;
		uint64_t NumNames = 0U;
		uint64_t MetaDataMsgOffset = 0U;
		uint64_t NumMetaDataMsgs = 0U;
		bool RawStatFile = false;

		inline bool IsFinalized()
		{
			return NumMetaDataMsgs > 0U && MetaDataMsgOffset > 0U && FrameTableOffset > 0U;
		}

		inline bool HasCompressedData()
		{
			return Version >= eHasCompressedData;
		}
	}m_Header;
};
