#pragma once

#include <stdint.h>

#include "UNameEnum.h"

class UName
{
public:
	enum eFindName
	{
		eFind,
		eAdd,
		eReplace
	};

	UName(const char *pInName, int32_t inNum = 0, eFindName = eAdd, bool bSplitName = true, int32_t hardcodeIndex = -1);
	UName(eUNameEnum hardcodeIndex, const char *pName);
protected:
	void Init(const char *pInName, int32_t inNum, eFindName eType, bool bSplitName, int32_t hardcodeIndex);
	bool SplitName(const char *pInName, char *pOutName, size_t outNameLen, int32_t &outNum);
	uint16_t GetNonCasePreservingHash(const char *pSrc);
	uint16_t GetCasePreservingHash(const char *pSrc);
private:
	int32_t m_ComparisonIndex = 0;
	int32_t m_DisplayIndex = 0;
	uint32_t m_Number = 0;
};
