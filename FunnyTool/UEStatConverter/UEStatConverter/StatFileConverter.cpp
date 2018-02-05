#include "StatFileConverter.h"
#include "CommandLine.h"

#include <iostream>

void StatFileConverter::BuildStatFileList()
{
	const char *pStatFileDir = CommandLine::GetInFileDir();
	const char *pCsvFileDir = CommandLine::GetOutFileDir();

	bool bKeepInSrcDir = (0 == strcmp(pStatFileDir, pCsvFileDir)) ? true : false;

	RecurveFileList(pStatFileDir, pCsvFileDir, bKeepInSrcDir);
}

void StatFileConverter::DoConvert()
{
}

void StatFileConverter::RecurveFileList(const char *pInFileDir, const char *pOutFileDir, bool bKeepSrcDir)
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

				RecurveFileList(subDir, pOutFileDir, bKeepSrcDir);
			}
			else
			{
				std::string fileName(findData.cFileName);
				size_t postFixPos = fileName.rfind(".ue4stats");
				if (std::string::npos != postFixPos)
				{
					std::string inFile(pInFileDir);
					std::string outFileNameFix = inFile.substr(inFile.rfind('\\') + 1U);

					inFile += '\\';
					inFile += fileName;

					std::string outFileName = fileName.substr(0U, postFixPos);
					outFileName += '.';
					outFileName += outFileNameFix;
					///outFileName += ".csv";

					std::string outFile = bKeepSrcDir ? pInFileDir : pOutFileDir;
					outFile += '\\';
					outFile += outFileName;

					m_FileList.push_back(std::make_pair(inFile, outFile));
				}
			}
		}

		if (!FindNextFileA(hFileHandle, &findData))
		{
			break;
		}
	}
}
