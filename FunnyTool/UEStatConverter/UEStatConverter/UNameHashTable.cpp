#include "UNameHashTable.h"
#include "UNameEnum.h"

UNameHashTable *UNameHashTable::s_Instance = nullptr;

void UNameHashTable::InitGCRCTable()
{
	assert(!m_bInited);
	assert((eNameHashBucketCount & (eNameHashBucketCount - 1)) == 0);

	m_TNameEntryArray.ExpandChunk(eMaxHardcodedNameIndex + 1);

	/// Register all hardcoded names.
#define RegisterName(num, nameStr) UName Temp_##nameStr(eUNameEnum(num), (#nameStr));
#include "UnrealNames.inl"
#undef RegisterName

	m_bInited = true;
}

bool UNameHashTable::RegisterUName(const char *pInName, UName::eFindName findType, int32_t hardcodeIndex,
	uint16_t nonCasePreservingHash, uint16_t casePreservingHash, int32_t &outComparisonIndex, int32_t &outDisplayIndex)
{
	assert(pInName);

	bool bFound = FindOrAddUNameEntry(pInName, findType, eIgnoreCase, nonCasePreservingHash, outComparisonIndex);

	outDisplayIndex = outComparisonIndex;

	return bFound;
}

bool UNameHashTable::FindOrAddUNameEntry(const char *pInName, UName::eFindName findType, eComparisonMode mode, uint16_t hash, int32_t &outIndex)
{
	if (outIndex < 0)
	{
		for (UNameEntry *pEntry = m_NameHashHead[hash]; pEntry; pEntry = pEntry->HashNext)
		{
			::_mm_prefetch((char const*)(pEntry->HashNext), _MM_HINT_T0);

			bool bEqual = (mode == eIgnoreCase) ? (stricmp(pInName, pEntry->AnsiName) == 0) : (strcmp(pInName, pEntry->AnsiName) == 0);
			if (bEqual)
			{
				outIndex = pEntry->Index;

				if (findType == UName::eReplace)
				{
					assert(strlen(pInName) == strlen(pEntry->AnsiName));
					strcpy_s(pEntry->AnsiName, pInName);
				}

				assert(outIndex >= 0);
				return true;
			}
		}

		if (findType == UName::eFind)
		{
			return false;
		}
	}

	if (outIndex < 0)
	{
		outIndex = m_TNameEntryArray.ExpandChunk(1);
	}
	else
	{
		assert(outIndex < m_TNameEntryArray.NumElems);
	}

	UNameEntry *pNewEntry = MakeNameEntry(pInName, outIndex);

#ifdef _X64
	if (::_InterlockedCompareExchange64((int64_t volatile *)&m_TNameEntryArray[outIndex], (int64_t)pNewEntry, (int64_t)nullptr) != 0L)
#else
	if (::_InterlockedCompareExchange((long volatile *)&pppChunk, (long)ppNewChunk, nullptr)
#endif
	{
		assert(0);
	}

	UNameEntry *pOldHashHead = m_NameHashHead[hash];
	UNameEntry *pOldHashTail = m_NameHashTail[hash];
	if (!pOldHashHead)
	{
		assert(!pOldHashTail);

#ifdef _X64
		if (::_InterlockedCompareExchange64((int64_t volatile *)&m_NameHashHead[outIndex], (int64_t)pNewEntry, (int64_t)pOldHashHead) != (int64_t)pOldHashHead)
#else
		if (::_InterlockedCompareExchange((long volatile *)pppChunk, (long)ppNewChunk, nullptr)
#endif
		{
			assert(0);
		}

		m_NameHashTail[hash] = pNewEntry;
	}
	else
	{
		assert(pOldHashTail);

#ifdef _X64
		if (::_InterlockedCompareExchange64((int64_t volatile *)&pOldHashTail->HashNext[outIndex], (int64_t)pNewEntry, (int64_t)nullptr) != (int64_t)nullptr)
#else
		if (::_InterlockedCompareExchange((long volatile *)pppChunk, (long)ppNewChunk, nullptr)
#endif
		{
			assert(0);
		}

		m_NameHashTail[hash] = pNewEntry;
	}

	assert(outIndex >= 0);
	return true;
}

UNameHashTable::UNameEntry *UNameHashTable::MakeNameEntry(const char *pName, int32_t index)
{
	UNameEntry *pNewEntry = new UNameEntry();
	assert(pNewEntry);

	_InterlockedExchange((long*)&pNewEntry->Index, (index << 1) | 0);

#ifdef _X64
	::_InterlockedExchange64((int64_t*)(&pNewEntry->HashNext), (int64_t)(nullptr));
#else
	::_InterlockedExchange((long*)(&pNewEntry->HashNext), (long)(nullptr));
#endif

	strcpy_s(pNewEntry->AnsiName, pName);

	return pNewEntry;
}