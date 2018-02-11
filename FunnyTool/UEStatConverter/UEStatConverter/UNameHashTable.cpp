#include "UNameHashTable.h"
#include "UNameEnum.h"

UNameHashTable *UNameHashTable::s_Instance = nullptr;

void UNameHashTable::InitGCRCTable()
{
	assert(!m_bInited);
	assert((eNameHashBucketCount & (eNameHashBucketCount - 1)) == 0);

	m_TNameEntryArray.ExpandChunks(eMaxHardcodedNameIndex + 1);

	/// Register all hardcoded names.
#define RegisterName(num, nameStr) UName Temp_##nameStr(eUNameEnum(num), (#nameStr));
#include "UnrealNames.inl"
#undef RegisterName

	m_bInited = true;
}

bool UNameHashTable::RegisterUName(const char *pInName, UName::eAction eAct, int32_t hardcodeIndex,
	uint16_t nonCasePreservingHash, uint16_t casePreservingHash, int32_t &outComparisonIndex, int32_t &outDisplayIndex)
{
	assert(pInName);

	bool bFound = FindOrAddUNameEntry(pInName, eAct, eIgnoreCase, nonCasePreservingHash, outComparisonIndex);

	if (bFound && hardcodeIndex < 0)
	{
		const UNameEntry *pEntry = m_TNameEntryArray[outComparisonIndex];
		if (strcmp(pInName, pEntry->AnsiName) != 0)
		{
			if (!FindOrAddUNameEntry(pInName, eAct, eCaseSensitive, casePreservingHash, outDisplayIndex))
			{
				outDisplayIndex = outComparisonIndex;
			}
		}
		else
		{
			outDisplayIndex = outComparisonIndex;
		}
	}

	outDisplayIndex = outComparisonIndex;

	return bFound;
}

bool UNameHashTable::FindOrAddUNameEntry(const char *pInName, UName::eAction eAct, eComparisonMode mode, uint16_t hash, int32_t &outIndex)
{
	if (outIndex < 0)
	{
		for (UNameEntry *pEntry = m_NameHashHead[hash]; pEntry; pEntry = pEntry->HashNext)
		{
			::_mm_prefetch((char const*)(pEntry->HashNext), _MM_HINT_T0);

			bool bEqual = (mode == eIgnoreCase) ? (_stricmp(pInName, pEntry->AnsiName) == 0) : (strcmp(pInName, pEntry->AnsiName) == 0);
			if (bEqual)
			{
				outIndex = pEntry->Index;

				if (eAct == UName::eReplace)
				{
					assert(strlen(pInName) == strlen(pEntry->AnsiName));
					strcpy_s(pEntry->AnsiName, pInName);
				}

				assert(outIndex >= 0);
				return true;
			}
		}

		if (eAct == UName::eFind)
		{
			return false;
		}
	}

	if (outIndex < 0)
	{
		outIndex = m_TNameEntryArray.ExpandChunks(1);
	}
	else
	{
		assert(outIndex < m_TNameEntryArray.NumElems);
	}

	assert(m_TNameEntryArray[outIndex] == nullptr);
	UNameEntry *pNewEntry = MakeNameEntry(pInName, outIndex);
	m_TNameEntryArray[outIndex] = pNewEntry;

	UNameEntry *pOldHashHead = m_NameHashHead[hash];
	UNameEntry *pOldHashTail = m_NameHashTail[hash];
	if (!pOldHashHead)
	{
		assert(!pOldHashTail && m_NameHashHead[hash] == pOldHashHead);

		m_NameHashHead[hash] = pNewEntry;
		m_NameHashTail[hash] = pNewEntry;
	}
	else
	{
		assert(pOldHashTail && pOldHashTail->HashNext == nullptr);

		pOldHashTail->HashNext = pNewEntry;
		m_NameHashTail[hash] = pNewEntry;
	}

	assert(outIndex >= 0);
	return true;
}

UNameHashTable::UNameEntry *UNameHashTable::MakeNameEntry(const char *pName, int32_t index)
{
	UNameEntry *pNewEntry = new UNameEntry();
	assert(pNewEntry);

	pNewEntry->Index = (index << 1) | 0;
	pNewEntry->HashNext = nullptr;
	strcpy_s(pNewEntry->AnsiName, pName);

	return pNewEntry;
}