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

	bool RegisterUName(const char *pInName, UName::eFindName findType, int32_t hardcodeIndex, 
		uint16_t nonCasePreservingHash, uint16_t casePreservingHash, int32_t &outComparisonIndex, int32_t &outDisplayIndex);
	bool FindOrAddUNameEntry(const char *pInName, UName::eFindName findType, eComparisonMode mode, uint16_t hash, int32_t &outIndex);
protected:
	struct UNameEntry
	{
		int32_t Index = 0;
		UNameEntry *HashNext = nullptr;
		char AnsiName[eNameSize] = {};
	};

	template <uint32_t TMaxElems, uint32_t TElemsPerChunk> struct NameEntryArray
	{
		int32_t ExpandChunk(int32_t numToAdd)
		{
			assert(NumElems + numToAdd <= TMaxElems);

			uint32_t newSize = NumElems + numToAdd - 1;
			assert(newSize >= 0 && newSize < TMaxElems);
			uint32_t chunkIndex = newSize / TElemsPerChunk;
			
			while (true)
			{
				if (chunkIndex < NumChunk)
				{
					break;
				}

				UNameEntry ***pppChunk = &Chunk[chunkIndex];
				UNameEntry **ppNewChunk = new UNameEntry*[sizeof(UNameEntry*) * TElemsPerChunk]();
#ifdef _X64
				if (::_InterlockedCompareExchange64((int64_t volatile *)pppChunk, (int64_t)ppNewChunk, (int64_t)nullptr))
#else
				if (::_InterlockedCompareExchange((long volatile *)pppChunk, (long)ppNewChunk, nullptr)
#endif
				{
					assert(0);
				}
				else
				{
					++NumChunk;
				}
			}

			assert(chunkIndex < NumChunk && Chunk[chunkIndex]);

			int32_t result = NumElems;
			NumElems += numToAdd;
			return result;
		}

		UNameEntry const* const& operator[](int32_t index) const
		{
			int32_t chunkIndex = index / TElemsPerChunk;
			int32_t withinChunkIndex = index % TElemsPerChunk;
			assert(index < NumChunk && index >= 0);
			assert(chunkIndex < NumChunk);
			assert(index < TMaxElems);

			UNameEntry **ppEntry = Chunk[chunkIndex];
			assert(ppEntry);

			UNameEntry const* const* ppResult = ppEntry + withinChunkIndex;
			assert(ppResult);

			return *ppResult;
		}

		enum
		{
			eChunkSize = (TMaxElems + TElemsPerChunk - 1) / TElemsPerChunk
		};

		UNameEntry **Chunk[eChunkSize] = {};

		int32_t NumElems = 0;
		int32_t NumChunk = 0;
	};

	typedef NameEntryArray<2U * 1024U * 1024U, 16384U> TNameEntryArray;

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
