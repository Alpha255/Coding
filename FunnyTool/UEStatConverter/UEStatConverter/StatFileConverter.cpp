#include "StatFileConverter.h"
#include "CommandLine.h"
#include "StatFile.h"
#include "CSVWriter.h"

#include <iostream>
#include <thread>
#include <direct.h>

void CreateSubDir(const char *pSubDir)
{
	assert(pSubDir);

	if (!CommandLine::IsKeepFileInSrcDir())
	{
		char outDir[MAX_PATH] = {};
		strcpy_s(outDir, CommandLine::GetOutFileDir());
		strcat_s(outDir, "\\");
		strcat_s(outDir, pSubDir);

		///if (::mkdir(outDir) != 0)
		if (::CreateDirectoryA(outDir, nullptr) == 0)
		{
			assert(!"Failed to create file directory!!!!");
		}
	}
}

void StatFileConverter::BuildStatFileList(const char *pInFileDir)
{
	char rootDir[MAX_PATH] = {};
	strcpy_s(rootDir, pInFileDir);
	strcat_s(rootDir, "\\*.*");

	::WIN32_FIND_DATAA findData = {};
	::HANDLE hFileHandle = ::FindFirstFileA(rootDir, &findData);
	while (true)
	{
		if (findData.cFileName[0] != '.')
		{
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				char subDir[MAX_PATH] = {};
				strcpy_s(subDir, pInFileDir);
				strcat_s(subDir, "\\");
				strcat_s(subDir, findData.cFileName);

				///CreateSubDir(findData.cFileName);
				BuildStatFileList(subDir);
			}
			else
			{
				std::string fileName(findData.cFileName);
				if (IsEndwithStr(fileName, ".ue4stats"))
				{
					std::string inFile(pInFileDir);
					std::string outFileNameFix = inFile.substr(inFile.rfind('\\') + 1U);

					inFile += '\\';
					inFile += fileName;

					m_FileList.push_back(inFile);
				}
			}
		}

		if (!::FindNextFileA(hFileHandle, &findData))
		{
			break;
		}
	}
}

void StatFileConverter::DoConvert()
{
	::SYSTEM_INFO sysInfo = {};
	::GetSystemInfo(&sysInfo);
	uint32_t threadNum = sysInfo.dwNumberOfProcessors;
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
		StatFile statFile(m_FileList.at(i).c_str(), CommandLine::GetStatList());
		///CSVWriter writer("test.csv");
		///writer.WriteRow("Frame, Name, Value\n");
	}
}
