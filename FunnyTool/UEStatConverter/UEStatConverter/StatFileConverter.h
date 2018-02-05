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
private:
	std::vector<std::pair<std::string, std::string>> m_FileList;
};
