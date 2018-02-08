#pragma once

#include <stdint.h>

class UName
{
public:
	enum eFindName
	{
		eFind,
		eAdd,
		eReplace,
		eNameSize = 1024
	};

	UName(const char *pInName, int32_t inNum = 0, eFindName = eAdd, bool bSplitName = true, int32_t hardcodeIndex = -1);
protected:
	bool SplitName(const char *pInName, char *pOutName, size_t outNameLen, int32_t &outNum);
private:
};
