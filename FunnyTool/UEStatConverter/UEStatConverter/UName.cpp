#include "UName.h"

#include "UCommon.h"
#include "UNameHashTable.h"

#include <string>
#include <assert.h>
#include <algorithm>

#define NameExternalToInternal(x) (x + 1)

UName::UName(const char *pInName, int32_t inNum, eFindName eType, bool bSplitName, int32_t hardcodeIndex)
{
	Init(pInName, inNum, eType, bSplitName, hardcodeIndex);
}

UName::UName(eUNameEnum hardcodeIndex, const char *pName)
{
	Init(pName, 0, eAdd, false, hardcodeIndex);
}

void UName::Init(const char *pInName, int32_t inNum, eFindName eType, bool bSplitName, int32_t hardcodeIndex)
{
	char temp[UNameHashTable::eNameSize] = {};
	int32_t tempNum = 0;

	if (inNum == 0 && bSplitName)
	{
		SplitName(pInName, temp, UNameHashTable::eNameSize, tempNum);
		pInName = temp;
		inNum = NameExternalToInternal(tempNum);
	}

	if (!pInName[0])
	{
		assert(hardcodeIndex < 1);
		m_ComparisonIndex = eUNameEnum::NAME_None;
		m_DisplayIndex = eUNameEnum::NAME_None;
		m_Number = 0;
		return;
	}

	const uint16_t nonCasePreservingHash = GetNonCasePreservingHash(pInName);
	const uint16_t casePreservingHash = GetCasePreservingHash(pInName);
	int32_t comparisonIndex = hardcodeIndex;
	int32_t displayIndex = hardcodeIndex;
	bool bFound = UNameHashTable::Instance()->RegisterUName(pInName, eType, hardcodeIndex, nonCasePreservingHash, casePreservingHash, comparisonIndex, displayIndex);
	if (bFound)
	{
		m_ComparisonIndex = comparisonIndex;
		m_DisplayIndex = displayIndex;
		m_Number = inNum;
	}
	else
	{
		m_ComparisonIndex = eUNameEnum::NAME_None;
		m_DisplayIndex = eUNameEnum::NAME_None;
		m_Number = 0;
	}
}

bool UName::SplitName(const char *pInName, char *pOutName, size_t outNameLen, int32_t &outNum)
{
#define Max_int32 ((int32_t)0x7fffffff)
	bool bResult = false;
	const size_t inNameLen = strlen(pInName);

	if (inNameLen > 0)
	{
		const char *pLast = pInName + (inNameLen - 1);
		const char *pCh = pLast;
		if (*pCh >= '0' && *pCh <= '9')
		{
			while (*pCh >= '0' && *pCh <= '9' && pCh > pInName)
			{
				--pCh;
			}

			if (*pCh == '_')
			{
				if (pCh[1] != '0' || pLast - pCh == 1)
				{
					uint64_t temp = _atoi64(pCh + 1);
					if (temp > Max_int32)
					{
						outNum = (int32_t)temp;
						strncpy_s(pOutName, outNameLen, pInName, std::min<size_t>(pCh - pInName + 1, outNameLen));

						bResult = true;
					}
				}
			}
		}
	}

	return bResult;
#undef Max_int32
}

uint16_t UName::GetNonCasePreservingHash(const char *pSrc)
{
	assert(pSrc);
	assert(Common::CRCTable_DEPRECATED[1] != 0);

	uint32_t hashU32 = 0U;
	while (*pSrc)
	{
		char ch = (char)toupper(*pSrc++);
		uint8_t byte = ch;
		hashU32 = ((hashU32 >> 8) & 0x00FFFFFF) ^ Common::CRCTable_DEPRECATED[(hashU32 ^ byte) & 0x000000FF];
	}

	uint16_t hashU16 = hashU32 & 0xFFFF & (UNameHashTable::eNameHashBucketCount - 1);

	return hashU16;
}

uint16_t UName::GetCasePreservingHash(const char *pSrc)
{
	assert(pSrc);

	uint32_t hashU32 = 0U;
	hashU32 = ~hashU32;
	while (char ch = *pSrc++)
	{
		hashU32 = (hashU32 >> 8) ^ Common::CRCTablesSB8[0][(hashU32 ^ ch) & 0xFF];
		hashU32 = (hashU32 >> 8) ^ Common::CRCTablesSB8[0][(hashU32) & 0xFF];
		hashU32 = (hashU32 >> 8) ^ Common::CRCTablesSB8[0][(hashU32) & 0xFF];
		hashU32 = (hashU32 >> 8) ^ Common::CRCTablesSB8[0][(hashU32) & 0xFF];
	}
	hashU32 = ~hashU32;

	uint16_t hashU16 = hashU32 & 0xFFFF & (UNameHashTable::eNameHashBucketCount - 1);

	return hashU16;
}
