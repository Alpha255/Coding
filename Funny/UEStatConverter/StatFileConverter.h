#pragma once

#include <vector>
#include <mutex>

class StatFileConverter
{
public:
	StatFileConverter() = default;
	~StatFileConverter() = default;

	void DoConvert();
protected:
	void DoTask(uint32_t startIndex, uint32_t endIndex);
	void UpdateProgress(uint32_t index);
private:
	std::vector<std::string> m_FileList;
	std::mutex m_Mutex;
	uint64_t m_ProcessedFileNum = 0U;
};
