#pragma once

#include <fstream>
#include <vector>
#include <unordered_map>

class StatFile
{
public:
	StatFile(const char *pInFile);
	~StatFile();
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

	enum eStatMetaFlag
	{
		eSendingName = 0x40,
		eShift = 0x07,
		eStatShift = 0x09
	};

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
	};

	struct StatMessage
	{
		struct NameInfo
		{
			int32_t Index = 0;
			int32_t Number = 0;
		}Name;
	};

	bool IsWithHeader(std::ifstream &inFileStream);
	void ReadString(std::ifstream &inFileStream, std::string &outStr);
	void ReadHeader(std::ifstream &inFileStream);
	void ReadNameInfo(std::ifstream &inFileStream, StatMessage::NameInfo &outNameInfo);
	void ReadMessage(std::ifstream &inFileStream, StatMessage &outMsg);
	void ReadNamesAndMetaDataMsgs(std::ifstream &inFileStream);
private:
	bool m_bReady = false;
	bool m_bByteSwapping = false;

	StatHeader m_Header;
	std::vector<StatMessage> m_StatMsgs;
	std::unordered_map<int32_t, int32_t> m_NameIndexMap;
};
