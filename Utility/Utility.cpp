#include "Utility.h"

NamespaceBegin(Utility)

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

void GetFileListFromDirectory(std::vector<std::string> &outFileList, const char *pTargetDir, const char *pFilter)
{
	assert(pTargetDir);

	///outFileList.clear();

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

				GetFileListFromDirectory(outFileList, subDir, pFilter);
			}
			else
			{
				std::string fileName(findData.cFileName);
				std::string fileNameFull(pTargetDir);
				fileNameFull += '\\';
				fileNameFull += fileName;
				if (pFilter && IsStrEndwith(fileName, pFilter))
				{
					outFileList.push_back(fileNameFull);
				}
				///else
				///{
				///	outFileList.push_back(fileNameFull);
				///}
			}
		}

		if (!::FindNextFileA(hFileHandle, &findData))
		{
			break;
		}
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

NamespaceEnd(Utility)