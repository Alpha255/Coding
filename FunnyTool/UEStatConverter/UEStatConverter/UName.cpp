#include "UName.h"

#include <string>
#include <assert.h>
#include <algorithm>

UName::UName(const char *pInName, int32_t inNum, eFindName eType, bool bSplitName, int32_t hardcodeIndex)
{
	char temp[eNameSize] = {};
	int32_t tempNum = 0;

	if (inNum == 0 && bSplitName)
	{
		SplitName(pInName, temp, eNameSize, tempNum);
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
