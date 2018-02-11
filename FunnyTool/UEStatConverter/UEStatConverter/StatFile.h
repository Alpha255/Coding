#pragma once

#include <fstream>
#include <vector>
#include <unordered_map>
#include <assert.h>
#include <Windows.h>

class StatFile
{
public:
	enum eParticleType
	{
		eCascade,
		ePopcornFX,
		eParticleCount
	};

	enum eStatType
	{
		eDrawCalls,
		eFrameTime,
		eGPUTime,
		eMemoryUsage,
		eStatCount
	};

	struct StatsInfo
	{
		uint64_t Frame = 0U;
		char Name[MAX_PATH] = {};
		double Value = 0.0;
	};

	StatFile(const char *pInFile, const std::vector<std::string> &statList);
	~StatFile();

	const std::vector<StatsInfo> &GetStats(eParticleType particleType, eStatType eStat) const
	{
		assert(particleType < eParticleCount && eStat < eStatCount);
		return m_Stats[particleType][eStat];
	}
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

	struct StatFrameInfo
	{
		int64_t FileOffset;
		std::unordered_map<uint32_t, int64_t> ThreadCycles;
	};

	bool IsWithHeader(std::ifstream &inFileStream);
	void ReadString(std::ifstream &inFileStream, std::string &outStr);
	void ReadHeader(std::ifstream &inFileStream);
	void ReadFramesInfo(std::ifstream &inFileStream);
	void ReadStats(std::ifstream &inFileStream);
	void ReadStatsRaw(std::ifstream &/*inFileStream*/)
	{
		assert(0);
	}
	void ReadStatsRegular(std::ifstream &inFileStream);
private:
	bool m_bReady = false;
	bool m_bByteSwapping = false;

	StatHeader m_Header;

	std::vector<StatsInfo> m_Stats[eParticleCount][eStatCount];
};
