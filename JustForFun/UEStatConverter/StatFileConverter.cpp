#include "StatFileConverter.h"
#include "CommandLine.h"
#include "Utility.h"

#include <thread>
#include <iostream>

void StatFileConverter::DoConvert()
{
	Utility::GetFileListFromDirectory(m_FileList, CommandLine::GetInFileDir(), ".ue4stats");

	uint32_t threadNum = std::thread::hardware_concurrency() - 1; /// main thread
	uint32_t totalFileNum = (uint32_t)m_FileList.size();
	uint32_t fileNumPerThread = totalFileNum / threadNum;

	for (uint32_t i = 0U; i < threadNum; ++i)
	{
		uint32_t start = i * fileNumPerThread;
		uint32_t end = start + fileNumPerThread;
		end = (i == (threadNum - 1) ? (totalFileNum - 1) : end);

		std::thread task(&StatFileConverter::DoTask, this, start, end);
		task.join();
	}
}

void StatFileConverter::DoTask(uint32_t startIndex, uint32_t endIndex)
{
	for (uint32_t i = startIndex; i < endIndex; ++i)
	{
		m_Mutex.lock();
		m_ProcessedFileNum += 1;
		double percent = (double)m_ProcessedFileNum / (double)m_FileList.size() * 100.0f;

		const char *pCurFile = m_FileList.at(i).c_str() + m_FileList.at(i).rfind('\\') + 1;

		std::cout << "Processing file: " << pCurFile << " " << percent << "%" << std::endl;
		m_Mutex.unlock();
	}
}
