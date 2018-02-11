#pragma once

#include <stdint.h>

#include "UNameEnum.h"

class UName
{
public:
	enum eAction
	{
		eFind,
		eAdd,
		eReplace
	};

	UName() = default;
	UName(const char *pInName, int32_t inNum = 0, eAction = eAdd, bool bSplitName = true, int32_t hardcodeIndex = -1);
	UName(eUNameEnum hardcodeIndex, const char *pName);
	UName(int32_t comparisonIndex, int32_t displayIndex, int32_t inNum)
		: m_ComparisonIndex(comparisonIndex)
		, m_DisplayIndex(displayIndex)
		, m_Number(inNum)
	{
	}

	inline int32_t GetComparisonIndex()
	{
		return m_ComparisonIndex;
	}

	inline int32_t GetNumber()
	{
		return m_Number;
	}
protected:
	void Init(const char *pInName, int32_t inNum, eAction eAct, bool bSplitName, int32_t hardcodeIndex);
	bool SplitName(const char *pInName, char *pOutName, size_t outNameLen, int32_t &outNum);
	uint16_t GetNonCasePreservingHash(const char *pSrc);
	uint16_t GetCasePreservingHash(const char *pSrc);
private:
	int32_t m_ComparisonIndex = 0;
	int32_t m_DisplayIndex = 0;
	uint32_t m_Number = 0;
};
