#include "System.h"

NamespaceBegin(System)

std::string ResourceFileDirectory(eResourceType resType)
{
	static const char* s_ResourcePath[eResourceType::eResTypeCount] =
	{
		"\\Resource\\Shaders\\",
		"\\Resource\\Textures\\",
		"\\Resource\\SDKMeshs\\",
		"\\Resource\\TxtMeshs\\",
		"\\Resource\\ObjMeshs\\",
		"\\Resource\\Sounds\\",
		"\\Resource\\RawData\\",
	};
	static char directory[MAX_PATH]{};
	::GetModuleFileNameA(::GetModuleHandle(nullptr), directory, MAX_PATH);

	std::string appDir(directory);
	std::string resFileDir = appDir.substr(0, appDir.rfind("\\"));
	resFileDir += s_ResourcePath[resType];

	return resFileDir;
}

std::string ResourceFilePath(const char *pFileName, eResourceType resType)
{
	std::string path = ResourceFileDirectory(resType);

	path += pFileName;

	return path;
}

bool IsStrEndwith(const std::string &srcStr, const char *pPostfix)
{
	size_t findPos = srcStr.rfind(pPostfix);
	return (findPos != std::string::npos) && ((srcStr.length() - findPos) == strlen(pPostfix));
}

void RecursiveFileList(std::vector<std::string> &outFileList, const char *pTargetDir)
{
	char rootDir[MAX_PATH] = {};
	strcpy_s(rootDir, pTargetDir);
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
				strcpy_s(subDir, pTargetDir);
				strcat_s(subDir, "\\");
				strcat_s(subDir, findData.cFileName);

				RecursiveFileList(outFileList, subDir);
			}
			else
			{
				std::string fileName(findData.cFileName);
				std::string fileNameFull(pTargetDir);
				fileNameFull += '\\';
				fileNameFull += fileName;
				outFileList.push_back(fileNameFull);
			}
		}

		if (!::FindNextFileA(hFileHandle, &findData))
		{
			break;
		}
	}
}

void RecursiveFileListWithFilter(std::vector<std::string> &outFileList, const char *pTargetDir, const char *pFilter)
{
	char rootDir[MAX_PATH] = {};
	strcpy_s(rootDir, pTargetDir);
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
				strcpy_s(subDir, pTargetDir);
				strcat_s(subDir, "\\");
				strcat_s(subDir, findData.cFileName);

				RecursiveFileListWithFilter(outFileList, subDir, pFilter);
			}
			else
			{
				std::string fileName(findData.cFileName);
				std::string fileNameFull(pTargetDir);
				fileNameFull += '\\';
				fileNameFull += fileName;
				if (IsStrEndwith(fileName, pFilter))
				{
					outFileList.push_back(fileNameFull);
				}
			}
		}

		if (!::FindNextFileA(hFileHandle, &findData))
		{
			break;
		}
	}
}

void GetFileListFromDirectory(std::vector<std::string> &outFileList, const char *pTargetDir, const char *pFilter)
{
	assert(pTargetDir);

	outFileList.clear();

	if (pFilter)
	{
		RecursiveFileListWithFilter(outFileList, pTargetDir, pFilter);
	}
	else
	{
		RecursiveFileList(outFileList, pTargetDir);
	}
}

bool IsValidDirectory(const char *pTargetDir)
{
	assert(pTargetDir);

	::WIN32_FIND_DATAA fileData = {};
	bool bValid = false;

	::HANDLE fileHandle = ::FindFirstFileA(pTargetDir, &fileData);
	if ((fileHandle != INVALID_HANDLE_VALUE) && (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		bValid = true;
	}
	::FindClose(fileHandle);

	return bValid;
}

NamespaceEnd(System)