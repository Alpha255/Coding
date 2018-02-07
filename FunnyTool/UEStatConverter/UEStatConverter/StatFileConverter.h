#pragma once

#include <vector>

class StatFileConverter
{
public:
	StatFileConverter() = default;
	~StatFileConverter() = default;

	void BuildStatFileList();

	void DoConvert();
protected:
	void RecurveFileList(const char *pInFileDir, const char *pOutFileDir, bool bKeepSrcDir);
	inline bool IsEndwithStr(const std::string &src, const char *pTarget)
	{
		size_t findPos = src.rfind(pTarget);
		return (findPos != std::string::npos) && ((src.length() - findPos) == strlen(pTarget));
	}
private:
	///std::vector<std::pair<std::string, std::string>> m_FileList;
	std::vector<std::string> m_FileList;
};
