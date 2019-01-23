#include "System.h"
#include "ResourceFile.h"

NamespaceBegin(System)

inline char ToLower(char c)
{
	return (char)::tolower((int)c);
}

bool IsStrEndwith(const char *pStr, const char *pPostfix)
{
	assert(pStr && pPostfix);

	std::string str(pStr);
	std::string postfix(pPostfix);

	std::transform(str.begin(), str.end(), str.begin(), ToLower);
	std::transform(postfix.begin(), postfix.end(), postfix.begin(), ToLower);

	size_t findPos = str.rfind(postfix);
	return (findPos != std::string::npos) && ((str.length() - findPos) == strlen(pPostfix));
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
				if (IsStrEndwith(fileName.c_str(), pFilter))
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

bool FileExists(const char *pFilePath)
{
	assert(pFilePath);

	uint32_t attri = ::GetFileAttributesA(pFilePath);

	return (attri != INVALID_FILE_ATTRIBUTES &&
		!(attri & FILE_ATTRIBUTE_DIRECTORY));
}

inline void GetShaderCode(const char *pTag, std::string &rawCode)
{
	size_t beginIndex = rawCode.find(pTag);
	assert(beginIndex != std::string::npos);
	size_t endIndex = rawCode.find(pTag, beginIndex + strlen(pTag));
	assert(endIndex != std::string::npos);

	rawCode = std::string(rawCode.begin() + beginIndex + strlen(pTag), rawCode.begin() + endIndex);
}

std::string GetShaderCode(const char *pFileName, const char *pRenderTag, uint32_t shaderType)
{
	const char *s_ShaderTag[] =
	{
		"#VertexShader",
		"#HullShader",
		"#DomainShader",
		"#GeometryShader",
		"#PixelShader",
		"#ComputeShader",
	};

	assert(pFileName && pRenderTag && shaderType < _countof(s_ShaderTag));

	ResourceFile shaderFile(pFileName);
	auto pRawCode = std::unique_ptr<uint8_t>(shaderFile.Load());
	std::string rawCode((char *)pRawCode.get());

	GetShaderCode(pRenderTag, rawCode);

	GetShaderCode(s_ShaderTag[shaderType], rawCode);

	return rawCode;
}

void Log(const char *pFormatMsg, ...)
{
	static const size_t s_MsgLength = MAX_PATH * 5U;
	static char message[s_MsgLength] = {};

	va_list ap = nullptr;
	va_start(ap, pFormatMsg);
	_vsnprintf_s(message, s_MsgLength, pFormatMsg, ap);
	va_end(ap);

	strcat_s(message, "\n");
	OutputDebugStringA(message);
}

NamespaceEnd(System)