#pragma once

#include <vector>
#include <set>

class StatFileConverter
{
public:
	StatFileConverter() = default;
	~StatFileConverter() = default;

	void BuildStatFileList(const char *pInFileDir);

	void DoConvert();
	void DoTask(uint32_t startIndex, uint32_t endIndex);
protected:
	inline bool IsEndwithStr(const std::string &src, const char *pTarget)
	{
		size_t findPos = src.rfind(pTarget);
		return (findPos != std::string::npos) && ((src.length() - findPos) == strlen(pTarget));
	}
private:
	std::vector<std::string> m_FileList;
};
