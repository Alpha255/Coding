#pragma once

#include <assert.h>
#include <intrin.h>
#include <memory>

#include "UName.h"

class UNameHashTable
{
public:
	enum
	{
		eNameSize = 1024,
		eNameHashBucketCount = 65536,
		eMaxHardcodedNameIndex = 602
	};

	enum eComparisonMode
	{
		eIgnoreCase,
		eCaseSensitive
	};

	inline static UNameHashTable *Instance()
	{
		if (nullptr == s_Instance)
		{
			s_Instance = new UNameHashTable();
		}

		assert(s_Instance);
		return s_Instance;
	}

	inline static void Destory()
	{
		if (s_Instance)
		{
			delete s_Instance;
			s_Instance = nullptr;
		}
	}

	void InitGCRCTable();

	bool RegisterUName(const char *pInName, UName::eAction eAct, int32_t hardcodeIndex,
		uint16_t nonCasePreservingHash, uint16_t casePreservingHash, int32_t &outComparisonIndex, int32_t &outDisplayIndex);
	bool FindOrAddUNameEntry(const char *pInName, UName::eAction eAct, eComparisonMode mode, uint16_t hash, int32_t &outIndex);
protected:
	struct UNameEntry
	{
		int32_t Index = 0;
		UNameEntry *HashNext = nullptr;
		char AnsiName[eNameSize] = {};
	};

	template <int32_t TMaxElems, int32_t TElemsPerChunk> struct NameEntryArray
	{
		int32_t ExpandChunks(int32_t numToAdd)
		{
			assert(NumElems + numToAdd <= TMaxElems);
			int32_t result = NumElems;

			int32_t index = NumElems + numToAdd - 1;
			assert(index >= 0 && index < TMaxElems);
			int32_t chunkIndex = index / TElemsPerChunk;
			
			while (true)
			{
				if (chunkIndex < NumChunk)
				{
					break;
				}

				assert(Chunks[chunkIndex] == nullptr);
				Chunks[chunkIndex] = new UNameEntry*[TElemsPerChunk]();
				++NumChunk;
			}

			assert(chunkIndex < NumChunk && Chunks[chunkIndex]);

			NumElems += numToAdd;
			return result;
		}

		UNameEntry* & operator[](int32_t index)
		{
			int32_t chunkIndex = index / TElemsPerChunk;
			int32_t withinChunkIndex = index % TElemsPerChunk;
			assert(index < NumElems && index >= 0 && chunkIndex < NumChunk && index < TMaxElems);

			UNameEntry **ppEntry = Chunks[chunkIndex];
			assert(ppEntry);

			UNameEntry **ppResult = ppEntry + withinChunkIndex;
			assert(ppResult);

			return *ppResult;
		}

		enum
		{
			eChunkSize = (TMaxElems + TElemsPerChunk - 1) / TElemsPerChunk
		};

		UNameEntry **Chunks[eChunkSize] = {};

		int32_t NumElems = 0;
		int32_t NumChunk = 0;
	};

	typedef NameEntryArray<2 * 1024 * 1024, 16384> TNameEntryArray;

	UNameHashTable() = default;
	~UNameHashTable() = default;

	UNameEntry *MakeNameEntry(const char *pName, int32_t index);
private:
	static UNameHashTable *s_Instance;

	UNameEntry *m_NameHashHead[eNameHashBucketCount] = {};
	UNameEntry *m_NameHashTail[eNameHashBucketCount] = {};

	TNameEntryArray m_TNameEntryArray;

	bool m_bInited = false;
};
